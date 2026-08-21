#include "representation/GameOverState.hpp"
#include "representation/Game.hpp"
#include "representation/PlayingState.hpp"

namespace representation {

GameOverState::GameOverState(Game& game, bool hasWon, int finalScore)
    : State(game), hasWon_(hasWon), finalScore_(finalScore) {

    if (!font_.loadFromFile("assets/Ithaca-LVB75.ttf")) {
        throw std::runtime_error("Failed to load HUD font for GameOverState");
    }

    titleText_.setFont(font_);
    titleText_.setCharacterSize(48);
    titleText_.setFillColor(hasWon_ ? sf::Color::Green : sf::Color::Red);
    titleText_.setOutlineColor(sf::Color::Black);
    titleText_.setOutlineThickness(3.f);
    titleText_.setString(hasWon_ ? "VICTORY!" : "GAME OVER");

    scoreText_.setFont(font_);
    scoreText_.setCharacterSize(24);
    scoreText_.setFillColor(sf::Color::White);
    scoreText_.setOutlineColor(sf::Color::Black);
    scoreText_.setOutlineThickness(2.f);
    scoreText_.setString("Final Score: " + std::to_string(finalScore_));

    instructionText_.setFont(font_);
    instructionText_.setCharacterSize(20);
    instructionText_.setFillColor(sf::Color::Yellow);
    instructionText_.setOutlineColor(sf::Color::Black);
    instructionText_.setOutlineThickness(2.f);
    instructionText_.setString("Press ENTER or SPACE to Play Again\nPress ESC to Quit");

    if (soundBuffer_.loadFromFile("assets/gameovermusic.mp3")) {
        endSound_.setBuffer(soundBuffer_);
    }
    endSound_.play();
}

void GameOverState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter)) {
        game_.startNewGame();
        game_.getStateManager().setState(std::make_shared<PlayingState>(game_));
    } else if (event.key.code == sf::Keyboard::Escape) {
        game_.getWindow().close();
    }
}

void GameOverState::update(double /*deltaTime*/) {
}

void GameOverState::render(sf::RenderWindow& window) {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    const auto windowSize = window.getSize();

    auto titleBounds = titleText_.getLocalBounds();
    titleText_.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    titleText_.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 90.f);

    auto scoreBounds = scoreText_.getLocalBounds();
    scoreText_.setOrigin(scoreBounds.left + scoreBounds.width / 2.0f, scoreBounds.top + scoreBounds.height / 2.0f);
    scoreText_.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 20.f);

    auto instBounds = instructionText_.getLocalBounds();
    instructionText_.setOrigin(instBounds.left + instBounds.width / 2.0f, instBounds.top + instBounds.height / 2.0f);
    instructionText_.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + 50.f);

    window.draw(titleText_);
    window.draw(scoreText_);
    window.draw(instructionText_);
}

} // namespace representation
