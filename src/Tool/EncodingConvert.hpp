// #ifndef MARBAS_TOOL_ENCODING_CONVERT_H
// #define MARBAS_TOOL_ENCODING_CONVERT_H
//
// #include "Common/Common.hpp"
//
// namespace Marbas {
//
// /**
//  * @brief Convert a string from one encoding to another
//  */
// class EncodingConvert {
//  public:
//   /**
//    * @brief set the input charactor's encoding and the output charactor's encoding
//    *
//    * @param fromEncoding the intput charactor's encoding
//    * @param toEncoding the output charactor's encoding
//    */
//   EncodingConvert(const String& fromEncoding, const String& toEncoding);
//   ~EncodingConvert();
//
//  public:
//   /**
//    * @brief convert the string from input encoding to output encoding
//    *
//    * @param str the converted string
//    *
//    * @return the string with the output encoding
//    *
//    * @note make sure that the string's encoding is the input encoding;
//    */
//   String Convert(String&& str);
//
//  private:
//   String m_fromEncoing;
//   String m_toEncoding;
//   void* m_iconHandle;
// };
//
// /**
//  * @brief Convert any encoded string to UTF-8 encoding
//  *
//  * @param str A string that needs to be converted to UTF-8 encoding
//  *
//  * @return UTF-8 encoded string
//  */
// String EncodingConvertToUTF8(String&& str);
//
// }  // namespace Marbas
//
// #endif
