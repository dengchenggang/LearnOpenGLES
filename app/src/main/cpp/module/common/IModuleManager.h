#pragma once

namespace module {
class IModuleManager {
public:
    virtual void changeModule(const char* moduleName) = 0;
};
}