#pragma once

#include "Actor.h"
#include "Image.h"
#include "utils/config/JsonConfigLoader.h"

namespace module {

class NVSActor : public engine::Actor {
    friend class engine::Level;
public:
    ~NVSActor() override;

    NVSActor(const NVSActor&) = delete;
    NVSActor& operator=(const NVSActor&) = delete;

    void Resize(int32_t width, int32_t height);

protected:
    NVSActor(engine::Level& level, uint64_t id, const JsonConfigLoaderPtr& config);

    void onBeginPlay() override;
    void onEndPlay() override;

private:
    JsonConfigLoaderPtr mConfig;
    engine::Image& mImage;
};

} // namespace module
