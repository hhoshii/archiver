#pragma once
#include <cstdint>
#include <string>
#include <vector>

class HuffmanNode {
public:
    HuffmanNode(const int32_t symbol, const int32_t frequency)
        : symbol_(symbol), frequency_(frequency), left_(nullptr), right_(nullptr), parent_(nullptr) {
    }

    HuffmanNode(int32_t symbol, int32_t frequency, HuffmanNode *left, HuffmanNode *right);

    void Build(int32_t c, std::vector<bool> &code);

    HuffmanNode *&GetSon(bool x);

    ~HuffmanNode();

    bool IsTerminate() const;

    int32_t GetSymbol() const;

    int32_t GetFrequency() const;

    int32_t GetLenOfCode() const;

private:
    int32_t symbol_;
    int32_t frequency_;
    HuffmanNode *left_;
    HuffmanNode *right_;
    HuffmanNode *parent_;
};

struct Compare {
    bool operator()(const HuffmanNode *a, const HuffmanNode *b) const {
        return std::make_pair(a->GetFrequency(), a->GetSymbol()) >
               std::make_pair(b->GetFrequency(), b->GetSymbol());
    }
};
