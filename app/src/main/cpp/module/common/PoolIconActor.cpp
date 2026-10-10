#include "PoolIconActor.h"
#include "Level.h"
#include "utils/Log.h"

namespace module {

KEY_VALUE(TAG, PoolIconActor)

PoolIconActor::PoolIconActor(engine::Level& level, uint64_t id, size_t initialSize)
    : Actor(level, id)
{
    for (size_t i = 0; i < initialSize; ++i) {
        createIcon();
    }
}

PoolIconActor::~PoolIconActor() = default;

IconActor& PoolIconActor::createIcon() {
    auto& icon = mLevel.createActor<IconActor>();
    icon.attachToActor(*this);
    icon.setEnabled(false).setVisible(false);
    mPool.push_back({icon, false});
    return icon;
}

IconActor* PoolIconActor::acquire(engine::Actor& parent) {
    IconActor* result = nullptr;
    for (auto& entry : mPool) {
        if (!entry.active) {
            result = &entry.icon.get();
            entry.active = true;
            break;
        }
    }
    if (!result) {
        auto& icon = createIcon();
        mPool.back().active = true;
        result = &icon;
        LogI("%s acquire icon id=%llu (new), pool size=%zu", TAG, result->getId(), mPool.size());
    } else {
        LogI("%s acquire icon id=%llu", TAG, result->getId());
    }
    result->detachFromActor();
    result->attachToActor(parent);
    result->setEnabled(true).setVisible(true);
    return result;
}

void PoolIconActor::release(IconActor& icon) {
    for (auto& entry : mPool) {
        if (&entry.icon.get() == &icon) {
            entry.active = false;
            icon.detachFromActor();
            icon.attachToActor(*this);
            icon.setEnabled(false).setVisible(false);
            LogI("%s release icon id=%llu", TAG, icon.getId());
            return;
        }
    }
    LogW("%s release: icon id=%llu not found in pool", TAG, icon.getId());
}

void PoolIconActor::releaseAll() {
    for (auto& entry : mPool) {
        if (entry.active) {
            entry.active = false;
            entry.icon.get().setEnabled(false).setVisible(false);
        }
    }
    LogI("%s releaseAll, pool size=%zu", TAG, mPool.size());
}

size_t PoolIconActor::activeCount() const {
    size_t count = 0;
    for (const auto& entry : mPool) {
        if (entry.active) ++count;
    }
    return count;
}

void PoolIconActor::onBeginPlay() {
    LogI("%s onBeginPlay, id=%llu, pool size=%zu", TAG, getId(), mPool.size());
    setEnabled(true).setVisible(true);
    Actor::onBeginPlay();
}

void PoolIconActor::onEndPlay() {
    LogI("%s onEndPlay, id=%llu", TAG, getId());
    releaseAll();
    Actor::onEndPlay();
}

} // namespace module
