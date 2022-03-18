#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>

// TODO: maybe rework Storage into subclassed unordered_map?
template <typename ContentType> class Storage {
protected:
    std::unordered_map<std::string, ContentType> items;

public:
    virtual ~Storage() = default;

    virtual void load(std::string path, std::string extension) = 0;

    ContentType operator[](std::string key) {
        return items[key];
    }
};

class SpriteStorage : public Storage<Texture2D> {
public:
    void load(std::string path, std::string extension) override;
};

class SoundStorage : public Storage<Sound> {
public:
    void load(std::string path, std::string extension) override;
};
