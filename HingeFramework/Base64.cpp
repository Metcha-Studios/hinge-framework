#include "pch.h"
#include "include/HingeFramework/Base64.h"

#include <string>
#include <sstream>
//#include <vector>
//#include <algorithm>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

namespace hinge_framework {
    //const std::string base64_chars =
    //    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const std::string base64_padding[] = { "", "==","=" };

    const char* encodeBase64(const char* plain_text) {
        namespace bai = boost::archive::iterators;

        std::stringstream os;

        // convert binary values to base64 characters
        typedef bai::base64_from_binary
            // retrieve 6 bit integers from a sequence of 8 bit bytes
            <bai::transform_width<const char*, 6, 8> > base64_enc; // compose all the above operations in to a new iterator

        std::copy(base64_enc(plain_text), base64_enc(plain_text + ((std::string)plain_text).size()),
            std::ostream_iterator<char>(os));

        os << base64_padding[((std::string)plain_text).size() % 3];
        return os.str().c_str();


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
        namespace bai = boost::archive::iterators;

        std::stringstream os;

        typedef bai::transform_width<bai::binary_from_base64<const char*>, 8, 6> base64_dec;

        unsigned int size = ((std::string)base64_text).size();

        // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
        if (size && base64_text[size - 1] == '=') {
            --size;
            if (size && base64_text[size - 1] == '=') --size;
        }
        if (size == 0) return std::string().c_str();

        std::copy(base64_dec(((std::string)base64_text).data()), base64_dec(((std::string)base64_text).data() + size),
            std::ostream_iterator<char>(os));

        return os.str().c_str();


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
}
