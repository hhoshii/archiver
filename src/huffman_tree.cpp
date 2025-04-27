#include "huffman_tree.h"

HuffmanNode::HuffmanNode(const int32_t symbol, const int32_t frequency, HuffmanNode* left, HuffmanNode* right) {
    symbol_ = symbol;
    frequency_ = frequency;
    left_ = left;
    right_ = right;
    parent_ = nullptr;

    if (left_ != nullptr) {
        left_->parent_ = this;
    }

    if (right_ != nullptr) {
        right_->parent_ = this;
    }
}

void HuffmanNode::Build(const int32_t c, std::vector<bool>& code) {
    auto v = this;

    for (bool x : code) {
        HuffmanNode*& son = v->GetSon(x);
        if (son == nullptr) {
            son = new HuffmanNode(0, 0);
            son->parent_ = v;
        }

        v = son;
    }

    v->symbol_ = c;
}

HuffmanNode*& HuffmanNode::GetSon(const bool x) {
    if (x) {
        return right_;
    }
    return left_;
}

HuffmanNode::~HuffmanNode() {
    delete left_;
    delete right_;
}

bool HuffmanNode::IsTerminate() const {
    return left_ == nullptr && right_ == nullptr;
}

int32_t HuffmanNode::GetSymbol() const {
    return symbol_;
}

int32_t HuffmanNode::GetFrequency() const {
    return frequency_;
}

int32_t HuffmanNode::GetLenOfCode() const {
    int32_t len = 0;
    auto current = this;
    while (current->parent_ != nullptr) {
        len++;
        current = current->parent_;
    }

    return len;
}
