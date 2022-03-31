#include "Config.h"

#include <toml++/toml.h>

namespace Marbas {

Config::Config(const String& configDir): m_configDir(configDir) {
    auto config = toml::parse_file(configDir.c_str());
    m_fontPath = config["base"]["font"].value_or("");
    m_localeFilePath = config["base"]["locale"].value_or("");
}

void Config::Load() {
}

void Config::LoadLocaleFile() {
    try {
        auto config = toml::parse_file(m_localeFilePath.c_str());
        auto menu = config["menu"];

        if(!menu.is_value() && !menu.is_array()){
            String errorMsg = FORMAT("can't parse item {} in locale config file: {}", 
                                     "menu", m_localeFilePath)
            throw std::runtime_error(errorMsg);
        }

        // TODO: parse locale config file
        auto menuItems = menu.as_array();
        for (auto& item : *menuItems) {
            if(!item.is_string()) {
                String errorMsg = FORMAT("can't parse item {} in locale config file: {}", 
                                         "menu", m_localeFilePath)
                throw std::runtime_error(errorMsg);
            }
            auto itemStr = item.as_string();
        }
    }
    catch(std::exception& e) {
        LOG(ERROR) << FORMAT("can't parse locale file {}", m_localeFilePath);
        throw e;
    }
}

}  // namespace Marbas
