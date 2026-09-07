#pragma once
#include <string>

namespace engine {
class ILevelManager {
public:
    virtual void changeLevel(const std::string& levelName) = 0;
};
}