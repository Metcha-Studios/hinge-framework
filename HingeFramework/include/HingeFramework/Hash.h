#pragma once
#ifndef HASH_H
#define HASH_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif // HINGE_EXPORTS

#endif // HINGE_API

//#include <string>

namespace hinge_framework {
    extern "C" HINGE_API const char* hashPlaintext(const char* plaintext);
    extern "C" HINGE_API bool compareHash(const char* plaintext, const char* hashValue);
}

#endif // HASH_H
