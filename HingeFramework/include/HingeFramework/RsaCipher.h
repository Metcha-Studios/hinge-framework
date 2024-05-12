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
#include <cstdint> // ���� uint16_t
#include <openssl/evp.h> // OpenSSL EVP_PKEY
#include <nlohmann/json.hpp> // ���� JSON �ṹ

#include "Cipher.h"

namespace hinge_framework {
    /**
     * @brief RSA ���ܽ����ࡣ
     */
    class HINGE_API RsaCipher : public Cipher {
    public:
        /**
         * @brief ���캯������ʼ�� OpenSSL �Ⲣ���ù̶���Կ�ԡ�
         */
        RsaCipher();

        /**
         * @brief �������������� OpenSSL ��Դ���ͷŹ̶���Կ���ڴ档
         */
        ~RsaCipher();

        /**
         * @brief ����ָ�����ȵ� RSA ��Կ�ԡ�
         *
         * @param key_length RSA ��Կ����
         * @return ���ɵ� RSA ��Կ��
         */
        KeyPair generateKeyPair(const uint16_t key_length);

        /**
         * @brief ����ָ�����Ⱥ� ID �� RSA ��Կ�ԡ�
         *
         * @param key_length RSA ��Կ����
         * @param key_id RSA ��Կ ID
         * @return ���ɵ� RSA ��Կ��
         */
        KeyPair generateKeyPair(const uint16_t key_length, const char* key_id);

        /**
         * @brief ����Կ��д���ļ���
         *
         * @param key_pair ��Կ��
         * @param file_path ��Կ�ļ�·��
         * @return д���Ƿ�ɹ�
         */
        bool writeKeyToFile(const KeyPair& key_pair, const char* file_path);

        /**
         * @brief ���ļ��ж�ȡָ�� ID ����Կ�ԡ�
         *
         * @param key_id ��Կ�� ID
         * @param file_path ��Կ�ļ�·��
         * @return ��ȡ����Կ��
         */
        KeyPair readKeyFromFile(const char* key_id, const char* file_path);

        /**
         * @brief �����Կ�Ƿ��������Կ�ļ��С�
         *
         * @param key_id ��Կ ID
         * @param file_path ��Կ�ļ�·��
         * @return ��Կ�Ƿ����
         */
        bool isKeyExists(const std::string key_id, const char* file_path) const override;

        /**
         * @brief ʹ�ù�Կ�������ݡ�
         *
         * @param plaintext ��������
         * @param public_key ��Կ
         * @return ���ܺ����������
         */
        std::string encrypt(const std::string& public_key, const std::string& plaintext) const override;

        /**
         * @brief ʹ��˽Կ�������ݡ�
         *
         * @param ciphertext ��������
         * @param private_key ˽Կ
         * @return ���ܺ����������
         */
        std::string decrypt(const std::string& private_key, const std::string& ciphertext) const override;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param public_key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool encryptFile(const std::string& public_key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param private_key ˽Կ
         * @return �����Ƿ�ɹ�
         */
        bool decryptFile(const std::string& private_key, const char* input_file_path, const char* output_file_path) const override;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param public_key ��Կ
         * @return �����Ƿ�ɹ�
         */
        bool encryptDirectory(const std::string& public_key, const std::string input_directory, const std::string output_directory) const override;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param private_key ˽Կ
         * @return �����Ƿ�ɹ�
         */
        bool decryptDirectory(const std::string& private_key, const std::string input_directory, const std::string output_directory) const override;

    private:
        /**
         * @brief ���� EVP_PKEY ����
         *
         * @param pem_key PEM ��ʽ��Կ����
         * @param is_public_key �Ƿ�Ϊ��Կ
         * @return EVP_PKEY ����
         */
        EVP_PKEY* createPKEYFromPEM(const std::string& pem_key, const bool is_public_key) const;

        /**
         * @brief ���µ���Կ��׷�ӵ��ļ��С�
         *
         * @param key_pair ��Կ��
         * @param file_path ��Կ�ļ�·��
         * @return �Ƿ�ɹ�׷��
         */
        bool appendKeyToFile(const KeyPair& key_pair, const char* file_path);
    };
}

#endif // RSA_CIPHER_H
