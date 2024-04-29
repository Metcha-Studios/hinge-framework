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
#include <cstdint> // ���� uint16_t
#include <openssl/evp.h> // OpenSSL EVP_PKEY
#include <nlohmann/json.hpp> // ���� JSON �ṹ

#include "Cipher.h"

namespace hinge_framework {
    /**
     * @brief AES ���ܽ����ࡣ
     */
    class HINGE_API Aes256Cipher : public Cipher {
    public:
        /**
         * @brief ���캯������ʼ�� OpenSSL �⡣
         */
        Aes256Cipher();

        /**
         * @brief �������������� OpenSSL ��Դ��
         */
        ~Aes256Cipher();

        /**
         * @brief ����ָ�����ȵ� AES ��Կ��
         *
         * @param key_length AES ��Կ����
         * @return ���ɵ� AES ��Կ
         */
        Key generateKey();

        /**
         * @brief ����ָ�����Ⱥ� ID �� AES ��Կ��
         *
         * @param key_length AES ��Կ����
         * @param key_id AES ��Կ ID
         * @return ���ɵ� AES ��Կ
         */
        Key generateKey(const char* key_id);

        /**
         * @brief ����Կд���ļ���
         *
         * @param key ��Կ
         * @param file_path ��Կ�ļ�·��
         * @return д���Ƿ�ɹ�
         */
        bool writeKeyToFile(const Key& key, const char* file_path);

        /**
         * @brief ���ļ��ж�ȡָ�� ID ����Կ��
         *
         * @param key_id ��Կ ID
         * @param file_path ��Կ�ļ�·��
         * @return ��ȡ����Կ
         */
        Key readKeyFromFile(const char* key_id, const char* file_path);

        /**
         * @brief �����Կ�Ƿ��������Կ�ļ��С�
         *
         * @param key_id ��Կ ID
         * @param file_path ��Կ�ļ�·��
         * @return ��Կ�Ƿ����
         */
        bool isKeyExists(const std::string key_id, const char* file_path) const override;

        /**
         * @brief ʹ����Կ�������ݡ�
         *
         * @param plaintext ��������
         * @param key ��Կ
         * @return ���ܺ����������
         */
        std::string encrypt(const std::string& key, const std::string& plaintext) const override;

        /**
         * @brief ʹ����Կ�������ݡ�
         *
         * @param ciphertext ��������
         * @param key ��Կ
         * @return ���ܺ����������
         */
        std::string decrypt(const std::string& key, const std::string& ciphertext) const override;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool encryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool decryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool encryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const override;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool decryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const override;

    private:
        /**
         * @brief ���µ���Կ׷�ӵ��ļ��С�
         *
         * @param key ��Կ
         * @param file_path ��Կ�ļ�·��
         * @return �Ƿ�ɹ�׷��
         */
        bool appendKeyToFile(const Key& key, const char* file_path);
    };
}

#endif // AES256_CIPHER_H
