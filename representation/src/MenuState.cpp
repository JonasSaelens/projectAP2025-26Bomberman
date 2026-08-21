#include "representation/MenuState.hpp"
#include "representation/Game.hpp"
#include "representation/PlayingState.hpp"

namespace representation {

namespace {
sf::Font& getMenuFont() {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (!font.loadFromFile("assets/Ithaca-LVB75.ttf")) {
            throw std::runtime_error("Failed to load HUD font for MenuState");
        }
        loaded = true;
    }
    return font;
}
} // namespace

MenuState::MenuState(Game& game)
    : State(game) {
    getMenuFont();

    if (backgroundMusic_.openFromFile("assets/menumusic.mp3")) {
        backgroundMusic_.setLoop(true);
        backgroundMusic_.setVolume(40.f);
        backgroundMusic_.play();
    }
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
            game_.startNewGame();

            game_.getStateManager().setState(std::make_shared<PlayingState>(game_));
        } else if (event.key.code == sf::Keyboard::Escape) {
            game_.getWindow().close();
        }
    }
}

void MenuState::update(double /*deltaTime*/) {
}

void MenuState::render(sf::RenderWindow& window) {
    const auto size = window.getSize();

    sf::RectangleShape background(
        sf::Vector2f(static_cast<float>(size.x),
                     static_cast<float>(size.y)));
    background.setFillColor(sf::Color(25, 20, 45));
    window.draw(background);

    constexpr float tileSize = 48.f;

    for (unsigned int y = 0; y < size.y; y += tileSize) {
        for (unsigned int x = 0; x < size.x; x += tileSize) {
            sf::RectangleShape tile(
                sf::Vector2f(tileSize - 2.f, tileSize - 2.f));

            tile.setPosition(
                static_cast<float>(x + 1),
                static_cast<float>(y + 1));

            if (((x / static_cast<unsigned int>(tileSize)) +
                 (y / static_cast<unsigned int>(tileSize))) % 2 == 0) {
                tile.setFillColor(sf::Color(35, 29, 60));
            } else {
                tile.setFillColor(sf::Color(30, 25, 52));
            }

            window.draw(tile);
        }
    }

    sf::RectangleShape borderTop(
        sf::Vector2f(static_cast<float>(size.x), 16.f));
    borderTop.setFillColor(sf::Color(90, 60, 120));
    borderTop.setPosition(0.f, 0.f);
    window.draw(borderTop);

    sf::RectangleShape borderBottom(
        sf::Vector2f(static_cast<float>(size.x), 16.f));
    borderBottom.setFillColor(sf::Color(90, 60, 120));
    borderBottom.setPosition(
        0.f,
        static_cast<float>(size.y) - 16.f);
    window.draw(borderBottom);

    sf::Text titleText_;
    titleText_.setFont(getMenuFont());
    titleText_.setCharacterSize(48);
    titleText_.setFillColor(sf::Color::Yellow);
    titleText_.setOutlineColor(sf::Color::Black);
    titleText_.setOutlineThickness(3.f);
    titleText_.setString("BOMBERMAN");

    const auto titleBounds = titleText_.getLocalBounds();
    titleText_.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    titleText_.setPosition(size.x / 2.0f, 100.f);
    window.draw(titleText_);

    sf::Text playButtonText_;
    playButtonText_.setFont(getMenuFont());
    playButtonText_.setCharacterSize(24);
    playButtonText_.setFillColor(sf::Color::White);
    playButtonText_.setOutlineColor(sf::Color::Black);
    playButtonText_.setOutlineThickness(2.f);
    playButtonText_.setString("Press ENTER or SPACE to Play\nPress ESC to Quit");

    const auto playBounds = playButtonText_.getLocalBounds();
    playButtonText_.setOrigin(playBounds.left + playBounds.width / 2.0f, playBounds.top + playBounds.height / 2.0f);
    playButtonText_.setPosition(size.x / 2.0f, 180.f);
    window.draw(playButtonText_);

    sf::Text scoreTitle("HIGH SCORES", getMenuFont(), 28);
    scoreTitle.setFillColor(sf::Color::Cyan);

    const auto scoreTitleBounds = scoreTitle.getLocalBounds();
    scoreTitle.setOrigin(
        scoreTitleBounds.left + scoreTitleBounds.width / 2.f,
        scoreTitleBounds.top + scoreTitleBounds.height / 2.f);

    scoreTitle.setPosition(
        static_cast<float>(size.x) / 2.f,
        255.f);

    window.draw(scoreTitle);

    const auto& scores = game_.getScore().getHighScores();

    float y = 300.f;
    int rank = 1;

    for (int score : scores) {
        sf::Text line(
            std::to_string(rank) + ". " + std::to_string(score),
            getMenuFont(),
            22);

        line.setFillColor(sf::Color::White);

        const auto bounds = line.getLocalBounds();
        line.setOrigin(
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f);

        line.setPosition(
            static_cast<float>(size.x) / 2.f,
            y);

        window.draw(line);

        y += 30.f;
        ++rank;
    }
}

} // namespace representation
