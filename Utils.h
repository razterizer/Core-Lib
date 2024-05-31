//
//  Utils.h
//  Core Lib
//
//  Created by Rasmus Anthin on 2024-05-20.
//


#pragma once
#include <cstdarg>  // For va_list, va_start, va_end
#include <cstdio>   // For sscanf and sscanf_s

namespace utils
{

  // #NOTE: On Windows, you cannot use the %n format specifier as it is not supported.
  int sscanf(const char* str, const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    int result;

#ifdef _WIN32
    result = ::vsscanf_s(str, format, args);
#else
    result = ::vsscanf(str, format, args);
#endif

    va_end(args);
    return result;
  }

}
