#pragma once

#include <SFML/System/Vector2.hpp>

#include "logic/Vector2.hpp"

namespace representation {
    /**
 * @brief Handles coordinate conversions between world-space and window screen-space.
 *
 * Maps logical 2D world coordinates (typically normalized or scaled)
 * to pixel-based SFML window coordinates for rendering.
 */
    class Camera {
    public:
        /**
     * @brief Constructs a new Camera instance.
     *
     * @param windowWidth The initial width of the display window in pixels.
     * @param windowHeight The initial height of the display window in pixels.
     */
        Camera(unsigned windowWidth, unsigned windowHeight)
            : width_(windowWidth), height_(windowHeight) {
        }

        /**
     * @brief Converts a 2D world position into screen pixel coordinates.
     *
     * @param worldPosition The logical world position vector.
     * @return An sf::Vector2f representing pixel coordinates on the screen.
     */
        sf::Vector2f worldToScreen(logic::Vector2 worldPosition) const {
            const float x = static_cast<float>((worldPosition.x + 1.0) / 2.0 * width_);
            const float y = static_cast<float>((worldPosition.y + 1.0) / 2.0 * height_);
            return {x, y};
        }

        /**
     * @brief Converts a 2D world dimension/size into screen scale pixel dimensions.
     *
     * @param worldSize The logical world size vector.
     * @return An sf::Vector2f representing pixel dimensions on the screen.
     */
        sf::Vector2f worldToScreenScale(logic::Vector2 worldSize) const {
            const float w = static_cast<float>(worldSize.x / 2.0 * width_);
            const float h = static_cast<float>(worldSize.y / 2.0 * height_);
            return {w, h};
        }

        /**
     * @brief Updates the target window dimensions used for coordinate scaling.
     *
     * @param width The new window width in pixels.
     * @param height The new window height in pixels.
     */
        void setWindowSize(unsigned width, unsigned height) {
            width_ = width;
            height_ = height;
        }

    private:
        unsigned width_; ///< Current rendering window width in pixels.
        unsigned height_; ///< Current rendering window height in pixels.
    };
} // namespace representation
