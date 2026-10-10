#pragma once

#include "Actor.h"
#include "IconActor.h"
#include <vector>
#include <string>
#include <functional>

namespace module {

class PoolIconActor : public engine::Actor {
public:
    ~PoolIconActor() override;

    PoolIconActor(const PoolIconActor&) = delete;
    PoolIconActor& operator=(const PoolIconActor&) = delete;
public:
    IconActor* acquire(engine::Actor& parent);
    void release(IconActor& icon);
    void releaseAll();

    size_t activeCount() const;
    size_t poolSize() const { return mPool.size(); }

protected:
    PoolIconActor(engine::Level& level, uint64_t id, size_t initialSize = 0);

    void onBeginPlay() override;
    void onEndPlay() override;

private:
    IconActor& createIcon();

    struct IconEntry {
        std::reference_wrapper<IconActor> icon;
        bool active;
    };
    std::vector<IconEntry> mPool;
};

} // namespace module
