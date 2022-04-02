#include "Config.h"
#include "Tool/EncodingConvert.h"

#include <glog/logging.h>
#include <toml++/toml.h>
#include <uchardet/uchardet.h>
#include <iconv.h>

#include <iostream>

namespace Marbas {

Config::Config(const String& configDir): m_configDir(configDir.c_str()) {
    auto configFile = m_configDir / "config.toml";
    auto localizationPath =  m_configDir / "localization";
    auto config = toml::parse_file(configFile.c_str());
    m_fontPath = config["base"]["font"].value_or("");
    m_localeFilePath = localizationPath / config["base"]["locale"].value_or("") / "locale.toml";
}

void Config::Load() {
    LoadLocaleFile();
}

void Config::LoadLocaleFile() {
    try {
        auto file = m_localeFilePath.c_str();
        auto config = toml::parse_file(m_localeFilePath.string());
        auto menu = config["menu"];
        auto menuList = config["menu"]["menuList"];

        if(!menuList.is_array()){
            String errorMsg = FORMAT("can't parse item {} in locale config file: {}", 
                                     "menu", m_localeFilePath.string());
            throw std::runtime_error(errorMsg.c_str());
        }

        // TODO: parse locale config file
        auto menuItems = menuList.as_array();
        for (auto& item : *menuItems) {
            if(!item.is_string()) {
                String errorMsg = FORMAT("can't parse item {} in locale config file: {}", 
                                         "menu", m_localeFilePath.string())
                throw std::runtime_error(errorMsg.c_str());
            }
            auto itemStr = item.value_or("");
            auto name = String(EncodingConvertToUTF8(menu[itemStr]["name"].value_or("")));
            m_ResourcesNameMap[itemStr] = name;
        }
    }
    catch(std::exception& e) {
        LOG(ERROR) << FORMAT("can't parse locale file {}", m_localeFilePath.string());
        throw e;
    }
}

}  // namespace Marbas
