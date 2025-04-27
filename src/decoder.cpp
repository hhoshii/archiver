#include "decoder.h"

Decoder::Decoder(Reader& reader) {
    archive_.open(reader.GetArchiveName());
    if (!archive_.is_open()) {
        is_correct_ = false;
        return;
    }

    do {
    } while (DecodeFile());
    archive_.close();
}

Decoder::~Decoder() {
    archive_.close();
    file_.close();
}

bool Decoder::IsCorrect() const {
    return is_correct_;
}

bool Decoder::DecodeFile() {
    std::vector<std::pair<int32_t, int32_t>> order;

    BitStream read = BitStream();

    ReadOrder(order, read);

    std::unordered_map<int32_t, std::vector<bool>> canonical;

    GetCanonical(order, canonical);

    HuffmanNode* root = BuildTrie(order, canonical);
    const std::string name = ReadName(root, read);

    const bool result = WriteFile(root, name, read);
    file_.close();

    delete root;
    return result;
}

void Decoder::ReadOrder(std::vector<std::pair<int32_t, int32_t>>& order, BitStream& read) {
    int32_t symbols_count = read.ReadSymbol(archive_, MaxLen);

    for (int32_t i = 0; i < symbols_count; ++i) {
        order.push_back({0, read.ReadSymbol(archive_, MaxLen)});
    }

    for (int32_t i = 0, len = 1; i < symbols_count; ++len) {
        size_t cnt = read.ReadSymbol(archive_, MaxLen);
        for (; cnt > 0 && i < symbols_count; --cnt, ++i) {
            order[i].first = len;
        }
    }
}

HuffmanNode* Decoder::BuildTrie(std::vector<std::pair<int32_t, int32_t>>& order,
                                std::unordered_map<int32_t, std::vector<bool>>& canonical) {
    const auto root = new HuffmanNode(0, 0);
    for (auto& [len, c] : order) {
        root->Build(c, canonical[c]);
    }
    return root;
}

int32_t Decoder::FindSymbol(HuffmanNode* root, BitStream& read) {
    if (root == nullptr) {
        return ArchiveEnd;
    }

    while (!root->IsTerminate()) {
        root = root->GetSon(read.ReadSymbol(archive_, 1));

        if (root == nullptr) {
            return ArchiveEnd;
        }
    }
    return root->GetSymbol();
}

std::string Decoder::ReadName(HuffmanNode* root, BitStream& read) {
    std::string name;
    for (int32_t c = FindSymbol(root, read); c != FilenameEnd; c = FindSymbol(root, read)) {
        name.push_back(static_cast<char>(c));
    }

    return name;
}

bool Decoder::WriteFile(HuffmanNode* root, const std::string& name, BitStream& read) {
    file_.open(name);

    for (int32_t c = FindSymbol(root, read);; c = FindSymbol(root, read)) {
        switch (c) {
            case OneMoreFile:
                return true;
            case ArchiveEnd:
                return false;
            default:
                file_ << static_cast<char>(c);
        }
    }
}

void Decoder::GetCanonical(const std::vector<std::pair<int32_t, int32_t>>& order,
                           std::unordered_map<int32_t, std::vector<bool>>& canonical) {
    int32_t current_length = order[0].first;
    std::vector<bool> current_code(current_length);

    for (const auto& [fst, snd] : order) {
        const int32_t length = fst;
        const int32_t symbol_index = abs(snd);

        while (current_length < length) {
            current_code.push_back(false);
            current_length++;
        }

        if (symbol_index < MaxCode) {
            canonical[symbol_index] = current_code;
        }
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
