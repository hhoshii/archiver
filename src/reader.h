#pragma once
#include <string>
#include <vector>

constexpr char DASH = '-';
constexpr std::string_view Create = "-c", Delete = "-d", Help = "-h";
constexpr int32_t MinCountForEncode = 3, CountForDecode = 3, CountForHelp = 3;

enum class Type {
    ENCODE,
    DECODE,
    HELP
};


class Reader {
public:
    Reader(int argc, char** argv);

    bool IsCorrect() const;

    Type GetType() const;

    std::vector<std::string>& GetFilesPaths();

    std::string GetArchiveName();

private:
    Type type_ = Type::HELP;
    bool is_correct_ = true;
    std::vector<std::string> files_paths_;
    std::string archive_name_;
};
