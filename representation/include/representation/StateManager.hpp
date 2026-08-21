#pragma once

#include <memory>

#include "representation/State.hpp"

namespace representation {

/**
 * @brief Manages application state transitions and holds the active state.
 *
 * Acts as a finite state machine controller, enabling switching between
 * different screens (e.g., menu, gameplay, game over) and providing access to the current active state.
 */
class StateManager {
public:
    /**
     * @brief Replaces the current state with a new state.
     * @param state Shared pointer to the new State instance to activate.
     */
    void setState(std::shared_ptr<State> state) { currentState_ = std::move(state); }
    /**
     * @brief Retrieves a raw pointer to the currently active state.
     * @return Pointer to the active State, or nullptr if none is set.
     */
    State* getCurrentState() const { return currentState_.get(); }

private:
    std::shared_ptr<State> currentState_; ///< Smart pointer holding the active application state.
};
} // namespace representation
