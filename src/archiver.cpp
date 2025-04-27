#include "encoder.h"

#include <string>
#include <iostream>

#include "decoder.h"
#include "reader.h"

constexpr int32_t ERROR = 111;

int main(int argc, char** argv) {
    auto reader = Reader(argc, argv);
    if (!reader.IsCorrect()) {
        return ERROR;
    }

    if (reader.GetType() == Type::ENCODE) {
        auto encoder = Encoder(reader);

        if (!encoder.IsCorrect()) {
            std::cerr << "Some of files doesn't exists" << std::endl;
            return ERROR;
        }
    } else if (reader.GetType() == Type::DECODE) {
        auto decoder = Decoder(reader);

        if (!decoder.IsCorrect()) {
            std::cerr << "error: Error with archive" << std::endl;
            return ERROR;
        }
    } else {
        std::cerr << "archiver -c archive_name file1 [file2 ...] - заархивировать файлы file1, file2, ... и сохранить результат в файл archive_name." << std::endl;
        std::cerr << "archiver -d archive_name - разархивировать файлы из архива archive_name и положить в текущую директорию" << std::endl;
        std::cerr << "archiver -h - вывести справку по использованию программы." << std::endl;
    }

    return 0;
}
