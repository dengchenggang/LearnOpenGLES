#pragma once

#include "Actor.h"
#include "Image.h"
#include <string>

typedef struct AHardwareBuffer AHardwareBuffer;

namespace module {

class IconActor : public engine::Actor {
public:
    ~IconActor() override;

    IconActor(const IconActor&) = delete;
    IconActor& operator=(const IconActor&) = delete;

    engine::Image& getImage() const { return mImage; }

    bool setPath(const std::string& path);
    bool setHardwareBuffer(const std::string& url, AHardwareBuffer* buffer);

    void setPosition(float x, float y);
    void setSize(float width, float height);

protected:
    IconActor(engine::Level& level, uint64_t id);

    void onBeginPlay() override;
    void onEndPlay() override;

private:
    engine::Image& mImage;
};

} // namespace module
