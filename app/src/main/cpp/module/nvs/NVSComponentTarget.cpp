#include "NVSComponentTarget.h"
#include "Level.h"
#include "utils/Log.h"

namespace module {

KEY_VALUE(TAG, NVSComponentTarget)

NVSComponentTarget::NVSComponentTarget(engine::Actor& owner, engine::Level& level)
    : engine::ActorComponent(owner)
    , mLevel(level)
{
}

NVSComponentTarget::~NVSComponentTarget() = default;

void NVSComponentTarget::show(const std::vector<Info>& infos) {
    releaseAll();

    for (const auto& info : infos) {
        auto& entry = acquire();
        entry.target.get().show(info.type, info.x, info.y, info.width, info.height);
    }

    LogI("%s show: count=%zu, pool size=%zu", TAG, infos.size(), mPool.size());
}

NVSComponentTarget::PoolEntry& NVSComponentTarget::acquire() {
    for (auto& entry : mPool) {
        if (!entry.active) {
            entry.active = true;
            return entry;
        }
    }

    auto& target = mLevel.createActor<TargetActor>();
    target.attachToActor(GetOwner());
    mPool.push_back({target, true});
    LogI("%s acquire (new), pool size=%zu", TAG, mPool.size());
    return mPool.back();
}

void NVSComponentTarget::releaseAll() {
    for (auto& entry : mPool) {
        if (entry.active) {
            entry.active = false;
            entry.target.get().hide();
        }
    }
}

void NVSComponentTarget::onBeginPlay() {
    LogI("%s onBeginPlay", TAG);
}

void NVSComponentTarget::onEndPlay() {
    LogI("%s onEndPlay", TAG);
    releaseAll();
}

} // namespace module
