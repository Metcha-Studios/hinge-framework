#include "pch.h"
#include "include/HingeFramework/Aes256Cipher.h"

#include <random>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <nlohmann/json.hpp>

#include "include/HingeFramework/Cipher.h"
#include "include/HingeFramework/RsaCipher.h"
#include "include/HingeFramework/Uuid.h"

namespace hinge_framework {
    /*
    * public:
    */

    Aes256Cipher::Aes256Cipher() {
        OpenSSL_add_all_algorithms();
        ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Error creating EVP_CIPHER_CTX");
        }
    }

    Aes256Cipher::~Aes256Cipher() {
        EVP_CIPHER_CTX_free(ctx);
        EVP_cleanup();
    }

    Key Aes256Cipher::generateKey() {
        Key key;

        const char* const CHARSET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const int KEY_LENGTH = 32; // 256 bits
        char* const generated_key = new char[KEY_LENGTH + 1]; // += "\0"

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::strlen(CHARSET) - 2); // -2是因为索引从0开始，而字符串以 '\0' 结尾，即 0 - 31 范围

        // 随机调取32个字符
        for (int i = 0; i < KEY_LENGTH; ++i) {
            int index = dis(gen);
            generated_key[i] = CHARSET[index];
        }
        generated_key[KEY_LENGTH] = '\0';

        key.id_ = hinge_framework::uuidV4Generator();
        key.key_ = std::string(generated_key);

        delete[] generated_key;

        return key;
    }

    Key Aes256Cipher::generateKey(const char* key_id) {
        Key key;

        const char* const CHARSET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const int const KEY_LENGTH = 32; // 256 bits
        char* const generated_key = new char[KEY_LENGTH + 1]; // += "\0"

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::strlen(CHARSET) - 2); // -2是因为索引从0开始，而字符串以 '\0' 结尾，即 0 - 31 范围

        // 随机调取32个字符
        for (int i = 0; i < KEY_LENGTH; ++i) {
            int index = dis(gen);
            generated_key[i] = CHARSET[index];
        }
        generated_key[KEY_LENGTH] = '\0';

        key.id_ = key_id;
        key.key_ = std::string(generated_key);

        delete[] generated_key;

        return key;
    }

    bool Aes256Cipher::writeKeyToFile(const Key& key, const char* file_path) {
        bool is_succeed;
        // 检查文件是否存在
        std::ifstream file_exists(file_path);
        if (file_exists.good()) {
            file_exists.close();
            // 文件已存在，以附加模式打开
            is_succeed = appendKeyToFile(key, file_path);
        }
        else {
            file_exists.close();
            // 文件不存在，创建新文件
            createBinaryEntFile(file_path);
            // 向文件添加密钥对
            is_succeed = appendKeyToFile(key, file_path);
        }

        return is_succeed;
    }

    Key Aes256Cipher::readKeyFromFile(const char* key_id, const char* file_path) {
        const RsaCipher rsa;
        Key key;

        // 读取密钥文件
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        const std::string encryptedKeyData((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // 解密密钥文件
        const nlohmann::ordered_json keys_json = nlohmann::ordered_json::parse(rsa.decrypt(getEntKeyPair().private_key_, encryptedKeyData));

        // 检查指定的密钥 ID 是否存在
        if (!isKeyExists(key_id, file_path)) {
            throw std::runtime_error("Key with specified ID not found in the file.");
        }

        // 提取指定的密钥对
        const nlohmann::ordered_json key_data = keys_json["aes256_keys"];
        key.id_ = key_id;
        key.key_ = key_data[key_id].get<std::string>();

        return key;
    }

    bool Aes256Cipher::isKeyExists(const std::string key_id, const char* file_path) const {
        const RsaCipher rsa;

        // 读取密钥文件
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            return false;
            throw std::runtime_error("Failed to open file for reading.");
        }

        const std::string encrypted_key_data((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // 解密密钥文件
        const nlohmann::ordered_json keys_json = nlohmann::ordered_json::parse(rsa.decrypt(getEntKeyPair().private_key_, encrypted_key_data));

        // 检查密钥是否存在
        return keys_json["aes256_keys"].find(key_id) != keys_json["aes256_keys"].end();
    }

    std::string Aes256Cipher::encrypt(const std::string& key, const std::string& plaintext) const {
        // Generate a random IV (Initialization Vector)
        unsigned char iv[EVP_MAX_IV_LENGTH];
        if (RAND_bytes(iv, EVP_MAX_IV_LENGTH) != 1) {
            throw std::runtime_error("Error generating IV");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv);

        // Encrypt the plaintext
        std::string ciphertext;
        int32_t len;
        uint64_t ciphertext_len;

        ciphertext.resize(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
        EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size());
        ciphertext_len = len;

        EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[ciphertext_len]), &len);
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        // Combine IV and ciphertext
        std::string result(reinterpret_cast<char*>(iv), EVP_MAX_IV_LENGTH);
        result += ciphertext.substr(0, ciphertext_len);

        return result;
    }

    std::string Aes256Cipher::decrypt(const std::string& key, const std::string& ciphertext) const {
        // Extract IV from the ciphertext
        unsigned char iv[EVP_MAX_IV_LENGTH];
        std::memcpy(iv, ciphertext.c_str(), EVP_MAX_IV_LENGTH);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv);

        // Decrypt the ciphertext
        std::string plaintext;
        int32_t len;
        uint64_t plaintext_len;

        plaintext.resize(ciphertext.size() - EVP_MAX_IV_LENGTH);
        EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &len, reinterpret_cast<const unsigned char*>(ciphertext.c_str() + EVP_MAX_IV_LENGTH), ciphertext.size() - EVP_MAX_IV_LENGTH);
        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plaintext[plaintext_len]), &len);
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        return plaintext.substr(0, plaintext_len);
    }

    bool Aes256Cipher::encryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const {
        // Make sure the output file path exists
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }

        std::ifstream in_file(input_file_path, std::ios::binary);
        if (!in_file.is_open()) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        std::ofstream out_file(output_file_path, std::ios::binary);
        if (!out_file.is_open()) {
            in_file.close();
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

        unsigned char* in_buffer = static_cast<unsigned char*>(malloc(4096));
        unsigned char* out_buffer = static_cast<unsigned char*>(malloc(4096 + AES_BLOCK_SIZE));
        if (in_buffer == nullptr || out_buffer == nullptr) {
            throw std::exception("Failed to allocate memory for buffer");
            in_file.close();
            out_file.close();
            EVP_CIPHER_CTX_free(ctx);
            free(in_buffer);
            free(out_buffer);
            return false;
        }

        int32_t bytes_read, bytes_written;

        while ((bytes_read = in_file.read(reinterpret_cast<char*>(in_buffer), 4096).gcount()) > 0) {
            if (EVP_EncryptUpdate(ctx, out_buffer, &bytes_written, in_buffer, bytes_read) != 1) {
                in_file.close();
                out_file.close();
                EVP_CIPHER_CTX_free(ctx);
                free(in_buffer);
                free(out_buffer);
                return false;
            }
            out_file.write(reinterpret_cast<const char*>(out_buffer), bytes_written);
        }

        if (EVP_EncryptFinal_ex(ctx, out_buffer, &bytes_written) != 1) {
            in_file.close();
            out_file.close();
            EVP_CIPHER_CTX_free(ctx);
            free(in_buffer);
            free(out_buffer);
            return false;
        }
        out_file.write(reinterpret_cast<const char*>(out_buffer), bytes_written);

        free(in_buffer);
        free(out_buffer);
        in_file.close();
        out_file.close();
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }

    bool Aes256Cipher::decryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const {
        // Make sure the output file path exists
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }

        std::ifstream in_file(input_file_path, std::ios::binary);
        if (!in_file.is_open()) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        std::ofstream out_file(output_file_path, std::ios::binary);
        if (!out_file.is_open()) {
            in_file.close();
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

        unsigned char* in_buffer = static_cast<unsigned char*>(malloc(4096 + AES_BLOCK_SIZE));
        unsigned char* out_buffer = static_cast<unsigned char*>(malloc(4096 + AES_BLOCK_SIZE));
        if (in_buffer == nullptr || out_buffer == nullptr) {
            throw std::exception("Failed to allocate memory for buffer");
            in_file.close();
            out_file.close();
            EVP_CIPHER_CTX_free(ctx);
            free(in_buffer);
            free(out_buffer);
            return false;
        }

        int32_t bytes_read, bytes_written;

        while ((bytes_read = in_file.read(reinterpret_cast<char*>(in_buffer), 4096 + AES_BLOCK_SIZE).gcount()) > 0) {
            if (EVP_DecryptUpdate(ctx, out_buffer, &bytes_written, in_buffer, bytes_read) != 1) {
                in_file.close();
                out_file.close();
                EVP_CIPHER_CTX_free(ctx);
                free(in_buffer);
                free(out_buffer);
                return false;
            }
            out_file.write(reinterpret_cast<const char*>(out_buffer), bytes_written);
        }

        if (EVP_DecryptFinal_ex(ctx, out_buffer, &bytes_written) != 1) {
            in_file.close();
            out_file.close();
            EVP_CIPHER_CTX_free(ctx);
            free(in_buffer);
            free(out_buffer);
            return false;
        }
        out_file.write(reinterpret_cast<const char*>(out_buffer), bytes_written);

        free(in_buffer);
        free(out_buffer);
        in_file.close();
        out_file.close();
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }

    bool Aes256Cipher::encryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(input_directory)) {
                if (entry.is_regular_file()) {
                    const std::string input_file = entry.path().string();
                    const std::string output_file = output_directory + entry.path().string().substr(input_directory.size());
                    if (!encryptFile(key, input_file.c_str(), output_file.c_str()))
                        return false;
                }
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error during encryption: " << e.what() << std::endl;
            return false;
        }
    }

    bool Aes256Cipher::decryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(input_directory)) {
                if (entry.is_regular_file()) {
                    const std::string input_file = entry.path().string();
                    const std::string output_file = output_directory + entry.path().string().substr(input_directory.size());
                    if (!decryptFile(key, input_file.c_str(), output_file.c_str()))
                        return false;
                }
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error during decryption: " << e.what() << std::endl;
            return false;
        }
    }

    /*
    * private:
    */

    bool Aes256Cipher::appendKeyToFile(const Key& key, const char* file_path) const {
        nlohmann::ordered_json keys_json;
        const RsaCipher rsa;

        // 检查密钥是否已存在
        if (isKeyExists(key.id_, file_path)) {
            std::cerr << "Key with ID '" << key.id_ << "' already exists in the file '" << file_path << "'.\n";
            return false;
        }

        // 读取密钥文件
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            throw std::runtime_error("Failed to open file for appending.");
        }

        const std::string encrypted_key_data((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // 解密密钥文件
        keys_json = nlohmann::ordered_json::parse(rsa.decrypt(getEntKeyPair().private_key_, encrypted_key_data));

        // 添加新的密钥对
        keys_json["aes256_keys"][key.id_] = key.key_;

        // 序列化 JSON 结构
        const std::string serialized_keys = keys_json.dump(4);

        //std::cout << serializedKeys << std::endl;

        // 使用 ENT 密钥文件所用密钥加密 JSON 数据
        const std::string encrypted_keys = rsa.encrypt(getEntKeyPair().public_key_, serialized_keys);

        // 将加密后的数据写入文件
        std::ofstream out_file(file_path, std::ios::out | std::ios::binary);
        if (!out_file.is_open()) {
            throw std::runtime_error("Failed to open file for writing.");
        }
        out_file.write(encrypted_keys.c_str(), encrypted_keys.size());
        out_file.close();

        return true;
    }
}
