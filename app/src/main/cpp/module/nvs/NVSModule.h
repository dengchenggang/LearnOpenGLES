#pragma once
#include "level/Level.h"
#include "utils/config/JsonConfigLoader.h"
#include <memory>

namespace module {

template<typename T>
struct Referrence {
    T& ref;
    explicit Referrence(T& p) : ref(p) {}
};

template<typename T>
using ReferrencePtr = std::unique_ptr<Referrence<T>>;

class NVSModule : public engine::Level {
public:
    NVSModule(engine::ILevelManager& levelManager, const JsonConfigLoaderPtr& config);
    ~NVSModule() override;
protected:
    void onInit() override;
    void onResize(int32_t width, int32_t height) override;
    void onDeInit() override;
private:
    JsonConfigLoaderPtr mConfig;
    ReferrencePtr<engine::Actor> mActor;
};

}