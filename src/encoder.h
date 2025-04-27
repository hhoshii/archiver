#pragma once

#include "huffman_tree.h"
#include "reader.h"
#include "bit_stream.h"

#include <fstream>
#include <unordered_map>

class Encoder {
public:
    explicit Encoder(Reader& reader);

    ~Encoder();

    bool IsCorrect() const;

private:
    std::ifstream file_;
    std::ofstream archive_file_;
    bool is_correct_ = true;

    void EncodeFile(const std::string& file_path, bool is_end, BitStream& write);

    int32_t BuildTable(std::vector<std::pair<int32_t, int32_t>>& order, const std::string& file_path,
                       std::unordered_map<int32_t, std::vector<bool>>& canonical, std::string& file_name);

    void CountFrequency(std::string& file_name, std::unordered_map<int32_t, int32_t>& frequency);

    static HuffmanNode* BuildHuffmanTree(std::unordered_map<int32_t, int32_t>& frequency,
                                         std::vector<HuffmanNode*>& terminal_nodes);

    static void GetCanonical(const std::vector<std::pair<int32_t, int32_t>>& order,
                             std::unordered_map<int32_t, std::vector<bool>>& canonical);

    void Write(std::unordered_map<int32_t, std::vector<bool>>& canonical, std::string& file_name,
               std::vector<std::pair<int32_t, int32_t>>& order, int32_t symbols_count, bool is_end, BitStream& write);
};
