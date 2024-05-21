#include "pch.h"
#include "include/HingeFramework/Hash.h"

#include <cstring>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include "include/HingeFramework/Base64.h"

namespace hinge_framework {
    // Fixed length for salt
    const uint16_t SALT_LENGTH = 16; // for example, change this to the desired length

    // Generate a random salt
    void generateRandomSalt(char* salt) {
        unsigned char rand_bytes[SALT_LENGTH];

        RAND_bytes(rand_bytes, SALT_LENGTH);

        for (int i = 0; i < SALT_LENGTH; ++i) {
            while (rand_bytes[i] == '\0') {
                RAND_bytes(&rand_bytes[i], 1);
            }
            salt[i] = rand_bytes[i];
        }

        salt[SALT_LENGTH] = '\0';
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
            strcpy_s(result.salt, salt);
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

    bool compareHashes(const char* plaintext, const Hash hash) {
        Hash computed_hash = sha3_256(plaintext, hash.salt);
        return strcmp(computed_hash.hash, hash.hash) == 0;
    }

    bool storeHash(const Hash hash, const char* db_file_path, const char* db_password) {
        SQLite::Database db(db_file_path, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE | SQLite::OPEN_FULLMUTEX);

        if (db_password != nullptr && db_password[0] != '\0') {
            db.key(db_password);
        }

        db.exec("CREATE TABLE IF NOT EXISTS Hashes (Hash TEXT PRIMARY KEY, Salt TEXT)");

        // Check if the hash already exists
        SQLite::Statement check_query(db, "SELECT COUNT(*) FROM Hashes WHERE Hash = ?");
        check_query.bind(1, hash.hash);

        if (check_query.executeStep()) {
            uint16_t count = check_query.getColumn(0);
            if (count > 0) {
                return true;
            }
        }

        SQLite::Statement query(db, "INSERT INTO Hashes (Hash, Salt) VALUES (?, ?)");
        query.bind(1, hash.hash);
        query.bind(2, hash.salt);

        try {
            query.exec();
        }
        catch (std::exception& e) {
            /*
            * Handle exception (e.g., duplicate key)
            */
            return false;
        }

        return true;
    }

    Hash* retrieveHash(const char* hash_value, const char* db_file_path, const char* db_password) {
        SQLite::Database db(db_file_path, SQLite::OPEN_READWRITE | SQLite::OPEN_FULLMUTEX);

        if (db_password != nullptr && db_password[0] != '\0') {
            db.key(db_password);
        }

        SQLite::Statement query(db, "SELECT Salt FROM Hashes WHERE Hash = ?");
        query.bind(1, hash_value);

        try {
            if (query.executeStep()) {
                // If row exists, retrieve salt
                std::string salt = query.getColumn(0);
                Hash* result = new Hash;
                strcpy_s(result->hash, hash_value);
                strcpy_s(result->salt, salt.c_str());
                return result;
            }
            else {
                // If row does not exist, return nullptr
                return nullptr;
            }
        }
        catch (std::exception& e) {
            /*
            * Handle exception
            */
            return nullptr;
        }
    }

    bool deleteHash(const char* hash_value, const char* db_file_path, const char* db_password) {
        SQLite::Database db(db_file_path, SQLite::OPEN_READWRITE | SQLite::OPEN_FULLMUTEX);

        if (db_password != nullptr && db_password[0] != '\0') {
            db.key(db_password);
        }

        SQLite::Statement query(db, "DELETE FROM Hashes WHERE Hash = ?");
        query.bind(1, hash_value);

        try {
            query.exec();
        }
        catch (std::exception& e) {
            /*
            * Handle exception (e.g., hash not found)
            */
            return false;
        }

        // Check if any row was affected (hash existed and got deleted)
        bool changes = db.execAndGet("SELECT changes();").getInt();
        if (changes > 0) {
            return true;
        }
        else {
            return false;
        }
    }
}
