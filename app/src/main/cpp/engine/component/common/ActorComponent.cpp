#include "ActorComponent.h"
#include "Actor.h"

namespace engine {

ActorComponent::ActorComponent(Actor& owner) : mOwner(owner) {}

ActorComponent::~ActorComponent() {}

} // namespace engine
