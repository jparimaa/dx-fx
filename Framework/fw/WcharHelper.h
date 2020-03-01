#pragma once

#include <string>

namespace fw
{
class WcharHelper
{
public:
    WcharHelper(const std::string& s);
    ~WcharHelper();
    wchar_t* getWchar() const;

private:
    wchar_t* wideChar;
};
} // namespace fw
