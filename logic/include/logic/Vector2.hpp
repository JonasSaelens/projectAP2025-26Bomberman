#pragma once

namespace logic {

/**
 * @brief Represents a 2D vector with double-precision coordinates.
 *
 * Commonly used for positions, sizes, velocities, and offsets in the game world,
 * providing standard arithmetic operators for vector math.
 */
struct Vector2 {
    double x = 0.0; ///< The x-coordinate component.
    double y = 0.0; ///< The y-coordinate component.

    /**
     * @brief Constructs a zero-initialized Vector2 (0, 0).
     */
    Vector2() = default;

    /**
     * @brief Constructs a Vector2 with specified x and y values.
     *
     * @param xIn The x-coordinate value.
     * @param yIn The y-coordinate value.
     */
    Vector2(double xIn, double yIn)
        : x(xIn), y(yIn) {
    }

    /**
     * @brief Adds another vector to this vector component-wise.
     * @param other The Vector2 to add.
     * @return A new Vector2 representing the sum.
     */
    Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
    /**
     * @brief Subtracts another vector from this vector component-wise.
     * @param other The Vector2 to subtract.
     * @return A new Vector2 representing the difference.
     */
    Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
    /**
     * @brief Multiplies this vector by a scalar value.
     * @param scalar The scalar multiplier.
     * @return A new Vector2 scaled by the given factor.
     */
    Vector2 operator*(double scalar) const { return {x * scalar, y * scalar}; }
};

} // namespace logic
