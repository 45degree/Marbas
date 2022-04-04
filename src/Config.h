#ifndef MARBAS_CONFIG_H
#define MARBAS_CONFIG_H

#include "Common.h"

#include <map>

namespace Marbas {

class Config {
public:
    explicit Config(const String& configDir);
    ~Config() = default;

public:
    void Load();

private:
    void LoadLocaleFile();

private:
    Path m_configDir;
    Path m_fontPath;
    Path m_localeFilePath;

    std::map<String, String> m_ResourcesNameMap;
};

}  // namespace Marbas

#endif