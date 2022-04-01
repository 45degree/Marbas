#ifndef MARBAS_TOOL_ENCODING_CONVERT_H
#define MARBAS_TOOL_ENCODING_CONVERT_H

#include "Common.h"

namespace Marbas {

class EncodingConvert {
public:
    EncodingConvert(const String& fromEncoding, const String& toEncoding);
    ~EncodingConvert();

public:
    String Convert(String&& str);

private:
    String m_fromEncoing;
    String m_toEncoding;
    void* m_iconHandle;
};

}  // namespace Marbas

#endif
