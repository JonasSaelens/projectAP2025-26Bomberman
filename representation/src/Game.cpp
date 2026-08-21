#include "representation/Game.hpp"

#include "logic/Stopwatch.hpp"
#include "representation/MenuState.hpp"
#include "representation/PlayingState.hpp"

namespace representation {

Game::Game(unsigned windowWidth, unsigned windowHeight, const std::string& title)
    : window_(std::make_shared<sf::RenderWindow>(sf::VideoMode(windowWidth, windowHeight),
                                                 title)),
      camera_(windowWidth, windowHeight),
      factory_(std::make_shared<ConcreteFactory>(window_)),
      world_(std::make_unique<logic::World>(factory_)),
      score_(std::make_shared<logic::Score>()) {
    window_->setFramerateLimit(60);
    stateManager_.setState(std::make_shared<MenuState>(*this));
}

void Game::startNewGame() {
    score_ = std::make_shared<logic::Score>();
    factory_ = std::make_shared<ConcreteFactory>(window_);
    world_ = std::make_unique<logic::World>(factory_);
    world_->initializeArena();

    for (const auto& e : world_->getEntities()) {
        e->attach(score_);
    }

    if (auto player = world_->getPlayer()) {
        player->attach(score_);
    }
}

void Game::processInput() {
    sf::Event event{};
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_->close();
        }
        if (auto* state = stateManager_.getCurrentState()) {
            state->handleEvent(event);
        }
    }
}

void Game::update(double deltaTime) {
    for (const auto& e : world_->getEntities()) {
        if (!e->hasObserver(score_)) {
            e->attach(score_);
        }
    }
    if (auto* state = stateManager_.getCurrentState()) {
        state->update(deltaTime);
    }
}

void Game::render() {
    window_->clear();
    if (auto* state = stateManager_.getCurrentState()) {
        state->render(*window_);
    }
    window_->display();
}

void Game::run() {
    while (window_->isOpen()) {
        logic::Stopwatch::getInstance().tick();
        const double deltaTime = logic::Stopwatch::getInstance().getDeltaTime();

        processInput();
        update(deltaTime);
        render();
    }
}

} // namespace representation
