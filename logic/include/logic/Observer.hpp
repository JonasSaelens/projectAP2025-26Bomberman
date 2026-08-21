#pragma once

#include "logic/Event.hpp"

namespace logic {
    /**
 * @brief Interface for objects that wish to receive notifications from a Subject.
 *
 * Implements the listener side of the Observer pattern, allowing decoupled
 * components to react to game events dispatched by entities or the world.
 */
    class Observer {
    public:
        /**
     * @brief Virtual destructor for the Observer interface.
     */
        virtual ~Observer() = default;

        /**
     * @brief Callback method invoked when a subscribed subject dispatches an event.
     *
     * @param event The event details containing information about what occurred.
     */
        virtual void onNotify(const Event &event) = 0;
    };
} // namespace logic
