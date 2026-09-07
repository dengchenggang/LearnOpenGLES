#include "ModuleBase.h"
#include "IModuleManager.h"

namespace module {

ModuleBase::ModuleBase(const char* name, IModuleManager& moduleManager)
    : mName(name)
    , mModuleManager(moduleManager)
{

}

ModuleBase::~ModuleBase() = default;


void ModuleBase::changeModule(const char* moduleName) {
    mModuleManager.changeModule(moduleName);
}

}
