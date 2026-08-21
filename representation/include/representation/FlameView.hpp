#pragma once

#include "representation/EntityView.hpp"
#include <vector>

namespace representation {
    /**
 * @brief Handles the visual presentation and frame-by-frame animation for explosion flame segments.
 *
 * Inherits from EntityView to render animated explosion sprites using ping-pong looping
 * through a sequence of texture rectangles.
 */
    class FlameView : public EntityView {
    public:
        /**
     * @brief Constructs a new FlameView instance.
     *
     * @param texture Shared pointer to the SFML texture atlas.
     * @param frames A vector of sf::IntRect texture coordinates representing the animation sequence.
     * @param size The world-space dimensions of the flame segment.
     */
        FlameView(std::shared_ptr<sf::Texture> texture,
                  const std::vector<sf::IntRect> &frames,
                  logic::Vector2 size)
            : EntityView(std::move(texture)), frames_(frames) {
            if (!frames_.empty()) {
                sprite_.setTextureRect(frames_[0]);
            }
            setSize(size);
        }

        /**
     * @brief Handles incoming game events to reset or update the flame view state.
     * @param event The event details dispatched by the subject.
     */
        void onNotify(const logic::Event &event) override {
            EntityView::onNotify(event);

            switch (event.type) {
                case logic::EventType::Spawned:
                    animTime_ = 0.0;
                    break;
                case logic::EventType::Died:
                    break;
                default:
                    break;
            }
        }

        /**
     * @brief Updates the flame's animation frame using a ping-pong loop over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void updateAnimation(double deltaTime) {
            if (!alive_ || frames_.empty()) return;
            animTime_ += deltaTime;

            const int baseFrameCount = static_cast<int>(frames_.size());
            if (baseFrameCount < 1) return;

            if (baseFrameCount == 1) {
                sprite_.setTextureRect(frames_[0]);
                return;
            }

            const int pingpongLen = 2 * baseFrameCount - 2;
            const int frameIndex = static_cast<int>(animTime_ / frameDuration_) % pingpongLen;

            int frame;
            if (frameIndex < baseFrameCount) {
                frame = frameIndex;
            } else {
                frame = 2 * baseFrameCount - 2 - frameIndex;
            }

            sprite_.setTextureRect(frames_[frame]);
        }

    private:
        std::vector<sf::IntRect> frames_; ///< Sequence of texture source rectangles for the animation loop.
        double animTime_ = 0.0; ///< Accumulated time tracking animation progress.
        double frameDuration_ = 0.08; ///< Duration each individual frame is displayed before switching.
    };
} // namespace representation