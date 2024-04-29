#include "Cipher.h"
#include "pch.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <nlohmann/json.hpp>

#include "uuidV4.h"
#include "RsaCipher.h"

namespace hinge_framework {
    /*
    * public:
    */

    Cipher::Cipher() {
        // 初始化OpenSSL库
        OpenSSL_add_all_algorithms();



        // 写死密钥对
        this->ent_key_pair_->public_key_ = R"(-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4SGZ8owR01HU6EueL8qB
+faRT9e7Gs/K4XLbo5Q6QGNbPp+4/nklzX+waxvU3XkYUjjM3criL0jm3IIC2N0w
RHgJiTr+zfe3OTVscLLZxttZhGCKVZK3dFrhtHm3qL+erS92a7Lt9cRROnv3hx/n
IMLPsC9gnc79Q4kD0X1+YsHcjgkfBuK65qTlhH8ywcJH3WpHY0h0CIgevtG8J4GV
46lAK+ovIAtaJMdL3ZXZZBUPr1/hOO3mWEOBWcW4LK2HH6uJKWuXj6SWr3ocw2mh
8s982rUPcELt8cKGJKA2byuRKusrnwhlDyhPilCQGxq6q53uyi98kTTm4Z6tA53G
BQIDAQAB
-----END PUBLIC KEY-----)";
        this->ent_key_pair_->private_key_ = R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDhIZnyjBHTUdTo
S54vyoH59pFP17saz8rhctujlDpAY1s+n7j+eSXNf7BrG9TdeRhSOMzdyuIvSObc
ggLY3TBEeAmJOv7N97c5NWxwstnG21mEYIpVkrd0WuG0ebeov56tL3Zrsu31xFE6
e/eHH+cgws+wL2Cdzv1DiQPRfX5iwdyOCR8G4rrmpOWEfzLBwkfdakdjSHQIiB6+
0bwngZXjqUAr6i8gC1okx0vdldlkFQ+vX+E47eZYQ4FZxbgsrYcfq4kpa5ePpJav
ehzDaaHyz3zatQ9wQu3xwoYkoDZvK5Eq6yufCGUPKE+KUJAbGrqrne7KL3yRNObh
nq0DncYFAgMBAAECggEAKbFYWlPYZUNNY6vFYuJlCc7BAIFaVE/RKclkpbmoxA3i
cwmPcAktmA4itONVm6J2f5unTRfqpYkhwAqPzmvAKZ31XiW/1JMgqoabS4/DxxEO
VfavB8QBF5pRHGRCQA0krLwSeUWMziwKQwN8RtkAq8+IOP+9K9yOiLhSLbTI+C7H
4mWWgeOcfQjqcFCsTdao0MOfnV0HafhonaxwfNQtnkhaYtG3DGqoXqdETfTuSYhR
VeHvAqKQrAzJlqxn6d48YYXOf2SC2Y8gVyY4ilUq963bStBpTsv3UqZ3X7/EgF10
bobwI7/2yT8e6yasWH9yIFfHSzswfT9W/V6rBxXtLQKBgQD1E6memVK4NxxJq3OH
1YuAJQZpHEt49pbjVKW++teLPNPZvXrvUcK67/51Ck/FMptJdS9UNPqGNyy/FsOX
DggCPHqhgoxHDGD7X2sxAsFrCyiwH2V6XGXmL4kzQL2PKlx6asNC/VUXBFGov0ck
yphBwrTweHB7QMiqqts+mNFnawKBgQDrKlvwZeyu3xa4YZM8xEletnYgXEcwXvRy
ZrYkQNXyYrSQ6HlKxWjwrhhmq+5kYJVoh7LydV/I3Jzk0ps6Rv5QOXoC22R806CK
4DbACYRPBFHsi9EWfzf1KcWGeltBU2folFbDIr4Hlr9HDGPafVXuzOa5HVIscGx7
usnrX9+UTwKBgCG+01xDQGCdNgfu07mgbdT3RCq0+q9tRNmIjkn9wDAr7//LV2Ic
n/OdmCwFf3ZnwXob6Khvow9YCZFGUSg7hl7IdRpIH4io67GxMdFNz9QtdSzLJe3z
SxSm9v1XrMJJ0vIIQVq4Bz9kGNpjfChUvSJQ7RkvfE18XiUh4Y58/FiBAoGBANaM
E8sUQkYcdC8NnnXvIn0NfGHzrmMcDmqc+InEuSxlDxWxco9b4WzLeKmbLGhZkZYH
T6RSn2uyklVVK5eUk4p/lagrntcfgDWTb3/ubs5kQ5XKzZx3voamcHZLBRxpSsBv
LWTkOnjT+HiKfkMk3BR8zfaPIJw4iVCsznE2S9WTAoGBAMLijbMWVnlpTLDpsLh0
8ZPZw08EGlPOAmLKYQqiJrahQLvb1OBGDgv/zsr5xc3BtCc3p2t+vvmBPpV+Lw7C
PddhVvGxu7cAnb6LCKbP7noWjI3Xa3e/ZE1bcO5ktxWQtX6G30o7wP79g2Gz2Do8
tb/HErXAn5wHrxUoNAw9Q69K
-----END PRIVATE KEY-----)";
    }

    Cipher::~Cipher() {
        // 清理OpenSSL资源
        EVP_cleanup();
    }

    void Cipher::setEntKeyPair(const RsaCipher::KeyPair& ent_key_pair) {
        this->ent_key_pair_->public_key_ = ent_key_pair.public_key_;
        this->ent_key_pair_->private_key_ = ent_key_pair.private_key_;
        return;
    }

    RsaCipher::KeyPair* Cipher::getEntKeyPair() const {
        return this->ent_key_pair_;
    }

    /*
    * private:
    */

    nlohmann::ordered_json Cipher::createBinaryEntFile(const char* ent_file_path, const char* version) const {
        const RsaCipher rsa;

        // 创建初始的 JSON 结构
        nlohmann::ordered_json keys_json;
        keys_json["version"] = version;
        keys_json["aes256_keys"] = {};
        keys_json["rsa_keys"] = {};

        const std::string serialized_keys = keys_json.dump(4);

        // 使用 ENT 密钥加密 JSON 数据
        const std::string encrypted_keys = rsa.encrypt(this->ent_key_pair_->public_key_, serialized_keys);

        // 确保文件所在路径存在
        std::filesystem::create_directories(std::filesystem::path(ent_file_path).parent_path());

        // 将加密后的数据写入二进制文件
        std::ofstream out_file(ent_file_path, std::ios::out | std::ios::binary);
        if (!out_file.is_open()) {
            throw std::runtime_error("Failed to create binary key file.");
        }
        out_file.write(encrypted_keys.c_str(), encrypted_keys.size());
        out_file.close();

        return keys_json;
    }
}
