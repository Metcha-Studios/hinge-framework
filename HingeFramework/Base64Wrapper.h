#pragma once

#include <iostream>

namespace Base64Wrapper {
    extern const std::string base64_chars;
    std::string encodeBase64(const std::string& plain_text);
    std::string decodeBase64(const std::string& base64_text);
}
