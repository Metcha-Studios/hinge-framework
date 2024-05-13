#include "pch.h"
#include "include/HingeFramework/Hash.h"

#include <string>
#include <openssl/evp.h>

namespace hinge_framework {
    const char* sha256(const char* str) {
        EVP_MD_CTX* mdctx;
        const EVP_MD* md;
        unsigned char md_value[EVP_MAX_MD_SIZE];
        uint32_t md_len;

        // Initialize the EVP_MD_CTX
        mdctx = EVP_MD_CTX_new();
        if (mdctx == NULL) {
            // Initialization failed
            return NULL;
        }

        // Set the hash algorithm to SHA-256
        md = EVP_sha256();

        // Initialize the hash context
        if (1 != EVP_DigestInit_ex(mdctx, md, NULL)) {
            // Initialization failed
            EVP_MD_CTX_free(mdctx);
            return NULL;
        }

        // Update the hash context
        if (1 != EVP_DigestUpdate(mdctx, str, strlen(str))) {
            // Update failed
            EVP_MD_CTX_free(mdctx);
            return NULL;
        }

        // Compute the hash value
        if (1 != EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
            // Computation failed
            EVP_MD_CTX_free(mdctx);
            return NULL;
        }

        // Free the hash context
        EVP_MD_CTX_free(mdctx);

        // Convert the hash value to a hexadecimal string
        static char hex_md_value[EVP_MAX_MD_SIZE * 2 + 1];
        for (uint32_t i = 0; i < md_len; ++i) {
            sprintf_s(&hex_md_value[i * 2], 3, "%02x", md_value[i]);
        }

        return hex_md_value;
    }
}
