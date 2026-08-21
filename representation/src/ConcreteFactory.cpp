#include "representation/ConcreteFactory.hpp"

#include <stdexcept>

#include "representation/BombView.hpp"
#include "representation/CharacterView.hpp"
#include "representation/PowerUpView.hpp"
#include "representation/WallView.hpp"
#include "representation/FlameView.hpp"
#include "logic/Event.hpp"

namespace representation {

ConcreteFactory::ConcreteFactory(std::shared_ptr<sf::RenderWindow> window)
    : window_(std::move(window)) {
}

// Load and cache texture assets to avoid duplicate loading
std::shared_ptr<sf::Texture> ConcreteFactory::loadTexture(const std::string& path) {
    auto cached = textureCache_.find(path);
    if (cached != textureCache_.end()) {
        return cached->second;
    }
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    textureCache_[path] = texture;
    return texture;
}

// Create the playable character with custom hitbox and view
std::shared_ptr<logic::Character> ConcreteFactory::createPlayer(logic::Vector2 position,
                                                                logic::Vector2 size) {
    double headCut = size.y * 0.33;
    logic::Vector2 hitboxSize{size.x * 0.6, size.y - headCut};
    logic::Vector2 hitboxOffset{0.0, headCut * 0.5};
    auto model = std::make_shared<logic::Character>(position, hitboxSize, hitboxOffset, true);

    auto texture = loadTexture("assets/Bomberman.png");

    auto view = std::make_shared<CharacterView>(texture, size);
    view->setLayer(2);

    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

// Create an AI bot character with distinct sprite sheet mapping and hitbox
std::shared_ptr<logic::Bot> ConcreteFactory::createBot(logic::Vector2 position,
                                                       logic::Vector2 size) {
    int selectedColumn = botColumns_[0];
    int selectedRow = botRows_[0];
    for (size_t i = 0; i < botColumns_.size(); ++i) {
        if (!botUsed_[i]) {
            selectedColumn = botColumns_[i];
            selectedRow = botRows_[i];
            botUsed_[i] = true;
            break;
        }
    }
    double headCut = size.y * 0.33;
    logic::Vector2 hitboxSize{size.x * 0.6, size.y - headCut};
    logic::Vector2 hitboxOffset{0.0, headCut * 0.5};
    auto model = std::make_shared<logic::Bot>(position, hitboxSize, hitboxOffset);
    auto texture = loadTexture("assets/Bomberman.png");

    auto view = std::make_shared<CharacterView>(texture, size, selectedColumn, selectedRow);
    view->setLayer(2);

    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

// Create a bomb entity linked to its owner and tile sprite graphic
std::shared_ptr<logic::Bomb> ConcreteFactory::createBomb(
    logic::Vector2 position, logic::Vector2 size, int radius,
    std::weak_ptr<logic::Character> owner) {
    auto model = std::make_shared<logic::Bomb>(position, size, radius, std::move(owner));

    auto texture = loadTexture("assets/BombermanTiles.png");
    const int tileW = 16;
    const int tileH = 16;
    const int bombColumn = 0;
    const int bombRow = 3;

    sf::IntRect bombRect(bombColumn * tileW, bombRow * tileH, tileW, tileH);
    auto view = std::make_shared<BombView>(texture, bombRect, size);
    view->setLayer(1);

    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

// Create animated explosion flame views based on orientation part
std::shared_ptr<logic::Flame> ConcreteFactory::createFlame(logic::Vector2 position,
                                                           logic::Vector2 size,
                                                           logic::FlamePart part) {
    const double lifeTime = 0.50;
    auto model = std::make_shared<logic::Flame>(position, size, lifeTime, part);

    auto texture = loadTexture("assets/BombermanTiles.png");
    const int tileW = 16;
    const int tileH = 16;

    auto rectFromTile = [&](int col, int row) {
        return sf::IntRect(col * tileW, row * tileH, tileW, tileH);
    };

    std::vector<sf::IntRect> frames;

    if (part == logic::FlamePart::Center) {
        frames.push_back(rectFromTile(2, 6));
        frames.push_back(rectFromTile(7, 6));
        frames.push_back(rectFromTile(2, 11));
        frames.push_back(rectFromTile(7, 11));
    } else {
        int baseCol = 0;
        int baseRow = 0;
        switch (part) {
        case logic::FlamePart::Vertical:
            baseCol = 2;
            baseRow = 7;
            break;
        case logic::FlamePart::Horizontal:
            baseCol = 3;
            baseRow = 6;
            break;
        case logic::FlamePart::EndUp:
            baseCol = 2;
            baseRow = 4;
            break;
        case logic::FlamePart::EndDown:
            baseCol = 2;
            baseRow = 8;
            break;
        case logic::FlamePart::EndLeft:
            baseCol = 0;
            baseRow = 6;
            break;
        case logic::FlamePart::EndRight:
            baseCol = 4;
            baseRow = 6;
            break;
        default:
            baseCol = 2;
            baseRow = 6;
            break;
        }

        const int dx[4] = {0, 5, 0, 5};
        const int dy[4] = {0, 0, 5, 5};
        for (int i = 0; i < 4; ++i) {
            frames.push_back(rectFromTile(baseCol + dx[i], baseRow + dy[i]));
        }
    }

    auto view = std::make_shared<FlameView>(texture, frames, size);
    view->setLayer(1);

    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

// Create destructible or solid wall entities
std::shared_ptr<logic::Wall> ConcreteFactory::createWall(logic::Vector2 position,
                                                         logic::Vector2 size,
                                                         bool destructible) {
    auto model = std::make_shared<logic::Wall>(position, size, destructible);
    auto texture = loadTexture("assets/BombermanTiles.png");

    const int tileW = 16;
    const int tileH = 16;

    sf::IntRect tileRect;
    if (destructible) {
        tileRect = sf::IntRect(4 * tileW, 3 * tileH, tileW, tileH);
    } else {
        tileRect = sf::IntRect(3 * tileW, 3 * tileH, tileW, tileH);
    }

    auto view = std::make_shared<WallView>(texture, tileRect, size);
    view->setLayer(0);
    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

// Create power-up items based on type (fire, bomb, skates)
std::shared_ptr<logic::PowerUp> ConcreteFactory::createPowerUp(logic::Vector2 position,
                                                               logic::Vector2 size,
                                                               logic::PowerUpType type) {
    std::shared_ptr<logic::PowerUp> model;
    auto texture = loadTexture("assets/Bomberman.png");

    const int tileW = 32;
    const int tileH = 32;

    sf::IntRect tileRect;
    switch (type) {
    case logic::PowerUpType::Fire:
        model = std::make_shared<logic::FirePowerUp>(position, size);
        tileRect = sf::IntRect(0 * tileW, 22 * tileH, tileW, tileH);
        break;
    case logic::PowerUpType::ExtraBomb:
        model = std::make_shared<logic::BombPowerUp>(position, size);
        tileRect = sf::IntRect(1 * tileW, 22 * tileH, tileW, tileH);
        break;
    case logic::PowerUpType::Skates:
        model = std::make_shared<logic::SkatesPowerUp>(position, size);
        tileRect = sf::IntRect(2 * tileW, 22 * tileH, tileW, tileH);
        break;
    }

    auto view = std::make_shared<PowerUpView>(texture, tileRect, size, type);
    view->setLayer(1);
    model->attach(view);
    view->onNotify(logic::Event{logic::EventType::Spawned, model->getPosition()});
    views_.push_back(view);
    return model;
}

} // namespace representation
