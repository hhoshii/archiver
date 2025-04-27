#pragma once
#include <vector>
#include "bit_stream.h"
#include "reader.h"
#include "huffman_tree.h"

#include <unordered_map>

class Decoder {
public:
    explicit Decoder(Reader& reader);

    ~Decoder();

    bool IsCorrect() const;


private:
    std::ofstream file_;
    std::ifstream archive_;
    bool is_correct_ = true;

    bool DecodeFile();

    void ReadOrder(std::vector<std::pair<int32_t, int32_t>>& order, BitStream& read);

    static HuffmanNode* BuildTrie(std::vector<std::pair<int32_t, int32_t>>& order, std::unordered_map<int32_t, std::vector<bool>>& canonical);

    int32_t FindSymbol(HuffmanNode* root, BitStream& read);

    std::string ReadName(HuffmanNode* root, BitStream& read);

    bool WriteFile(HuffmanNode* root, const std::string& name, BitStream& read);

    static void GetCanonical(const std::vector<std::pair<int32_t, int32_t>>& order,
                           std::unordered_map<int32_t, std::vector<bool>>& canonical);

};
