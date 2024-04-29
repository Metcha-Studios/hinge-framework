#include "Base64Wrapper.h"
#include "pch.h"

#include <string>
#include <vector>
#include <algorithm>

namespace Base64Wrapper {
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encodeBase64(const std::string& plain_text) {
        std::string base64_result;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        for (const char& ch : plain_text) {
            char_array_3[i++] = ch;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) {
                    base64_result += base64_chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i > 0) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = 0;
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; j <= i; j++) {
                base64_result += base64_chars[char_array_4[j]];
            }

            while (i++ < 3) {
                base64_result += '=';
            }
        }

        return base64_result;
    }

    std::string decodeBase64(const std::string& base64_text) {
        std::string plain_result;
        int i = 0;
        unsigned char char_array_4[4];
        unsigned char char_array_3[3];

        for (const char& ch : base64_text) {
            if (ch == '=') {
                break; // 遇到填充字符，结束解码 :)
            }
            char_array_4[i++] = ch;
            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = base64_chars.find(char_array_4[i]);
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++) {
                    plain_result += char_array_3[i];
                }
                i = 0;
            }
        }

        // 检查余下的字符，补充解码结果 **
        // * 要不是为了解决解密后的明文莫名其妙多出来一个奇怪字符，我干嘛多牺牲一晚上的休息时间 :(
        if (i > 0) {
            for (int j = i; j < 4; j++) {
                char_array_4[j] = 0;
            }

            for (int j = 0; j < 4; j++) {
                char_array_4[j] = base64_chars.find(char_array_4[j]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

            for (int j = 0; j < i - 1; j++) {
                plain_result += char_array_3[j];
            }
        }

        return plain_result;
    }
}
