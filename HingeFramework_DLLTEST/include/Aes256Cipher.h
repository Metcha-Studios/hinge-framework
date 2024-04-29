#pragma once
#ifndef AES256_CIPHER_H
#define AES256_CIPHER_H

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <cstdint> // 用于 uint16_t
#include <openssl/evp.h> // OpenSSL EVP_PKEY
#include <nlohmann/json.hpp> // 用于 JSON 结构

#include "Cipher.h"

namespace hinge_framework {
    /**
     * @brief AES 加密解密类。
     */
    class HINGE_API Aes256Cipher : public Cipher {
    public:
        /**
         * @brief 构造函数，初始化 OpenSSL 库。
         */
        Aes256Cipher();

        /**
         * @brief 析构函数，清理 OpenSSL 资源。
         */
        ~Aes256Cipher();

        /**
         * @brief 生成指定长度的 AES 密钥。
         *
         * @param key_length AES 密钥长度
         * @return 生成的 AES 密钥
         */
        Key generateKey();

        /**
         * @brief 生成指定长度和 ID 的 AES 密钥。
         *
         * @param key_length AES 密钥长度
         * @param key_id AES 密钥 ID
         * @return 生成的 AES 密钥
         */
        Key generateKey(const char* key_id);

        /**
         * @brief 将密钥写入文件。
         *
         * @param key 密钥
         * @param file_path 密钥文件路径
         * @return 写入是否成功
         */
        bool writeKeyToFile(const Key& key, const char* file_path);

        /**
         * @brief 从文件中读取指定 ID 的密钥。
         *
         * @param key_id 密钥 ID
         * @param file_path 密钥文件路径
         * @return 读取的密钥
         */
        Key readKeyFromFile(const char* key_id, const char* file_path);

        /**
         * @brief 检查密钥是否存在于密钥文件中。
         *
         * @param key_id 密钥 ID
         * @param file_path 密钥文件路径
         * @return 密钥是否存在
         */
        bool isKeyExists(const std::string key_id, const char* file_path) const override;

        /**
         * @brief 使用密钥加密数据。
         *
         * @param plaintext 明文数据
         * @param key 密钥
         * @return 加密后的密文数据
         */
        std::string encrypt(const std::string& key, const std::string& plaintext) const override;

        /**
         * @brief 使用密钥解密数据。
         *
         * @param ciphertext 密文数据
         * @param key 密钥
         * @return 解密后的明文数据
         */
        std::string decrypt(const std::string& key, const std::string& ciphertext) const override;

        /**
         * @brief 加密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param key 密钥
         * @return 加密是否成功
         */
        bool encryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief 解密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param key 密钥
         * @return 解密是否成功
         */
        bool decryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief 加密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param key 密钥
         * @return 加密是否成功
         */
        bool encryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const override;

        /**
         * @brief 解密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param key 密钥
         * @return 解密是否成功
         */
        bool decryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const override;

    private:
        /**
         * @brief 将新的密钥追加到文件中。
         *
         * @param key 密钥
         * @param file_path 密钥文件路径
         * @return 是否成功追加
         */
        bool appendKeyToFile(const Key& key, const char* file_path);
    };
}

#endif // AES256_CIPHER_H
