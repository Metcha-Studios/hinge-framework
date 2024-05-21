#pragma once
#ifndef CIPHER_H
#define CIPHER_H

#include <string>
#include <nlohmann/json.hpp> // ���� JSON �ṹ

namespace hinge_framework {
    /**
     * @brief ����Կ�ṹ�壬������ԿID����Կ��
     */
    struct Key {
        std::string id_; /**< ��Կ��Ψһ��ʶ�� */
        std::string key_; /**< ��Կ */
    };

    /**
     * @brief ��Կ�Խṹ�壬������ԿID����Կ��˽Կ��
     */
    struct KeyPair {
        std::string id_; /**< ��Կ�Ե�Ψһ��ʶ�� */
        std::string public_key_; /**< ��Կ */
        std::string private_key_; /**< ˽Կ */
    };

    /**
     * @brief ���ܽ��ܳ����ࡣ
     */
    class Cipher {
    public:
        /**
         * @brief ���캯������ʼ�� OpenSSL �Ⲣ���ù̶���Կ�ԡ�
         */
        Cipher();

        /**
         * @brief �������������� OpenSSL ��Դ���ͷŹ̶���Կ���ڴ档
         */
        ~Cipher();

        /**
         * @brief �������� ENT �ļ���������Կ�ԡ�
         *
         * @param ent_key_pair ���� ENT �ļ���������Կ��
         * @return Void
         */
        void setEntKeyPair(const KeyPair& ent_key_pair);

        /**
         * @brief ��ȡ���� ENT �ļ���������Կ�ԡ�
         *
         * @return ���� ENT �ļ���������Կ��
         */
        KeyPair getEntKeyPair() const;

        /**
         * @brief �����Կ�Ƿ��������Կ�ļ��С�
         *
         * @param key_id ��Կ ID
         * @param file_path ��Կ�ļ�·��
         * @return ��Կ�Ƿ����
         */
        virtual bool isKeyExists(const std::string key_id, const char* file_path) const = 0;

        /**
         * @brief ʹ����Կ�������ݡ�
         *
         * @param plaintext ��������
         * @param key ��Կ�ַ���
         * @return ���ܺ����������
         */
        virtual std::string encrypt(const std::string& key, const std::string& plaintext) const = 0;

        /**
         * @brief ʹ����Կ�������ݡ�
         *
         * @param ciphertext ��������
         * @param key ��Կ�ַ���
         * @return ���ܺ����������
         */
        virtual std::string decrypt(const std::string& key, const std::string& ciphertext) const = 0;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param key ��Կ�ַ���
         * @return �����Ƿ�ɹ�
         */
        virtual bool encryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const = 0;

        /**
         * @brief �����ļ���
         *
         * @param input_file_path �����ļ�·��
         * @param output_file_path ����ļ�·��
         * @param key ��Կ�ַ���
         * @return �����Ƿ�ɹ�
         */
        virtual bool decryptFile(const std::string& key, const char* input_file_path, const char* output_file_path) const = 0;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param key ��Կ�ַ���
         * @return �����Ƿ�ɹ�
         */
        virtual bool encryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const = 0;

        /**
         * @brief ����Ŀ¼�е������ļ���
         *
         * @param input_directory ����Ŀ¼·��
         * @param output_directory ���Ŀ¼·��
         * @param key ��Կ�ַ���
         * @return �����Ƿ�ɹ�
         */
        virtual bool decryptDirectory(const std::string& key, const std::string input_directory, const std::string output_directory) const = 0;

    protected:
        KeyPair ent_key_pair_; /**< ˽�г�Ա���ԣ����� ENT ��Կ�ļ���������Կ�� */

        /**
         * @brief ������ʼ��Կ�������ļ���
         *
         * @param ent_file_path ��������Կ�ļ�·��
         * @param version �汾�ţ�Ĭ��Ϊ "2"
         * @return ������ JSON �ṹ
         */
        nlohmann::ordered_json createBinaryEntFile(const char* ent_file_path, const char* version = "2") const;
    };
}

#endif // CIPHER_H
