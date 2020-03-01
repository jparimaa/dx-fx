#include "WcharHelper.h"

namespace fw
{
WcharHelper::WcharHelper(const std::string& s)
{
    const size_t size = s.length() + 1;
    wideChar = new wchar_t[size];
    mbstowcs(wideChar, s.c_str(), size);
}

WcharHelper::~WcharHelper()
{
    delete wideChar;
}

wchar_t* WcharHelper::getWchar() const
{
    return wideChar;
}
} // namespace fw
