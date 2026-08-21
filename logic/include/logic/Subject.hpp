#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "logic/Observer.hpp"

namespace logic {

/**
 * @brief Base class implementing the Subject side of the Observer pattern.
 *
 * Manages a collection of observers, allowing them to attach, detach,
 * and receive event notifications broadcasted by the subject.
 */
class Subject {
public:
    /**
     * @brief Virtual destructor for the Subject.
     */
    virtual ~Subject() = default;

    /**
     * @brief Attaches a new observer to receive notifications.
     * @param observer Shared pointer to the Observer being added.
     */
    void attach(const std::shared_ptr<Observer>& observer) { observers_.push_back(observer); }

    /**
     * @brief Detaches an existing observer, stopping notifications.
     * @param observer Shared pointer to the Observer being removed.
     */
    void detach(const std::shared_ptr<Observer>& observer) {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                         observers_.end());
    }

    /**
     * @brief Checks whether a specific observer is currently attached.
     *
     * @param observer Shared pointer to the Observer being checked.
     * @return True if the observer is attached, false otherwise.
     */
    bool hasObserver(const std::shared_ptr<Observer>& observer) const {
        return std::find(observers_.begin(), observers_.end(), observer) != observers_.end();
    }

protected:
    /**
     * @brief Broadcasts an event to all attached observers.
     * @param event The event details to send.
     */
    void notify(const Event& event) const {
        for (const auto& observer : observers_) {
            if (observer) {
                observer->onNotify(event);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Observer> > observers_; ///< Collection of registered observers.
};

} // namespace logic
