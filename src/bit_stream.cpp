#include "bit_stream.h"


void BitStream::Write(std::ofstream& file, const int32_t value, const int32_t len) {
    for (int i = len - 1; i >= 0; i--) {
        file << ((value >> i) & 1);
    }
    // buffer_ = (buffer_ << len) | value;
    // buffer_size_ += len;
    //
    // while (buffer_size_ >= Eight) {
    //     file << static_cast<unsigned char>((buffer_ & ((1 << buffer_size_) - 1)) >> (buffer_size_ - Eight));
    //     buffer_size_ -= Eight;
    // }
}

void BitStream::WriteCode(std::ofstream& file, std::vector<bool>& code) {
    for (const auto& c : code) {
        Write(file, c, 1);
    }
}

void BitStream::WriteAll(std::ofstream& file) const {
    if (buffer_size_ != 0) {
        file << static_cast<unsigned char>((buffer_ & ((1 << buffer_size_) - 1)) << (Eight - buffer_size_));
    }
}

bool BitStream::ReadSymbol(std::ifstream& file, int32_t& x, const int32_t len) {
    while (buffer_size_ < len) {
        if (file.peek() == EOF) {
            return false;
        }

        const int32_t c = file.get();
        buffer_ = (buffer_ << Eight) | c;
        buffer_size_ += Eight;
    }

    x = (buffer_ & ((1 << buffer_size_) - 1)) >> (buffer_size_ - len);
    buffer_size_ -= len;
    return true;
}

int32_t BitStream::ReadSymbol(std::ifstream& file, const int32_t len) {
    int32_t res = 0;
    char c = 0;
    for (int i = 0; i < len; i++) {
        file.get(c);

        res <<= 1;
        res |= c - '0';
    }
    // ReadSymbol(file, res, len);
    return res;
}