#pragma once

#include <vector>
#include <memory>
#include "CActorComponent.h"

namespace engine {

class Actor {
     friend class Engine;
public:
    virtual ~Actor() = default;
    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;
public:
    void beginPlay();
    void update(float deltaTime);
    void render();
    void endPlay();

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<CActorComponent, T>, "T must derive from CActorComponent");
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

    bool isActive() const { return mActive; }
    void setActive(bool active) { mActive = active; }

protected:
    bool mActive = true;

private:
    Actor();

    std::vector<CActorComponentPtr> mComponents;
};

using ActorPtr = std::unique_ptr<Actor>;
} // namespace engine
