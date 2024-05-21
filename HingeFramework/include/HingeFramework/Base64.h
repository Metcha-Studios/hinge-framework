#pragma once
#ifndef BASE64_H
#define BASE64_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif // HINGE_EXPORTS

#endif // HINGE_API

#include <string>

namespace hinge_framework {
    extern "C" HINGE_API const char* encodeBase64(const char* plain_text);
    extern "C" HINGE_API const char* decodeBase64(const char* base64_text);
    extern "C" HINGE_API const char* encodeBase64FromStr(const std::string& plain_text);
    extern "C" HINGE_API std::string decodeBase64ToStr(const char* base64_text);
}

#endif // BASE64_H
