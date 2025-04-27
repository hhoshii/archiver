//
// Created by bulat on 27.10.24.
//

#include "reader.h"

Reader::Reader(const int argc, char** argv) {
    int32_t count_of_dashes = 0;
    for (int i = 1; i < argc; ++i) {
        for (std::string arg = argv[i]; const auto& c : arg) {
             if (c == DASH) {
                 count_of_dashes++;
             }
        }
    }

    if (argc <= 1 || count_of_dashes != 1 || (argv[1] != Create && argv[1] != Delete && argv[1] != Help)) {
        is_correct_ = false;
        return;
    }

    if (argv[1] == Create) {
        if (argc <= MinCountForEncode) {
            is_correct_ = false;
            return;
        }

        archive_name_ = argv[2];
        type_ = Type::ENCODE;

        for (int i = 3; i < argc; ++i) {
            files_paths_.push_back(argv[i]);
        }
    } else if (argv[1] == Delete) {
        if (argc != CountForDecode) {
            is_correct_ = false;
            return;
        }

        archive_name_ = argv[2];
        type_ = Type::DECODE;
    } else {
        if (argc != CountForHelp || argv[1] != Help) {
            is_correct_ = false;
            return;
        }

        type_ = Type::HELP;
    }

}

bool Reader::IsCorrect() const {  // NOLINT
    return is_correct_;
}

Type Reader::GetType() const {  // NOLINT
    return type_;
}

std::vector<std::string>& Reader::GetFilesPaths() {
    return files_paths_;
}

std::string Reader::GetArchiveName() {
    return archive_name_;
}

