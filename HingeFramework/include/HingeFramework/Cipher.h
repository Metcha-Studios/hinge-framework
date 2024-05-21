#pragma once
#ifndef CIPHER_H
#define CIPHER_H

#include <string>
#include <nlohmann/json.hpp> // 用于 JSON 结构

namespace hinge_framework {
    /**
     * @brief 单密钥结构体，包含密钥ID和密钥。
     */
    struct Key {
        std::string id_; /**< 密钥的唯一标识符 */
        std::string key_; /**< 密钥 */
    };

    /**
     * @brief 密钥对结构体，包含密钥ID、公钥和私钥。
     */
    struct KeyPair {
        std::string id_; /**< 密钥对的唯一标识符 */
        std::string public_key_; /**< 公钥 */
        std::string private_key_; /**< 私钥 */
    };

    /**
     * @brief 加密解密抽象类。
     */
    class Cipher {
    public:
        /**
         * @brief 构造函数，初始化 OpenSSL 库并设置固定密钥对。
         */
        Cipher();

        /**
         * @brief 析构函数，清理 OpenSSL 资源并释放固定密钥对内存。
         */
        ~Cipher();

        /**
         * @brief 设置用于 ENT 文件操作的密钥对。
         *
         * @param ent_key_pair 用于 ENT 文件操作的密钥对
         * @return Void
         */
        void setEntKeyPair(const KeyPair& ent_key_pair);

        /**
         * @brief 获取用于 ENT 文件操作的密钥对。
         *
         * @return 用于 ENT 文件操作的密钥对
         */
        KeyPair getEntKeyPair() const;

        /**
         * @brief 检查密钥是否存在于密钥文件中。
         *
         * @param key_id 密钥 ID
         * @param file_path 密钥文件路径
         * @return 密钥是否存在
         */
        virtual bool isKeyExists(const std::string key_id, const char* file_path) const = 0;

        /**
         * @brief 使用密钥加密数据。
         *
         * @param plaintext 明文数据
         * @param key 密钥字符串
         * @return 加密后的密文数据
         */
        virtual std::string encrypt(const std::string& key, const std::string& plaintext) const = 0;

        /**
         * @brief 使用密钥解密数据。
         *
         * @param ciphertext 密文数据
         * @param key 密钥字符串
         * @return 解密后的明文数据
         */
        virtual std::string decrypt(const std::string& key, const std::string& ciphertext) const = 0;

        /**
         * @brief 加密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param key 密钥字符串
         * @return 加密是否成功
         */
        virtual bool encryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const = 0;

        /**
         * @brief 解密文件。
         *
         * @param input_file_path 输入文件路径
         * @param output_file_path 输出文件路径
         * @param key 密钥字符串
         * @return 解密是否成功
         */
        virtual bool decryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const = 0;

        /**
         * @brief 加密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param key 密钥字符串
         * @return 加密是否成功
         */
        virtual bool encryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const = 0;

        /**
         * @brief 解密目录中的所有文件。
         *
         * @param input_directory 输入目录路径
         * @param output_directory 输出目录路径
         * @param key 密钥字符串
         * @return 解密是否成功
         */
        virtual bool decryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const = 0;

    protected:
        KeyPair ent_key_pair_; /**< 私有成员属性，用于 ENT 密钥文件操作的密钥对 */

        /**
         * @brief 创建初始密钥二进制文件。
         *
         * @param ent_file_path 二进制密钥文件路径
         * @param version 版本号，默认为 "2"
         * @return 创建的 JSON 结构
         */
        nlohmann::ordered_json createBinaryEntFile(const char* ent_file_path, const char* version = "2") const;
    };
}

#endif // CIPHER_H
