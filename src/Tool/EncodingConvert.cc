// #include "Tool/EncodingConvert.hpp"
//
// #include <iconv.h>
// #include <uchardet/uchardet.h>
//
// namespace Marbas {
//
// EncodingConvert::EncodingConvert(const String& fromEncoding, const String& toEncoding)
//     : m_fromEncoing(fromEncoding), m_toEncoding(toEncoding) {
//   m_iconHandle = iconv_open(m_toEncoding.c_str(), m_fromEncoing.c_str());
// }
//
// EncodingConvert::~EncodingConvert() { iconv_close(m_iconHandle); }
//
// String EncodingConvert::Convert(String&& str) {
//   char* inputBuffer = str.data();
//   size_t inputLeft = str.size();
//
//   String res;
//   res.resize(str.size());
//   char* outputBuffer = res.data();
//   size_t outputLeft = res.size();
//
//   auto ret = iconv(m_iconHandle, &inputBuffer, &inputLeft, &outputBuffer, &outputLeft);
//   size_t oldLen = res.size() - outputLeft;
//
//   while (ret == static_cast<decltype(ret)>(-1)) {
//     res.resize(res.size() * 2);
//
//     outputBuffer = res.data() + oldLen;
//     outputLeft = res.size() - oldLen;
//
//     ret = iconv(m_iconHandle, &inputBuffer, &inputLeft, &outputBuffer, &outputLeft);
//
//     oldLen = res.size() - outputLeft;
//   }
//   return res;
// }
//
// String EncodingConvertToUTF8(String&& str) {
//   uchardet_t handle = uchardet_new();
//   uchardet_handle_data(handle, str.c_str(), str.size());
//   uchardet_data_end(handle);
//
//   const char* charset = uchardet_get_charset(handle);
//
//   EncodingConvert convert(charset, "utf-8");
//
//   return convert.Convert(std::move(str));
// }
//
// }  // namespace Marbas
