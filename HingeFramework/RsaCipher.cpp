#include "RsaCipher.h"
#include "pch.h"

#include <math.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <nlohmann/json.hpp>

#include "Cipher.h"
#include "uuidV4.h"

namespace hinge_framework {
    /*
    * public:
    */

    RsaCipher::RsaCipher() {
        // ��ʼ��OpenSSL��
        OpenSSL_add_all_algorithms();
    }

    RsaCipher::~RsaCipher() {
        // ����OpenSSL��Դ
        EVP_cleanup();
    }

    Cipher::KeyPair RsaCipher::generateKeyPair(const uint16_t key_length) {
        KeyPair key_pair;

        key_pair.id_ = uuid::uuidV4Generator();

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (!ctx) {
            return key_pair;
        }

        if (!EVP_PKEY_keygen_init(ctx) || !EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_length)) {
            EVP_PKEY_CTX_free(ctx);
            return key_pair;
        }

        EVP_PKEY* pkey = nullptr;
        if (!EVP_PKEY_keygen(ctx, &pkey)) {
            EVP_PKEY_CTX_free(ctx);
            return key_pair;
        }

        // ��ȡ��Կ
        BIO* bio_pub = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bio_pub, pkey);
        char* pub_key_data;
        long pubKey_len = BIO_get_mem_data(bio_pub, &pub_key_data);
        key_pair.public_key_.assign(pub_key_data, pubKey_len);
        BIO_free(bio_pub);

        // ��ȡ˽Կ
        BIO* bio_priv = BIO_new(BIO_s_mem());
        PEM_write_bio_PKCS8PrivateKey(bio_priv, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        char* priv_key_data;
        long priv_key_len = BIO_get_mem_data(bio_priv, &priv_key_data);
        key_pair.private_key_.assign(priv_key_data, priv_key_len);
        BIO_free(bio_priv);

        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return key_pair;
    }

    Cipher::KeyPair RsaCipher::generateKeyPair(const uint16_t key_length, const char* key_id) {
        KeyPair key_pair;

        key_pair.id_ = key_id;

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (!ctx) {
            return key_pair;
        }

        if (!EVP_PKEY_keygen_init(ctx) || !EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_length)) {
            EVP_PKEY_CTX_free(ctx);
            return key_pair;
        }

        EVP_PKEY* pkey = nullptr;
        if (!EVP_PKEY_keygen(ctx, &pkey)) {
            EVP_PKEY_CTX_free(ctx);
            return key_pair;
        }

        // ��ȡ��Կ
        BIO* bio_pub = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bio_pub, pkey);
        char* pub_key_data;
        long pub_key_len = BIO_get_mem_data(bio_pub, &pub_key_data);
        key_pair.public_key_.assign(pub_key_data, pub_key_len);
        BIO_free(bio_pub);

        // ��ȡ˽Կ
        BIO* bio_priv = BIO_new(BIO_s_mem());
        PEM_write_bio_PKCS8PrivateKey(bio_priv, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        char* priv_key_data;
        long priv_key_len = BIO_get_mem_data(bio_priv, &priv_key_data);
        key_pair.private_key_.assign(priv_key_data, priv_key_len);
        BIO_free(bio_priv);

        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return key_pair;
    }

    bool RsaCipher::writeKeyToFile(const KeyPair& key_pair, const char* file_path) {
        bool is_succeed;
        // ����ļ��Ƿ����
        std::ifstream file_exists(file_path);
        if (file_exists.good()) {
            file_exists.close();
            // �ļ��Ѵ��ڣ��Ը���ģʽ��
            is_succeed = appendKeyToFile(key_pair, file_path);
        }
        else {
            file_exists.close();
            // �ļ������ڣ��������ļ�
            createBinaryEntFile(file_path);
            // ���ļ������Կ��
            is_succeed = appendKeyToFile(key_pair, file_path);
        }

        return is_succeed;
    }

    Cipher::KeyPair RsaCipher::readKeyFromFile(const char* key_id, const char* file_path) {
        KeyPair key_pair;

        // ��ȡ��Կ�ļ�
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        const std::string encryptedKeyData((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // ������Կ�ļ�
        const nlohmann::ordered_json keys_json = nlohmann::ordered_json::parse(decrypt(getEntKeyPair()->private_key_, encryptedKeyData));

        // ���ָ������Կ ID �Ƿ����
        if (!isKeyExists(key_id, file_path)) {
            throw std::runtime_error("Key with specified ID not found in the file.");
        }

        // ��ȡָ������Կ��
        const nlohmann::ordered_json key_data = keys_json["rsa_keys"][key_id];
        key_pair.id_ = key_id;
        key_pair.public_key_ = key_data["public_key"].get<std::string>();
        key_pair.private_key_ = key_data["private_key"].get<std::string>();

        return key_pair;
    }

    bool RsaCipher::isKeyExists(const std::string key_id, const char* file_path) const {
        // ��ȡ��Կ�ļ�
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            return false;
            throw std::runtime_error("Failed to open file for reading.");
        }

        const std::string encrypted_key_data((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // ������Կ�ļ�
        const nlohmann::ordered_json keys_json = nlohmann::ordered_json::parse(decrypt(getEntKeyPair()->private_key_, encrypted_key_data));

        // �����Կ�Ƿ����
        return keys_json["rsa_keys"].find(key_id) != keys_json["rsa_keys"].end();
    }

    std::string RsaCipher::encrypt(const std::string& public_key, const std::string& plaintext) const {
        EVP_PKEY* pkey = createPKEYFromPEM(public_key, true);
        if (!pkey) {
            return "";
        }

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        if (!ctx) {
            EVP_PKEY_free(pkey);
            return "";
        }

        if (EVP_PKEY_encrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return "";
        }

        const size_t key_size = EVP_PKEY_size(pkey);
        const size_t block_size = key_size - 42; // RSA_PKCS1_PADDING adds overhead

        std::string encrypted_text;

        for (size_t i = 0; i < plaintext.size(); i += block_size) {
            size_t len = min(block_size, plaintext.size() - i);

            const std::string block = plaintext.substr(i, len);
            std::string encrypted_block;

            encrypted_block.resize(key_size);

            size_t encrypted_len = key_size; // ����һ���������洢���ܺ����ݵĳ���
            if (EVP_PKEY_encrypt(ctx, reinterpret_cast<unsigned char*>(&encrypted_block[0]), &encrypted_len, reinterpret_cast<const unsigned char*>(block.c_str()), block.size()) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return "";
            }

            encrypted_block.resize(encrypted_len); // �������ܺ����ݵĴ�С
            encrypted_text += encrypted_block;
        }

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return encrypted_text;
    }

    std::string RsaCipher::decrypt(const std::string& private_key, const std::string& ciphertext) const {
        EVP_PKEY* pkey = createPKEYFromPEM(private_key, false);
        if (!pkey) {
            return "";
        }

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        if (!ctx) {
            EVP_PKEY_free(pkey);
            return "";
        }

        if (EVP_PKEY_decrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return "";
        }

        const size_t key_size = EVP_PKEY_size(pkey);
        const size_t block_size = key_size;

        std::string decrypted_text;

        for (size_t i = 0; i < ciphertext.size(); i += block_size) {
            size_t len = min(block_size, ciphertext.size() - i);

            const std::string block = ciphertext.substr(i, len);
            std::string decrypted_block;

            decrypted_block.resize(key_size);

            size_t decrypted_len = key_size; // ����һ���������洢���ܺ����ݵĳ���
            if (EVP_PKEY_decrypt(ctx, reinterpret_cast<unsigned char*>(&decrypted_block[0]), &decrypted_len, reinterpret_cast<const unsigned char*>(block.c_str()), block.size()) <= 0) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(pkey);
                return "";
            }

            decrypted_block.resize(decrypted_len); // �������ܺ����ݵĴ�С
            decrypted_text += decrypted_block;
        }

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return decrypted_text;
    }

    bool RsaCipher::encryptFile(const std::string& public_key, const char* input_file_path, const char* output_file_path) const {
        // �Զ�����ģʽ�������ļ�
        std::ifstream input_file(input_file_path, std::ios::binary);
        if (!input_file.is_open()) {
            std::cerr << "Failed to open input file: " << input_file_path << std::endl;
            return false;
        }

        // ȷ������ļ�·������
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());

        // �Զ�����ģʽ��������ļ�
        std::ofstream output_file(output_file_path, std::ios::binary);
        if (!output_file.is_open()) {
            std::cerr << "Failed to create output file: " << output_file_path << std::endl;
            input_file.close();
            return false;
        }

        // �������ȡ�ļ����ݲ�����
        char* buffer = static_cast<char*>(malloc(sizeof(char) * 4096));
        if (buffer == nullptr) {
            throw std::exception("Failed to allocate memory for buffer");
        }

        while (input_file.read(buffer, sizeof(char) * 4096)) {
            const std::string encrypted_block = encrypt(public_key, std::string(buffer, input_file.gcount()));
            output_file.write(encrypted_block.data(), encrypted_block.size());
        }

        // �������һ���ļ�����
        if (input_file.gcount() > 0) {
            const std::string last_block = encrypt(public_key, std::string(buffer, input_file.gcount()));
            output_file.write(last_block.data(), last_block.size());
        }
        free(buffer);

        // �ر��ļ�
        input_file.close();
        output_file.close();

        //std::cout << "File: " << inputFilePath << "encrypted successfully, the path of output: " << outputFilePath << std::endl;
        return true;
    }

    bool RsaCipher::decryptFile(const std::string& private_key, const char* input_file_path, const char* output_file_path) const {
        // �Զ�����ģʽ�������ļ�
        std::ifstream input_file(input_file_path, std::ios::binary);
        if (!input_file.is_open()) {
            std::cerr << "Failed to open input file: " << input_file_path << std::endl;
            return false;
        }

        // ȷ������ļ�·������
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());

        // �Զ�����ģʽ��������ļ�
        std::ofstream output_file(output_file_path, std::ios::binary);
        if (!output_file.is_open()) {
            std::cerr << "Failed to create output file: " << output_file_path << std::endl;
            input_file.close();
            return false;
        }

        // �������ȡ�ļ����ݲ�����
        char* buffer = static_cast<char*>(malloc(sizeof(char) * 4096));
        if (buffer == nullptr) {
            throw std::exception("Failed to allocate memory for buffer");
        }

        while (input_file.read(buffer, sizeof(char) * 4096)) {
            const std::string decrypted_block = decrypt(private_key, std::string(buffer, input_file.gcount()));
            output_file.write(decrypted_block.data(), decrypted_block.size());
        }

        // �������һ���ļ�����
        if (input_file.gcount() > 0) {
            const std::string last_block = decrypt(private_key, std::string(buffer, input_file.gcount()));
            output_file.write(last_block.data(), last_block.size());
        }
        free(buffer);

        // �ر��ļ�
        input_file.close();
        output_file.close();

        //std::cout << "File: " << inputFilePath << "decrypted successfully, the path of output: " << outputFilePath << std::endl;
        return true;
    }

    bool RsaCipher::encryptDirectory(const std::string& public_key, const std::string input_directory, const std::string output_directory) const {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(input_directory)) {
                if (entry.is_regular_file()) {
                    const std::string input_file = entry.path().string();
                    const std::string output_file = output_directory + entry.path().string().substr(input_directory.size());
                    if (!encryptFile(public_key, input_file.c_str(), output_file.c_str()))
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

    bool RsaCipher::decryptDirectory(const std::string& private_key, const std::string input_directory, const std::string output_directory) const {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(input_directory)) {
                if (entry.is_regular_file()) {
                    const std::string input_file = entry.path().string();
                    const std::string output_file = output_directory + entry.path().string().substr(input_directory.size());
                    if (!decryptFile(private_key, input_file.c_str(), output_file.c_str()))
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

    EVP_PKEY* RsaCipher::createPKEYFromPEM(const std::string& pem_key, const bool is_public_key) const {
        BIO* bio = BIO_new_mem_buf(pem_key.c_str(), pem_key.size());
        if (!bio) {
            return nullptr;
        }

        EVP_PKEY* pkey = nullptr;
        if (is_public_key) {
            pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        }
        else {
            pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        }

        BIO_free(bio);
        return pkey;
    }

    bool RsaCipher::appendKeyToFile(const KeyPair& key_pair, const char* file_path) {
        nlohmann::ordered_json keys_json;

        // �����Կ�Ƿ��Ѵ���
        if (isKeyExists(key_pair.id_, file_path)) {
            std::cerr << "Key with ID '" << key_pair.id_ << "' already exists in the file '" << file_path << "'.\n";
            return false;
        }

        // ��ȡ��Կ�ļ�
        std::ifstream in_file(file_path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) {
            throw std::runtime_error("Failed to open file for appending.");
        }

        const std::string encrypted_key_data((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // ������Կ�ļ�
        keys_json = nlohmann::ordered_json::parse(decrypt(getEntKeyPair()->private_key_, encrypted_key_data));

        // ����µ���Կ��
        keys_json["rsa_keys"][key_pair.id_] = {
            {"public_key", key_pair.public_key_},
            {"private_key", key_pair.private_key_}
        };

        // ���л� JSON �ṹ
        const std::string serialized_keys = keys_json.dump(4);

        //std::cout << serializedKeys << std::endl;

        // ʹ�� ENT ��Կ�ļ�������Կ���� JSON ����
        const std::string encrypted_keys = encrypt(getEntKeyPair()->public_key_, serialized_keys);

        // �����ܺ������д���ļ�
        std::ofstream out_file(file_path, std::ios::out | std::ios::binary);
        if (!out_file.is_open()) {
            throw std::runtime_error("Failed to open file for writing.");
        }
        out_file.write(encrypted_keys.c_str(), encrypted_keys.size());
        out_file.close();

        return true;
    }
}
