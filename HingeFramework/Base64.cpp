#include "pch.h"
#include "include/HingeFramework/Base64.h"

#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace hinge_framework {
    //const std::string base64_chars =
    //    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    //const std::string base64_padding[] = { "", "==","=" };

    const char* encodeBase64(const char* plain_text) {
        using namespace boost::archive::iterators;
        std::vector<unsigned char> binary(plain_text, plain_text + strlen(plain_text));
        using It = base64_from_binary<transform_width<std::vector<unsigned char>::const_iterator, 6, 8>>;
        auto base64 = std::string(It(binary.begin()), It(binary.end()));
        // Add padding.
        base64.append((3 - binary.size() % 3) % 3, '=');

        // Allocate memory on the heap and copy the string
        std::unique_ptr<char[]> result(new char[base64.size() + 1]);
        strcpy_s(result.get(), base64.size() + 1, base64.c_str());
        return result.release();


        //std::string base64_result;
        //int i = 0;
        //int j = 0;
        //unsigned char char_array_3[3];
        //unsigned char char_array_4[4];

        //for (const char& ch : plain_text) {
        //    char_array_3[i++] = ch;
        //    if (i == 3) {
        //        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        //        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        //        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        //        char_array_4[3] = char_array_3[2] & 0x3f;

        //        for (i = 0; i < 4; i++) {
        //            base64_result += base64_chars[char_array_4[i]];
        //        }
        //        i = 0;
        //    }
        //}

        //if (i > 0) {
        //    for (j = i; j < 3; j++) {
        //        char_array_3[j] = 0;
        //    }

        //    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        //    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        //    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        //    char_array_4[3] = char_array_3[2] & 0x3f;

        //    for (j = 0; j <= i; j++) {
        //        base64_result += base64_chars[char_array_4[j]];
        //    }

        //    while (i++ < 3) {
        //        base64_result += '=';
        //    }
        //}

        //return base64_result.c_str();
    }

    const char* decodeBase64(const char* base64_text) {
        using namespace boost::archive::iterators;
        std::string base64(base64_text);
        using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
        std::vector<unsigned char> binary(It(base64.begin()), It(base64.end()));
        // Remove padding.
        auto length = base64.size();
        if (binary.size() > 2 && base64[length - 1] == '=' && base64[length - 2] == '=')
        {
            binary.erase(binary.end() - 2, binary.end());
        }
        else if (binary.size() > 1 && base64[length - 1] == '=')
        {
            binary.erase(binary.end() - 1, binary.end());
        }

        // Allocate memory on the heap and copy the binary data
        std::unique_ptr<char[]> result(new char[binary.size() + 1]);
        memcpy(result.get(), binary.data(), binary.size());
        result[binary.size()] = '\0';
        return result.release();


        //std::string plain_result;
        //int i = 0;
        //unsigned char char_array_4[4];
        //unsigned char char_array_3[3];

        //for (const char& ch : base64_text) {
        //    if (ch == '=') {
        //        break; // 遇到填充字符，结束解码 :)
        //    }
        //    char_array_4[i++] = ch;
        //    if (i == 4) {
        //        for (i = 0; i < 4; i++) {
        //            char_array_4[i] = base64_chars.find(char_array_4[i]);
        //        }

        //        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        //        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        //        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        //        for (i = 0; i < 3; i++) {
        //            plain_result += char_array_3[i];
        //        }
        //        i = 0;
        //    }
        //}

        //// 检查余下的字符，补充解码结果 **
        //// * 要不是为了解决解密后的明文莫名其妙多出来一个奇怪字符，我干嘛多牺牲一晚上的休息时间 :(
        //if (i > 0) {
        //    for (int j = i; j < 4; j++) {
        //        char_array_4[j] = 0;
        //    }

        //    for (int j = 0; j < 4; j++) {
        //        char_array_4[j] = base64_chars.find(char_array_4[j]);
        //    }

        //    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        //    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        //    for (int j = 0; j < i - 1; j++) {
        //        plain_result += char_array_3[j];
        //    }
        //}

        //return plain_result.c_str();
    }

    const char* encodeBase64FromStr(const std::string& plain_text) {
        using namespace boost::archive::iterators;

        std::vector<unsigned char> binary(plain_text.begin(), plain_text.end());

        using It = base64_from_binary<transform_width<std::vector<unsigned char>::const_iterator, 6, 8>>;

        auto base64 = std::string(It(binary.begin()), It(binary.end()));

        base64.append((3 - binary.size() % 3) % 3, '=');

        std::unique_ptr<char[]> result(new char[base64.size() + 1]);
        strcpy_s(result.get(), base64.size() + 1, base64.c_str());

        return result.release();
    }

    std::string decodeBase64ToStr(const char* base64_text) {
        using namespace boost::archive::iterators;

        std::string base64(base64_text);

        using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;

        std::vector<unsigned char> binary(It(base64.begin()), It(base64.end()));

        // Remove padding
        auto length = base64.size();
        if (binary.size() > 2 && base64[length - 1] == '=' && base64[length - 2] == '=')
        {
            binary.erase(binary.end() - 2, binary.end());
        }
        else if (binary.size() > 1 && base64[length - 1] == '=')
        {
            binary.erase(binary.end() - 1, binary.end());
        }

        return std::string(binary.begin(), binary.end());
    }
}
