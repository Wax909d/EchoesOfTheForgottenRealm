#pragma once
#include <string>
#include <map>

// Config — loads plain-text "key = value" parameters from a config file so
// that gameplay numbers (player HP, enemy stats, per-level spawn counts) can
// be tuned WITHOUT recompiling. This is what satisfies the "load parameters
// from config" requirement.
class Config {
public:
    // Parse the file at `path`. Returns false if it cannot be opened; in that
    // case every getter simply returns the caller-supplied fallback value.
    bool loadFromFile(const std::string& path);

    // Typed lookups. Each returns `fallback` if the key is absent or the stored
    // text cannot be parsed into the requested type.
    int         getInt   (const std::string& key, int fallback) const;
    float       getFloat (const std::string& key, float fallback) const;
    std::string getString(const std::string& key,
                          const std::string& fallback) const;

private:
    std::map<std::string, std::string> mValues; // raw key -> raw value text
};
