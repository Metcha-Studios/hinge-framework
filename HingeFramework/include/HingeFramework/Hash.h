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

#include <openssl/evp.h>

namespace hinge_framework {
    extern "C" HINGE_API typedef struct {
        char hash[EVP_MAX_MD_SIZE * 2 + 1]; // Hex representation of hash
        char salt[EVP_MAX_MD_SIZE * 2 + 1]; // Hex representation of salt
    } Hash;
    extern "C" HINGE_API Hash sha3_256(const char* str, const char* salt = "");
    extern "C" HINGE_API bool compareHashes(const char* plaintext, const Hash hash);
    extern "C" HINGE_API bool storeHash(const Hash hash, const char* db_file_path, const char* db_password);
    extern "C" HINGE_API Hash * retrieveHash(const char* hash_value, const char* db_file_path, const char* db_password);
}

#endif // HASH_H
