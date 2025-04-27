#include "encoder.h"

#include <algorithm>
#include <queue>

Encoder::Encoder(Reader& reader) {
    archive_file_.open(reader.GetArchiveName());
    if (!archive_file_.is_open()) {
        is_correct_ = false;
        return;
    }

    auto write = BitStream();

    for (size_t i = 0; i < reader.GetFilesPaths().size(); i++) {
        EncodeFile(reader.GetFilesPaths()[i], i + 1 == reader.GetFilesPaths().size(), write);
        file_.close();
        if (!is_correct_) {
            archive_file_.close();
            return;
        }
    }

    write.WriteAll(archive_file_);
}

Encoder::~Encoder() {
    file_.close();
    archive_file_.close();
}

void Encoder::EncodeFile(const std::string& file_path, const bool is_end, BitStream& write) {
    file_.open(file_path);

    if (!file_.is_open()) {
        is_correct_ = false;
        return;
    }

    std::unordered_map<int32_t, std::vector<bool>> canonical;
    std::vector<std::pair<int32_t, int32_t>> order;
    std::string file_name;
    const int32_t symbols_count = BuildTable(order, file_path, canonical, file_name);
    file_.close();

    file_.open(file_path);

    if (!file_.is_open()) {
        is_correct_ = false;
        return;
    }

    Write(canonical, file_name, order, symbols_count, is_end, write);
}

int32_t Encoder::BuildTable(std::vector<std::pair<int32_t, int32_t>>& order, const std::string& file_path,
                            std::unordered_map<int32_t, std::vector<bool>>& canonical, std::string& file_name) {

    for (int i = static_cast<int>(file_path.size()) - 1; i >= 0 && file_path[i] != '/'; --i) {
        file_name += file_path[i];
    }

    std::reverse(file_name.begin(), file_name.end());  // NOLINT

    std::unordered_map<int32_t, int32_t> frequency;
    CountFrequency(file_name, frequency);

    std::vector<HuffmanNode*> terminal_nodes(MaxCode, nullptr);
    const auto root = BuildHuffmanTree(frequency, terminal_nodes);

    std::vector<int32_t> code_lens(MaxCode);

    for (int i = 0; i < MaxCode; i++) {
        if (terminal_nodes[i] != nullptr) {
            code_lens[i] = terminal_nodes[i]->GetLenOfCode();
            order.push_back({code_lens[i], i});
        }
    }

    std::stable_sort(order.begin(), order.end());

    GetCanonical(order, canonical);
    delete root;
    return static_cast<int32_t>(frequency.size());
}

void Encoder::CountFrequency(std::string& file_name, std::unordered_map<int32_t, int32_t>& frequency) {
    for (const auto& c : file_name) {
        frequency[c]++;
    }

    frequency[FilenameEnd]++;
    frequency[OneMoreFile]++;
    frequency[ArchiveEnd]++;

    char c = 0;
    while (file_.get(c)) {
        frequency[c]++;
    }
}

HuffmanNode* Encoder::BuildHuffmanTree(std::unordered_map<int32_t, int32_t>& frequency,
                                       std::vector<HuffmanNode*>& terminal_nodes) {

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> heap;

    for (auto& [symbol, count] : frequency) {
        terminal_nodes[symbol] = new HuffmanNode(symbol, count);
        heap.push(terminal_nodes[symbol]);
    }

    while (heap.size() > 1) {
        const auto first = heap.top();
        heap.pop();

        const auto second = heap.top();
        heap.pop();

        heap.push(new HuffmanNode(0, first->GetFrequency() + second->GetFrequency(), first, second));
    }
    return heap.top();
}

void Encoder::GetCanonical(const std::vector<std::pair<int32_t, int32_t>>& order,
                           std::unordered_map<int32_t, std::vector<bool>>& canonical) {

    int32_t current_length = order[0].first;
    std::vector<bool> current_code(current_length);

    for (const auto& [fst, snd] : order) {
        const int32_t length = fst;
        const int32_t symbol_index = snd;

        while (current_length < length) {
            current_code.push_back(false);
            current_length++;
        }

        canonical[symbol_index] = current_code;


        bool flag = false;

        for (int32_t j = current_length - 1; j >= 0; j--) {
            if (!current_code[j]) {
                current_code[j] = true;
                flag = true;
                break;
            }
            current_code[j] = false;
        }

        if (!flag) {
            current_code.push_back(false);
            current_code[0] = true;
            current_length++;
        }

    }
}

void Encoder::Write(std::unordered_map<int32_t, std::vector<bool>>& canonical, std::string& file_name,
                    std::vector<std::pair<int32_t, int32_t>>& order, const int32_t symbols_count, const bool is_end,
                    BitStream& write) {
    write.Write(archive_file_, symbols_count, MaxLen);

    std::vector<int32_t> code_lens_count(MaxCode);

    for (auto& [cnt, code] : order) {
        write.Write(archive_file_, code, MaxLen);
        code_lens_count[canonical[code].size()]++;
    }

    while (code_lens_count.back() == 0) {
        code_lens_count.pop_back();
    }

    for (int i = 1; i < code_lens_count.size(); i++) {
        write.Write(archive_file_, code_lens_count[i], MaxLen);
    }

    for (const auto& c : file_name) {
        write.WriteCode(archive_file_, canonical[c]);
    }

    write.WriteCode(archive_file_, canonical[FilenameEnd]);

    char c = 0;
    while (file_.get(c)) {
        write.WriteCode(archive_file_, canonical[abs(c)]);
    }

    if (is_end) {
        write.WriteCode(archive_file_, canonical[ArchiveEnd]);
    } else {
        write.WriteCode(archive_file_, canonical[OneMoreFile]);
    }
}

bool Encoder::IsCorrect() const {
    return is_correct_;
}
