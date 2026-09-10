#pragma once

#include <vector>
#include <memory>
#include "ActorComponent.h"

namespace engine {

class Actor final {
     friend class Level;
public:
    ~Actor();
    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;
public:
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<ActorComponent, T>, "T must derive from ActorComponent");
        auto comp = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        mComponents.push_back(std::move(comp));
        ptr->onAttach();
        return *ptr;
    }

    template<typename T>
    T* getComponent() const {
        for (const auto& comp : mComponents) {
            if (auto* p = dynamic_cast<T*>(comp.get())) {
                return p;
            }
        }
        return nullptr;
    }

    bool isEnabled() const { return mEnabled.first; }
    Actor& setEnabled(bool enabled);

    bool isVisible() const { return mVisible.first; }
    Actor& setVisible(bool visible);

private:
    Actor();
private:
    void onBeginPlay();
    void onUpdate(float deltaTime);
    void onRender();
    void onEndPlay();
private:
    std::pair<bool, bool> mEnabled;
    std::pair<bool, bool> mVisible;
    std::vector<ActorComponentPtr> mComponents;
};

using ActorPtr = std::unique_ptr<Actor>;
} // namespace engine
