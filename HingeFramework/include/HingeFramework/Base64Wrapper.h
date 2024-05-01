#pragma once
#ifndef BASE64_WRAPPER_H
#define BASE64_WRAPPER_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif

#endif // HINGE_API

#include <string>

namespace hinge_framework {
    extern "C" HINGE_API const char* encodeBase64(const std::string& plain_text);
    extern "C" HINGE_API const char* decodeBase64(const std::string& base64_text);
}

#endif // BASE64_WRAPPER_H
