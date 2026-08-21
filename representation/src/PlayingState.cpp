#include "representation/PlayingState.hpp"

#include "representation/Game.hpp"
#include "representation/CharacterView.hpp"
#include "representation/BombView.hpp"
#include "representation/FlameView.hpp"
#include "representation/WallView.hpp"

#include <algorithm>

#include "representation/GameOverState.hpp"

namespace representation {

PlayingState::PlayingState(Game& game)
    : State(game) {
    backgroundReady_ = backgroundTexture_.loadFromFile("assets/BombermanTiles.png");

    if (backgroundReady_) {
        backgroundTile_.setTexture(backgroundTexture_);

        const int tileW = 16;
        const int tileH = 16;
        backgroundTile_.setTextureRect(sf::IntRect(0, 4 * tileH, tileW, tileH));
    }

    if (!font_.loadFromFile("assets/Ithaca-LVB75.ttf")) {
        throw std::runtime_error("Failed to load HUD font");
    }

    scoreText_.setFont(font_);
    scoreText_.setCharacterSize(24);
    scoreText_.setFillColor(sf::Color::White);
    scoreText_.setOutlineColor(sf::Color::Black);
    scoreText_.setOutlineThickness(2.f);
    scoreText_.setPosition(20.f, 10.f);

    if (backgroundMusic_.openFromFile("assets/playingmusic.mp3")) {
        backgroundMusic_.setLoop(true);
        backgroundMusic_.setVolume(40.f);
        backgroundMusic_.play();
    }
}

void PlayingState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            game_.getWorld().requestPlayerPlaceBomb();
        }
    }
    (void)event;
}

void PlayingState::update(double deltaTime) {
    logic::Direction playerDirection = logic::Direction::None;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        playerDirection = logic::Direction::Up;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        playerDirection = logic::Direction::Down;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        playerDirection = logic::Direction::Left;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        playerDirection = logic::Direction::Right;
    } else {
        playerDirection = logic::Direction::None;
    }
    game_.getWorld().setPlayerDirection(playerDirection);
    game_.getWorld().update(deltaTime);

    if (game_.getWorld().isGameOver()) {
        bool hasWon = game_.getWorld().has_player_won();
        int finalScore = game_.getScore().getCurrentScore();
        game_.getStateManager().setState(std::make_shared<GameOverState>(game_, hasWon, finalScore));
        return;
    }

    aliveScoreAccumulator_ += static_cast<float>(deltaTime);
    while (aliveScoreAccumulator_ >= 1.0f) {
        game_.getScore().addAliveBonus();
        aliveScoreAccumulator_ -= 1.0f;
    }

    for (const auto& view : game_.getFactory()->getViews()) {
        if (auto characterView = std::dynamic_pointer_cast<CharacterView>(view)) {
            characterView->updateAnimation(deltaTime);
        }
        if (auto bombView = std::dynamic_pointer_cast<BombView>(view)) {
            bombView->updateAnimation(deltaTime);
        }
        if (auto flameView = std::dynamic_pointer_cast<FlameView>(view)) {
            flameView->updateAnimation(deltaTime);
        }
        if (auto wallView = std::dynamic_pointer_cast<WallView>(view)) {
            wallView->updateAnimation(deltaTime);
        }
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    if (backgroundReady_) {
        const auto size = window.getSize();

        const float tileDrawW = static_cast<float>(size.x) / 15.0f;
        const float tileDrawH = static_cast<float>(size.y) / 13.0f;

        const auto rect = backgroundTile_.getTextureRect();
        const float sx = tileDrawW / static_cast<float>(rect.width);
        const float sy = tileDrawH / static_cast<float>(rect.height);

        backgroundTile_.setScale(sx, sy);

        for (unsigned y = 0; y < size.y; y += static_cast<unsigned>(tileDrawH)) {
            for (unsigned x = 0; x < size.x; x += static_cast<unsigned>(tileDrawW)) {
                backgroundTile_.setPosition(static_cast<float>(x), static_cast<float>(y));
                window.draw(backgroundTile_);
            }
        }
    }

    auto views = game_.getFactory()->getViews();
    auto& camera = game_.getCamera();

    std::vector<std::shared_ptr<representation::EntityView> > sortedViews(views.begin(), views.end());
    std::sort(sortedViews.begin(), sortedViews.end(),
              [](const auto& a, const auto& b) {
                  return a->getLayer() < b->getLayer();
              });

    for (const auto& view : sortedViews) {
        view->draw(window, camera);
    }

    scoreText_.setString("Score: " + std::to_string(game_.getScore().getCurrentScore()));
    window.draw(scoreText_);
}

} // namespace representation
