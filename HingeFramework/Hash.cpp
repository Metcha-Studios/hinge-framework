#include "pch.h"
#include "include/HingeFramework/Hash.h"

#include <iomanip>
#include <string>
#include <sstream>
#include <bcrypt.h>

namespace hinge_framework {
    const char* hashPlaintext(const char* plaintext) {
        BCRYPT_ALG_HANDLE h_alg = NULL;
        BCRYPT_HASH_HANDLE h_hash = NULL;
        DWORD cb_data = 0, cb_hash = 0;
        PBYTE pb_hash = NULL;
        char* hex_hash = nullptr;

        // Open an algorithm handle
        NTSTATUS status = BCryptOpenAlgorithmProvider(&h_alg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
        if (status == 0) { // Check for success (0 means success)
            // Calculate the size of the buffer to hold the hash
            status = BCryptGetProperty(h_alg, BCRYPT_HASH_LENGTH, (PBYTE)&cb_hash, sizeof(DWORD), &cb_data, 0);
            if (status == 0) {
                pb_hash = (PBYTE)malloc(cb_hash);
                if (pb_hash != NULL) {
                    // Create a hash object
                    status = BCryptCreateHash(h_alg, &h_hash, NULL, 0, NULL, 0, 0);
                    if (status == 0) {
                        // Hash the input buffer
                        status = BCryptHashData(h_hash, (PBYTE)plaintext, (ULONG)strlen(plaintext), 0);
                        if (status == 0) {
                            // Close the hash object
                            status = BCryptFinishHash(h_hash, pb_hash, cb_hash, 0);
                            if (status == 0) {
                                // Convert the hash to hexadecimal string
                                std::ostringstream oss;
                                oss << std::hex << std::setfill('0');
                                for (DWORD i = 0; i < cb_hash; ++i) {
                                    oss << std::setw(2) << static_cast<int>(pb_hash[i]);
                                }
                                std::string temp_hex_hash = oss.str();
                                hex_hash = _strdup(temp_hex_hash.c_str()); // Allocate memory and copy string
                            }
                        }
                        BCryptDestroyHash(h_hash);
                    }
                }
                free(pb_hash);
            }
            BCryptCloseAlgorithmProvider(h_alg, 0);
        }
        return hex_hash;
    }

    bool compareHash(const char* plaintext, const char* hash_value) {
        const char* computed_hash = hashPlaintext(plaintext);
        if (computed_hash == nullptr)
            return false;
        bool match = strcmp(computed_hash, hash_value) == 0;
        free((void*)computed_hash); // Free allocated memory
        return match;
    }
}
