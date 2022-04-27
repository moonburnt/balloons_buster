#pragma once

#include "platform.hpp"

class PlatformLinux : public Platform {
public:
    std::string get_resource_dir() override;
    std::string get_sprites_dir() override;
    std::string get_sounds_dir() override;
};
