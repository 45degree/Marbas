#include "Tool/EncodingConvert.h"
#include <gtest/gtest.h>
#include <fstream>

namespace Marbas {

class EncodingConvertTest : public ::testing::Test {
protected:
};

static String ReadFromFile(const Path& filePath) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    String content;
    try {
        file.open(filePath);
        file.seekg(0, std::ios::beg);

        std::stringstream stringStream;
        stringStream << file.rdbuf();
        file.close();

        content = stringStream.str();
    }
    catch(const std::exception& e) {
        if(file.is_open()) {
            file.close();
        }

        throw e;
    }
    return content;
}

TEST_F(EncodingConvertTest, ExceptionTest) { // NOLINT
    EncodingConvert convert("utf-8", "utf-8");
    ASSERT_NO_THROW(convert.Convert(u8"你好"));
}

TEST_F(EncodingConvertTest, GkbToUTF8) { // NOLINT
    EncodingConvert convert("gb18030", "utf-8");
    String utf8Str = ReadFromFile("Test/utf-8.txt");
    String newStr = convert.Convert(ReadFromFile("Test/gb18030.txt"));

    ASSERT_STREQ(utf8Str.c_str(), newStr.c_str());
}

}  // namespace Marbas
