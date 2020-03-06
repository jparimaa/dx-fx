#pragma once

#include <string>

namespace fw
{
class ToWchar
{
public:
    ToWchar(const std::string& s);
    ~ToWchar();
    wchar_t* getWchar() const;

private:
    wchar_t* wideChar = nullptr;
};

class ToChar
{
public:
    ToChar(const wchar_t* s);
    ~ToChar();
    char* getChar() const;

private:
    char* mbChar = nullptr;
};
} // namespace fw
