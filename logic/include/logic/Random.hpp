#pragma once

#include <random>

namespace logic {

/**
 * @brief A thread-safe singleton utility class for generating pseudo-random numbers.
 *
 * Encapsulates standard C++ random number facilities (`std::mt19937` and distributions)
 * to provide convenient methods for generating random integers, real numbers, and probability checks.
 */
class Random {
public:
    /**
     * @brief Retrieves the singleton instance of the Random utility.
     * @return Reference to the global Random instance.
     */
    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    Random(const Random&) = delete;

    Random& operator=(const Random&) = delete;

    /**
     * @brief Generates a random integer within a specified inclusive range.
     *
     * @param min The minimum possible integer value (inclusive).
     * @param max The maximum possible integer value (inclusive).
     * @return A random integer between min and max.
     */
    int getInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator_);
    }

    /**
     * @brief Generates a random floating-point number within a specified range.
     *
     * @param min The minimum possible real value.
     * @param max The maximum possible real value.
     * @return A random real number between min and max.
     */
    double getReal(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator_);
    }

    /**
     * @brief Performs a boolean probability check.
     *
     * @param probability The success probability threshold (between 0.0 and 1.0).
     * @return True if a random roll falls below the probability threshold, false otherwise.
     */
    bool chance(double probability) { return getReal(0.0, 1.0) < probability; }

private:
    /**
     * @brief Private constructor initializing the Mersenne Twister engine with a random seed.
     */
    Random()
        : generator_(std::random_device{}()) {
    }

    std::mt19937 generator_; ///< The underlying Mersenne Twister 32-bit pseudo-random number generator.
};

} // namespace logic
