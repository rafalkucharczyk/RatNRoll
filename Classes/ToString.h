/* std::to_string() implementation which is not present on Android */
#ifndef TO_STRING_H
#define TO_STRING_H
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <string>
#include <sstream>

namespace std
{
    template <typename T> std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

#endif /* CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID */
#endif /* TO_STRING_H */
