#pragma once
#include <cstdint>
#include <string>

namespace module {
class IModuleManager;

class ModuleBase {
public:
    ModuleBase(const char* name, IModuleManager& moduleManager);
    virtual ~ModuleBase();
    ModuleBase(const ModuleBase&) = delete;
    ModuleBase& operator=(const ModuleBase&) = delete;
public:
    const std::string& getId() const { return mName; }

    virtual void initialize() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void deinitialize() = 0;
protected:
    void changeModule(const char* moduleName);
private:
    const std::string mName;
    IModuleManager& mModuleManager;
};

}
