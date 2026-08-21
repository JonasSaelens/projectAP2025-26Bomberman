#pragma once

#include <chrono>

namespace logic {

/**
 * @brief A thread-safe singleton utility class for tracking frame time delta.
 *
 * Utilizes steady clocks to measure the elapsed time between consecutive
 * game loop ticks (`deltaTime`), facilitating frame-rate independent movement and updates.
 */
class Stopwatch {
public:
    /**
     * @brief Retrieves the singleton instance of the Stopwatch utility.
     * @return Reference to the global Stopwatch instance.
     */
    static Stopwatch& getInstance() {
        static Stopwatch instance;
        return instance;
    }

    Stopwatch(const Stopwatch&) = delete;

    Stopwatch& operator=(const Stopwatch&) = delete;

    /**
     * @brief Updates the clock, calculating the elapsed time since the last tick.
     */
    void tick() {
        const auto now = Clock::now();
        deltaTime_ = std::chrono::duration<double>(now - lastTick_).count();
        lastTick_ = now;
    }

    /**
     * @brief Gets the time elapsed during the previous frame in seconds.
     * @return The delta time value.
     */
    double getDeltaTime() const { return deltaTime_; }

private:
    using Clock = std::chrono::steady_clock; ///< The steady clock type used for time measurements.

    /**
     * @brief Private constructor initializing the initial tick timestamp.
     */
    Stopwatch()
        : lastTick_(Clock::now()) {
    }

    Clock::time_point lastTick_; ///< Timestamp of the most recent tick call.
    double deltaTime_ = 0.0;     ///< Duration of the last frame in seconds.
};

} // namespace logic
