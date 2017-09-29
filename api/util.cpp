#include <cstdarg>
#include "util.hpp"

// vsnprintf is not declared in C++98, but available in the C standard library
extern "C" int vsnprintf(char *, size_t, const char *, va_list);

using namespace std;

string string_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    size_t size = vsnprintf(NULL, 0, format, args) + 1u;
    char buf[size];
    vsnprintf(buf, size, format, args);
    return string(buf);
}
