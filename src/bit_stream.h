#pragma once
#include <fstream>
#include <vector>

constexpr int32_t MaxCode = 520;
constexpr int32_t MaxLen = 9;
constexpr int32_t Eight = 8;
constexpr int32_t FilenameEnd = 256;
constexpr int32_t OneMoreFile = 257;
constexpr int32_t ArchiveEnd = 258;

class BitStream {
public:
    explicit BitStream() : buffer_(0), buffer_size_(0) {}

    void Write(std::ofstream& file, int32_t value, int32_t len);

    void WriteCode(std::ofstream& file, std::vector<bool>& code);

    void WriteAll(std::ofstream& file) const;

    bool ReadSymbol(std::ifstream& file, int32_t& x, int32_t len);

    int32_t ReadSymbol(std::ifstream& file, const int32_t len);


private:
    int32_t buffer_;
    int32_t buffer_size_;
};
