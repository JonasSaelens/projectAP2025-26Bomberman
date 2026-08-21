#pragma once

#include "representation/EntityView.hpp"

namespace representation {

/**
 * @brief Handles the visual presentation and destruction animation for a Wall entity.
 *
 * Inherits from EntityView to render wall sprites and manage destruction sequence animations
 * when a destructible block is blown up via observer notifications.
 */
class WallView : public EntityView {
public:
    using EntityView::EntityView;

    /**
     * @brief Constructs a new WallView instance with specific texture region and size.
     *
     * @param texture Shared pointer to the SFML texture atlas.
     * @param tileRect The texture source rectangle representing the wall sprite.
     * @param size The world-space dimensions of the wall.
     */
    WallView(std::shared_ptr<sf::Texture> texture, const sf::IntRect& tileRect, logic::Vector2 size)
        : EntityView(std::move(texture)) {
        baseRect_ = tileRect;
        sprite_.setTextureRect(tileRect);
        setSize(size);
    }

    /**
     * @brief Handles incoming game events to trigger destruction animations when a wall dies.
     * @param event The event details dispatched by the subject.
     */
    void onNotify(const logic::Event& event) override {
        EntityView::onNotify(event);

        switch (event.type) {
        case logic::EventType::Died:
            if (hasBeenDrawn_) {
                isAnimating_ = true;
                animTime_ = 0.0;
                frame_ = 0;
            }
            break;

        default:
            break;
        }
    }

    /**
     * @brief Updates the wall's destruction animation frames over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void updateAnimation(double deltaTime) {
        if (!isAnimating_)
            return;

        animTime_ += deltaTime;
        frame_ = static_cast<int>(animTime_ / frameDuration_) % 6;

        const int frameW = baseRect_.width;
        const int frameH = baseRect_.height;
        sprite_.setTextureRect(sf::IntRect(
            baseRect_.left + frame_ * frameW,
            baseRect_.top,
            frameW,
            frameH
            ));

        if (frame_ == 0 && animTime_ >= frameDuration_ * 6) {
            isAnimating_ = false;
        }
    }

    /**
     * @brief Renders the wall sprite or its destruction animation frame onto the window.
     *
     * @reference window The SFML render window target.
     * @reference camera The camera handling world-to-screen coordinate mapping.
     */
    void draw(sf::RenderWindow& window, const Camera& camera) override {
        hasBeenDrawn_ = true;

        if (!alive_ && !isAnimating_)
            return;

        const sf::Vector2f targetPx = camera.worldToScreenScale(size_);

        sf::IntRect rect = sprite_.getTextureRect();
        if (rect.width == 0 || rect.height == 0) {
            const sf::Vector2u texSize = texture_->getSize();
            rect = sf::IntRect(0, 0, static_cast<int>(texSize.x), static_cast<int>(texSize.y));
        }

        const float sx = targetPx.x / static_cast<float>(rect.width);
        const float sy = targetPx.y / static_cast<float>(rect.height);

        sprite_.setOrigin(rect.width * 0.5f, rect.height * 0.5f);
        sprite_.setScale(sx, sy);
        sprite_.setPosition(camera.worldToScreen(lastKnownPosition_));

        window.draw(sprite_);
    }

private:
    sf::IntRect baseRect_{};      ///< The initial texture coordinate region for the wall sprite.
    double animTime_ = 0.0;       ///< Accumulated time tracking destruction animation progress.
    int frame_ = 0;               ///< Index of the current active destruction animation frame.
    double frameDuration_ = 0.12; ///< Duration each destruction frame is displayed.
    bool isAnimating_ = false;    ///< Flag tracking whether the destruction sequence is currently playing.
    bool hasBeenDrawn_ = false;   ///< Flag checking if the wall has rendered at least once.
};

} // namespace representation
