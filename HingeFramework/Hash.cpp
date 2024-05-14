#include "pch.h"
#include "include/HingeFramework/Hash.h"

#include <cstring>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "include/HingeFramework/Base64.h"

namespace hinge_framework {
    // Fixed length for salt
    const uint16_t SALT_LENGTH = 16; // for example, change this to the desired length

    // Generate a random salt
    void generateRandomSalt(char* salt) {
        RAND_bytes((unsigned char*)salt, SALT_LENGTH);
    }

    Hash sha3_256(const char* str, const char* salt) {
        Hash result;
        EVP_MD_CTX* mdctx;
        const EVP_MD* md;
        unsigned char md_value[EVP_MAX_MD_SIZE];
        uint32_t md_len;

        mdctx = EVP_MD_CTX_new();
        if (mdctx == NULL) {
            // Handle error
            strcpy_s(result.hash, "");
            strcpy_s(result.salt, "");
            return result;
        }

        md = EVP_sha3_256();

        if (1 != EVP_DigestInit_ex(mdctx, md, NULL)) {
            // Handle error
            strcpy_s(result.hash, "");
            strcpy_s(result.salt, "");
            EVP_MD_CTX_free(mdctx);
            return result;
        }

        // Add salt to the input
        if (salt && strlen(salt) > 0) {
            const char* original_salt = hinge_framework::decodeBase64(salt);
            if (1 != EVP_DigestUpdate(mdctx, original_salt, SALT_LENGTH)) { // Use fixed length
                // Handle error
                strcpy_s(result.hash, "");
                strcpy_s(result.salt, "");
                EVP_MD_CTX_free(mdctx);
                return result;
            }
        }
        else {
            // Generate random salt
            char generated_salt[SALT_LENGTH];
            generateRandomSalt(generated_salt);
            if (1 != EVP_DigestUpdate(mdctx, generated_salt, SALT_LENGTH)) { // Use fixed length
                // Handle error
                strcpy_s(result.hash, "");
                strcpy_s(result.salt, "");
                EVP_MD_CTX_free(mdctx);
                return result;
            }
            strcpy_s(result.salt, hinge_framework::encodeBase64(generated_salt));
        }

        // Add the original string
        if (1 != EVP_DigestUpdate(mdctx, str, strlen(str))) {
            // Handle error
            strcpy_s(result.hash, "");
            strcpy_s(result.salt, "");
            EVP_MD_CTX_free(mdctx);
            return result;
        }

        if (1 != EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
            // Handle error
            strcpy_s(result.hash, "");
            strcpy_s(result.salt, "");
            EVP_MD_CTX_free(mdctx);
            return result;
        }

        EVP_MD_CTX_free(mdctx);

        // Convert hash to hex
        for (size_t i = 0; i < md_len; ++i) {
            sprintf_s(&result.hash[i * 2], 3, "%02x", md_value[i]);
        }

        return result;
    }

    bool compareHashes(const char* plaintext, const Hash* hash) {
        Hash computed_hash = sha3_256(plaintext, hash->salt);
        return strcmp(computed_hash.hash, hash->hash) == 0;
    }
}
