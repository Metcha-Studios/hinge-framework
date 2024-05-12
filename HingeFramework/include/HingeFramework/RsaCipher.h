#pragma once
#ifndef RSA_CIPHER_H
#define RSA_CIPHER_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif // HINGE_EXPORTS

#endif // HINGE_API

#include <string>
#include <cstdint> // 用于 uint16_t
#include <openssl/evp.h> // OpenSSL EVP_PKEY
#include <nlohmann/json.hpp> // 用于 JSON 结构

#include "Cipher.h"

namespace hinge_framework {
    /**
     * @brief RSA 加密解密类。
     */
    class HINGE_API RsaCipher : public Cipher {
    public:
        /**
         * @brief 构造函数，初始化 OpenSSL 库并设置固定密钥对。
         */
        RsaCipher();

        /**
         * @brief 析构函数，清理 OpenSSL 资源并释放固定密钥对内存。
         */
        ~RsaCipher();

        /**
         * @brief 生成指定长度的 RSA 密钥对。
         *
         * @param key_length RSA 密钥长度
         * @return 生成的 RSA 密钥对
         */
        KeyPair generateKeyPair(const uint16_t key_length);

        /**
         * @brief 生成指定长度和 ID 的 RSA 密钥对。
         *
         * @param key_length RSA 密钥长度
         * @param key_id RSA 密钥 ID
         * @return 生成的 RSA 密钥对
         */
        KeyPair generateKeyPair(const uint16_t key_length, const char* key_id);

        /**
         * @brief 将密钥对写入文件。
         *
         * @param key_pair 密钥对
         * @param file_path 密钥文件路径
         * @return 写入是否成功
         */
        bool writeKeyToFile(const KeyPair& key_pair, const char* file_path);

        /**
         * @brief 从文件中读取指定 ID 的密钥对。
         *
         * @param key_id 密钥对 ID
         * @param file_path 密钥文件路径
         * @return 读取的密钥对
         */
        KeyPair readKeyFromFile(const char* key_id, const char* file_path);

        /**
         * @brief 检查密钥是否存在于密钥文件中。
         *
         * @param key_id 密钥 ID
         * @param file_path 密钥文件路径
         * @return 密钥是否存在
         */
        bool isKeyExists(const std::string key_id, const char* file_path) const override;

        /**
         * @brief 使用公钥加密数据。
         *
         * @param plaintext 明文数据
         * @param public_key 公钥
         * @return 加密后的密文数据
         */
        std::string encrypt(const std::string& public_key, const std::string& plaintext) const override;

        /**
         * @brief 使用私钥解密数据。
         *
         * @param ciphertext 密文数据
         * @param private_key 私钥
         * @return 解密后的明文数据
         */
        std::string decrypt(const std::string& private_key, const std::string& ciphertext) const override;

        /**
         * @brief 加密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param public_key 公钥
         * @return 加密是否成功
         */
        bool encryptFile(const std::string& public_key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief 解密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param private_key 私钥
         * @return 解密是否成功
         */
        bool decryptFile(const std::string& private_key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief 加密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param public_key 公钥
         * @return 加密是否成功
         */
        bool encryptDirectory(const std::string& public_key, const std::string input_directory, const std::string output_directory) const override;

        /**
         * @brief 解密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param private_key 私钥
         * @return 解密是否成功
         */
        bool decryptDirectory(const std::string& private_key, const std::string input_directory, const std::string output_directory) const override;

    private:
        /**
         * @brief 创建 EVP_PKEY 对象。
         *
         * @param pem_key PEM 格式密钥数据
         * @param is_public_key 是否为公钥
         * @return EVP_PKEY 对象
         */
        EVP_PKEY* createPKEYFromPEM(const std::string& pem_key, const bool is_public_key) const;

        /**
         * @brief 将新的密钥对追加到文件中。
         *
         * @param key_pair 密钥对
         * @param file_path 密钥文件路径
         * @return 是否成功追加
         */
        bool appendKeyToFile(const KeyPair& key_pair, const char* file_path);
    };
}

#endif // RSA_CIPHER_H
