#pragma once

#include "Actor.h"
#include "Image.h"
#include <string>

namespace module {

class IconActor : public engine::Actor {
    friend class engine::Level;
public:
    ~IconActor() override;

    IconActor(const IconActor&) = delete;
    IconActor& operator=(const IconActor&) = delete;

    void show(const std::string& path, int32_t x, int32_t y, int32_t width, int32_t height);
    void setColor(float r, float g, float b, float a);

protected:
    IconActor(engine::Level& level, uint64_t id);

    void onBeginPlay() override;
    void onEndPlay() override;

private:
    engine::Image& mImage;
};

} // namespace module
