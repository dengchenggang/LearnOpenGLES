#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include "ActorComponent.h"

namespace engine {

class Level;
class SceneComponent;

class Actor {
     friend class Level;
public:
    virtual ~Actor();
    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;
public:
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<ActorComponent, T>, "T must derive from ActorComponent");
        auto comp = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        mComponents.push_back(std::move(comp));
        if constexpr (std::is_base_of_v<SceneComponent, T>) {
            if (!mRootComponent) {
                mRootComponent = static_cast<SceneComponent*>(ptr);
            }
        }
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

    SceneComponent* getRootComponent() const { return mRootComponent; }

    void attachToActor(Actor& parent);
    void detachFromActor();
    Actor* getAttachParentActor() const;

protected:
    Actor(Level& level);
protected:
    virtual void onBeginPlay();
    virtual void onUpdate(float deltaTime);
    virtual void onRender();
    virtual void onEndPlay();
protected:
    Level& mLevel;
    std::pair<bool, bool> mEnabled;
    std::pair<bool, bool> mVisible;
    std::vector<ActorComponentPtr> mComponents;
    SceneComponent* mRootComponent = nullptr;
};

using ActorPtr = std::unique_ptr<Actor>;
} // namespace engine
