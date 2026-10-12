#pragma once

#include "ActorComponent.h"
#include "common/TargetActor.h"
#include <vector>
#include <functional>

namespace engine { class Level; }

namespace module {

class NVSComponentTarget : public engine::ActorComponent {
public:
    struct Info final {
        TargetActor::Type type;
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    };
public:
    NVSComponentTarget(engine::Actor& owner, engine::Level& level);
    ~NVSComponentTarget() override;

    void show(const std::vector<Info>& infos);

protected:
    void onBeginPlay() override;
    void onEnabledChanged(bool enabled) override {}
    void onUpdate(float deltaTime) override {}
    void onEndPlay() override;

private:
    struct PoolEntry {
        std::reference_wrapper<TargetActor> target;
        bool active;
    };

    PoolEntry& acquire();
    void releaseAll();

    engine::Level& mLevel;
    std::vector<PoolEntry> mPool;
};

} // namespace module
