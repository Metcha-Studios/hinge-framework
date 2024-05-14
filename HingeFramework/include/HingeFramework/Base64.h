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

namespace hinge_framework {
    extern "C" HINGE_API const char* encodeBase64(const char* plain_text);
    extern "C" HINGE_API const char* decodeBase64(const char* base64_text);
}

#endif // BASE64_H
