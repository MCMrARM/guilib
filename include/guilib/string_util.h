#pragma once

namespace guilib {

class StrUtil {

public:

    static void appendUTF8Char(std::string& str, int ch);
    static bool equalsIgnoreCase(const std::string& str, const std::string& str2);

};

}