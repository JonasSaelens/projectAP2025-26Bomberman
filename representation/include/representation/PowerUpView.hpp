#pragma once

#include "representation/EntityView.hpp"
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <memory>

namespace representation {
/**
 * @brief Handles the visual presentation and collection sound effects for a PowerUp entity.
 *
 * Inherits from EntityView to render power-up sprites and manage sound buffers
 * and active sound playback upon collection via observer notifications.
 */
class PowerUpView : public EntityView {
public:
    /**
     * @brief Constructs a new PowerUpView instance.
     *
     * @param texture Shared pointer to the SFML texture atlas.
     * @param initialRect The texture source rectangle for the specific power-up sprite.
     * @param size The world-space dimensions of the power-up.
     * @param powerUpType The category of power-up determining its visual and audio assets.
     */
    PowerUpView(std::shared_ptr<sf::Texture> texture,
                const sf::IntRect& initialRect,
                logic::Vector2 size,
                logic::PowerUpType powerUpType)
        : EntityView(std::move(texture)),
          powerUpType_(powerUpType) {
        sprite_.setTextureRect(initialRect);
        setSize(size);

        std::string soundPath = getSoundPathForType(powerUpType);
        if (!soundPath.empty()) {
            soundBuffer_.loadFromFile(soundPath);
        }
    }

    /**
     * @brief Handles incoming game events to trigger audio playback when collected.
     * @param event The event details dispatched by the subject.
     */
    void onNotify(const logic::Event& event) override {
        EntityView::onNotify(event);

        switch (event.type) {
        case logic::EventType::PowerUpCollected:
            playSoundSafely();
            break;

        default:
            break;
        }
    }

private:
    /**
     * @brief Determines the appropriate sound file path based on the power-up category.
     * @param type The PowerUpType enumeration value.
     * @return A string containing the asset file path.
     */
    std::string getSoundPathForType(logic::PowerUpType type) {
        switch (type) {
        case logic::PowerUpType::Skates:
            return "assets/powerup_speed.wav";
        case logic::PowerUpType::ExtraBomb:
            return "assets/powerup_bomb.wav";
        case logic::PowerUpType::Fire:
            return "assets/powerup_fire.wav";
        default:
            return "assets/powerup_default.wav";
        }
    }

    /**
     * @brief Plays the collection sound safely by managing a static pool of active sounds.
     */
    void playSoundSafely() {
        if (soundBuffer_.getSampleCount() > 0) {
            auto& activeSounds = getActiveSounds();

            activeSounds.erase(
                std::remove_if(activeSounds.begin(), activeSounds.end(), [](const sf::Sound& sound) {
                    return sound.getStatus() == sf::Sound::Stopped;
                }),
                activeSounds.end()
                );

            auto& sound = activeSounds.emplace_back();
            sound.setBuffer(soundBuffer_);
            sound.play();
        }
    }

    /**
     * @brief Retrieves a static container managing currently playing sound instances to prevent premature destruction.
     * @return Reference to a vector of active sf::Sound instances.
     */
    static std::vector<sf::Sound>& getActiveSounds() {
        static std::vector<sf::Sound> sounds;
        return sounds;
    }

    logic::PowerUpType powerUpType_; ///< The category type of the power-up.
    sf::SoundBuffer soundBuffer_;    ///< Audio buffer holding the collection sound data.
};
} // namespace representation
