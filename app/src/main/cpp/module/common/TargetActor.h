#pragma once

#include "Actor.h"
#include "IconActor.h"

namespace module {

class TargetActor : public engine::Actor {
    friend class engine::Level;
public:
    enum class Type {
        Green,
        Yellow,
        Red
    };
public:
    ~TargetActor() override;

    TargetActor(const TargetActor&) = delete;
    TargetActor& operator=(const TargetActor&) = delete;

    void show(Type type, int32_t x, int32_t y, int32_t width, int32_t height);
    void hide();

protected:
    TargetActor(engine::Level& level, uint64_t id);

    void onBeginPlay() override;
    void onEndPlay() override;

private:
    IconActor& mFrame;
    IconActor& mAlert;
};

} // namespace module
