#pragma once
#pragma warning(disable : 26451)
#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <cstdlib>
#include <filesystem>
#include <unicode/unistr.h>
std::string utf8ToSjis(const std::string& value)
{
    icu::UnicodeString src(value.c_str(), "utf8");
    int length = src.extract(0, src.length(), NULL, "shift_jis");
    std::vector<char> result(length + 1);
    src.extract(0, src.length(), &result[0], "shift_jis");
    return std::string(result.begin(), result.end() - 1);
}
std::string sjisToUtf8(const std::string& value)
{
    icu::UnicodeString src(value.c_str(), "shift_jis");
    int length = src.extract(0, src.length(), NULL, "utf8");
    std::vector<char> result(length + 1);
    src.extract(0, src.length(), &result[0], "utf8");
    return std::string(result.begin(), result.end() - 1);
}
std::wstring sjisToWs(const std::string& value)
{
    icu::UnicodeString src(value.c_str(), "shift_jis");
    int length = src.extract(0, src.length(), NULL, "utf-16");
    std::vector<char> tmp(length + 1);
    src.extract(0, src.length(), &tmp[0], "utf-16");
    std::vector<unsigned short> result;
    for (size_t i = 2; i < length; i++)
    {
        result.push_back((unsigned short)tmp[i++] & 0xff | ((unsigned short)tmp[i] & 0xff) << 8);
    }
    return std::wstring(result.begin(), result.end());
}
std::string RemExt(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}
void ReplaceExt(std::string& s, const std::string& newExt) {
    std::string::size_type i = s.rfind('.', s.length());
    if (i != std::string::npos) {
        s.replace(i + 1, newExt.length(), newExt);
    }
}
void MakeDir(std::string dir)
{
    std::filesystem::path a = std::filesystem::path{ dir }.parent_path();
    std::filesystem::create_directories(a);
}
char* FileLoad(std::string filename, int* length)
{
    std::ifstream fileStream(filename, std::ios::in | std::ios::binary | std::ios::ate);
    char* buffer;
    std::streampos fileSize;
    if (fileStream.is_open())
    {
        fileSize = fileStream.tellg();
        buffer = new char[fileSize];
        fileStream.seekg(0, std::ios::beg);
        fileStream.read(buffer, fileSize);
        fileStream.close();
        *length = (int)fileSize;
        return buffer;
    }
    else
    {
        std::cout << "Unable to open file >>" << filename << std::endl;
        std::exit(1);
    }
}
void BufferLoad(std::string filename, char* buffer, int* length)
{
    std::ifstream fileStream(filename, std::ios::in | std::ios::binary | std::ios::ate);
    std::streampos fileSize;
    if (fileStream.is_open())
    {
        fileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);
        fileStream.read(buffer, fileSize);
        fileStream.close();
        *length = (int)fileSize;
        return;
    }
    else
    {
        std::cout << "Unable to open file >>" << filename << std::endl;
        std::exit(1);
    }
}
void FileSave(std::string filename, char* buffer, int length)
{
    MakeDir(filename);
    std::ofstream fileStream(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (fileStream.is_open())
    {
        fileStream.write(buffer, length);
        fileStream.flush();
        fileStream.close();
    }
    else std::cout << "Unable to create file >> " << filename << std::endl;
}

