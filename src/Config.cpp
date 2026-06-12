#include "Config.h"
#include <fstream>

namespace {
// Strip leading/trailing whitespace from a copy of `s`.
std::string trim(const std::string& s) {
    const auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    const auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
} // namespace

bool Config::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')      // skip blanks and comments
            continue;
        const auto eq = line.find('=');
        if (eq == std::string::npos)             // ignore malformed lines
            continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));
        if (!key.empty())
            mValues[key] = val;
    }
    return true;
}

int Config::getInt(const std::string& key, int fallback) const {
    auto it = mValues.find(key);
    if (it == mValues.end()) return fallback;
    try { return std::stoi(it->second); } catch (...) { return fallback; }
}

float Config::getFloat(const std::string& key, float fallback) const {
    auto it = mValues.find(key);
    if (it == mValues.end()) return fallback;
    try { return std::stof(it->second); } catch (...) { return fallback; }
}

std::string Config::getString(const std::string& key,
                              const std::string& fallback) const {
    auto it = mValues.find(key);
    return it == mValues.end() ? fallback : it->second;
}
