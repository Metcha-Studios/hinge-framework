#pragma once
#ifndef BASE64_WRAPPER_H
#define BASE64_WRAPPER_H

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif

#include <string>

namespace hinge_framework {
    extern const std::string base64_chars;
    extern "C" HINGE_API const char* encodeBase64(const std::string& plain_text);
    extern "C" HINGE_API const char* decodeBase64(const std::string& base64_text);
}

#endif // BASE64_WRAPPER_H
