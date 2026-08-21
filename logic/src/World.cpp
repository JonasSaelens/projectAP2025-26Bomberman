#include "logic/World.hpp"

#include <algorithm>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>


#include "logic/Random.hpp"

namespace logic {

namespace {
// Convert grid row/col to continuous world coordinates
Vector2 cellToPosition(int row, int col) {
    const double cellWidth = 2.0 / World::kArenaCols;
    const double cellHeight = 2.0 / World::kArenaRows;
    const double x = -1.0 + cellWidth * (col + 0.5);
    const double y = -1.0 + cellHeight * (row + 0.5);
    return {x, y};
}

// Get standard sizing for arena cells
Vector2 cellSize() {
    return {2.0 / World::kArenaCols, 2.0 / World::kArenaRows};
}
} // namespace

World::World(std::shared_ptr<AbstractFactory> factory)
    : factory_(std::move(factory)) {
}

void World::initializeArena() {
    entities_.clear();
    bots_.clear();
    player_.reset();
    flames_.clear();

    std::ifstream mapFile("assets/map.txt");
    std::vector<std::string> rows;
    std::string line;

    // Load custom map layout from file if available
    if (mapFile.is_open()) {
        while (std::getline(mapFile, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (!line.empty()) {
                rows.push_back(line);
            }
        }
    }

    const bool validSize =
        rows.size() == static_cast<size_t>(kArenaRows) &&
        std::all_of(rows.begin(), rows.end(), [](const std::string& r) {
            return r.size() == static_cast<size_t>(kArenaCols);
        });

    // Populate entities based on custom map file or fallback to procedural generation
    if (validSize) {
        std::vector<std::shared_ptr<Wall> > destructibleWalls;
        for (int row = 0; row < kArenaRows; ++row) {
            for (int col = 0; col < kArenaCols; ++col) {
                const char tile = rows[row][col];
                const Vector2 pos = cellToPosition(row, col);
                const Vector2 size = cellSize();

                switch (tile) {
                case '#':
                    entities_.push_back(factory_->createWall(pos, size, /*destructible=*/false));
                    break;

                case '?': {
                    auto wall = factory_->createWall(pos, size, /*destructible=*/true);
                    entities_.push_back(wall);
                    destructibleWalls.push_back(wall);
                    break;
                }

                case 'P':
                    player_ = factory_->createPlayer(pos, size);
                    entities_.push_back(player_);
                    break;

                case 'B': {
                    auto bot = factory_->createBot(pos, size);
                    bots_.push_back(bot);
                    entities_.push_back(bot);
                    break;
                }

                case '_':
                default:
                    break;
                }
            }
        }
        // Randomly thin out excess destructible walls to keep maps dynamic
        const std::size_t keepTop = static_cast<std::size_t>(Random::getInstance().getInt(70, 75));
        if (destructibleWalls.size() > keepTop) {
            std::vector<std::pair<double, std::shared_ptr<Wall> > > scored;
            scored.reserve(destructibleWalls.size());
            for (auto& w : destructibleWalls) {
                scored.emplace_back(Random::getInstance().getReal(0.0, 1.0), w);
            }

            std::sort(scored.begin(), scored.end(),
                      [](const auto& a, const auto& b) { return a.first > b.first; });
            for (std::size_t i = keepTop; i < scored.size(); ++i) {
                scored[i].second->kill();
            }
        }
    } else {
        // Procedurally generate border walls, pillars, and random destructible blocks
        for (int row = 0; row < kArenaRows; ++row) {
            for (int col = 0; col < kArenaCols; ++col) {
                const bool isBorder =
                    row == 0 || col == 0 || row == kArenaRows - 1 || col == kArenaCols - 1;
                const bool isIndestructiblePillar = row % 2 == 0 && col % 2 == 0;

                const bool isPlayerSpawn = row <= 1 && col <= 1;
                const bool isTopRightSpawn = row <= 1 && col >= kArenaCols - 2;
                const bool isBottomLeftSpawn = row >= kArenaRows - 2 && col <= 1;
                const bool isBottomRightSpawn = row >= kArenaRows - 2 && col >= kArenaCols - 2;
                const bool isSpawnZone =
                    isPlayerSpawn || isTopRightSpawn || isBottomLeftSpawn || isBottomRightSpawn;

                if (isSpawnZone && !isBorder && !isIndestructiblePillar) {
                    continue;
                }

                if (isBorder || isIndestructiblePillar) {
                    entities_.push_back(factory_->createWall(cellToPosition(row, col), cellSize(),
                                                             /*destructible=*/false));
                    continue;
                }

                constexpr double kAirChance = 0.1;
                if (!Random::getInstance().chance(kAirChance)) {
                    entities_.push_back(factory_->createWall(cellToPosition(row, col), cellSize(),
                                                             /*destructible=*/true));
                }
            }
        }

        // Spawn player and bots at designated corners
        player_ = factory_->createPlayer(cellToPosition(1, 1), cellSize());
        entities_.push_back(player_);

        bots_.push_back(factory_->createBot(cellToPosition(1, kArenaCols - 2), cellSize()));
        bots_.push_back(factory_->createBot(cellToPosition(kArenaRows - 2, 1), cellSize()));
        bots_.push_back(factory_->createBot(cellToPosition(kArenaRows - 2, kArenaCols - 2), cellSize()));
        for (const auto& bot : bots_) {
            entities_.push_back(bot);
        }
    }

    if (!player_) {
        player_ = factory_->createPlayer(cellToPosition(1, 1), cellSize());
        entities_.push_back(player_);
    }

    gameOver_ = false;
    playerWon_ = false;
}

void World::update(double deltaTime) {
    if (gameOver_)
        return;

    // Handle player input movement and bomb requests
    if (player_ && player_->isAlive()) {
        if (pendingPlayerDirection_ != Direction::None) {
            Vector2 nextPosition = player_->getPosition();
            const double distance = player_->getSpeed() * deltaTime;

            switch (pendingPlayerDirection_) {
            case Direction::Up:
                nextPosition.y -= distance;
                break;
            case Direction::Down:
                nextPosition.y += distance;
                break;
            case Direction::Left:
                nextPosition.x -= distance;
                break;
            case Direction::Right:
                nextPosition.x += distance;
                break;
            default:
                break;
            }

            if (!wouldCollideWithSolid(*player_, nextPosition)) {
                player_->move(pendingPlayerDirection_, deltaTime);
            } else {
                player_->move(Direction::None, deltaTime);
            }
        } else {
            player_->move(Direction::None, deltaTime);
        }

        if (playerWantsToPlaceBomb_) {
            placeBomb(player_);
        }
    }
    pendingPlayerDirection_ = Direction::None;
    playerWantsToPlaceBomb_ = false;

    // Run bot AI decision-making concurrently using multi-threading
    if (!bots_.empty()) {
        std::vector<std::thread> threads;
        threads.reserve(bots_.size());

        for (const auto& bot : bots_) {
            if (bot && bot->isAlive()) {
                threads.emplace_back([this, bot, deltaTime]() {
                    bot->decideAction(*this, deltaTime);
                });
            }
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    // Update active entities (animations, timers, state changes)
    for (const auto& entity : entities_) {
        if (entity->isAlive()) {
            entity->update(deltaTime);
        }
    }

    // Check character-to-powerup collisions and apply collected power-ups
    for (const auto& entity : entities_) {
        auto character = std::dynamic_pointer_cast<Character>(entity);
        if (!character || !character->isAlive())
            continue;

        for (const auto& other : entities_) {
            auto powerUp = std::dynamic_pointer_cast<PowerUp>(other);
            if (!powerUp || !powerUp->isAlive())
                continue;

            if (character->intersects(*powerUp)) {
                powerUp->applyTo(*character);
                powerUp->collect(character->isPlayer());
            }
        }
    }

    // Manage bomb pass-through behavior when characters overlap unexploded bombs
    for (const auto& entity : entities_) {
        auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
        if (!bomb || !bomb->isAlive()) {
            continue;
        }

        for (const auto& ent : entities_) {
            auto character = std::dynamic_pointer_cast<Character>(ent);
            if (!character || !character->isAlive())
                continue;

            if (character->intersects(*bomb)) {
                bomb->addPassThroughCharacter(character.get());
            } else {
                bomb->removePassThroughCharacter(character.get());
            }
        }
    }

    // Trigger explosions for bombs whose timers have run out
    auto entitiesCopy = entities_;
    for (const auto& entity : entitiesCopy) {
        auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
        if (bomb && bomb->hasExploded()) {
            resolveExplosion(*bomb);
        }
    }

    // Update and cleanup active explosion flames
    for (const auto& flame : flames_) {
        if (flame->isAlive()) {
            flame->update(deltaTime);
        }
    }
    flames_.erase(std::remove_if(flames_.begin(), flames_.end(),
                                 [](const std::shared_ptr<EntityModel>& flame) {
                                     return !flame->isAlive();
                                 }), flames_.end());

    removeDeadEntities();

    // Check win/loss game state conditions
    const bool playerAlive = player_ && player_->isAlive();
    const bool anyBotsAlive = std::any_of(bots_.begin(), bots_.end(),
                                          [](const std::shared_ptr<Bot>& bot) {
                                              return bot && bot->isAlive();
                                          });

    if (!playerAlive && !gameOver_) {
        gameOver_ = true;
        playerWon_ = false;
        if (player_) {
            player_->emit(Event{EventType::Lost, player_->getPosition(), Direction::Down, true, true});
        }
    } else if (playerAlive && !anyBotsAlive && !gameOver_) {
        gameOver_ = true;
        playerWon_ = true;
        if (player_) {
            player_->emit(Event{EventType::Won, player_->getPosition(), Direction::Down, true, true});
        }
    }

}

void World::placeBomb(const std::shared_ptr<Character>& owner) {
    if (!owner || !owner->canPlaceBomb())
        return;
    const double cellWidth = 2.0 / World::kArenaCols;
    const double cellHeight = 2.0 / World::kArenaRows;

    int col = static_cast<int>((owner->getPosition().x + 1.0) / cellWidth);
    int row = static_cast<int>((owner->getPosition().y + 1.0) / cellHeight);

    col = std::max(0, std::min(col, World::kArenaCols - 1));
    row = std::max(0, std::min(row, World::kArenaRows - 1));

    Vector2 bombPosition{
        -1.0 + cellWidth * (col + 0.5),
        -1.0 + cellHeight * (row + 0.5)
    };

    auto bomb = factory_->createBomb(bombPosition, cellSize(), owner->getBombRadius(), owner);

    // Allow owner and overlapping characters to initially pass through the placed bomb
    bomb->addPassThroughCharacter(owner.get());
    for (const auto& entity : entities_) {
        if (auto character = std::dynamic_pointer_cast<Character>(entity)) {
            if (character->isAlive() && character->intersects(*bomb)) {
                bomb->addPassThroughCharacter(character.get());
            }
        }
    }

    owner->onBombPlaced();
    entities_.push_back(bomb);
}

bool World::moveIfPossible(Character& character, Direction direction, double deltaTime) {
    std::lock_guard<std::mutex> lock(g_worldMutex);

    if (direction == Direction::None) {
        character.move(Direction::None, deltaTime);
        return true;
    }

    Vector2 nextPosition = character.getPosition();
    const double distance = character.getSpeed() * deltaTime;
    switch (direction) {
    case Direction::Up:
        nextPosition.y -= distance;
        break;
    case Direction::Down:
        nextPosition.y += distance;
        break;
    case Direction::Left:
        nextPosition.x -= distance;
        break;
    case Direction::Right:
        nextPosition.x += distance;
        break;
    default:
        break;
    }

    if (wouldCollideWithSolid(character, nextPosition)) {
        character.move(Direction::None, deltaTime);
        return false;
    }

    character.move(direction, deltaTime);
    return true;
}

void World::requestPlaceBomb(Character* owner) {
    std::lock_guard<std::mutex> lock(g_worldMutex);
    if (!owner)
        return;
    for (const auto& entity : entities_) {
        auto character = std::dynamic_pointer_cast<Character>(entity);
        if (character && character.get() == owner) {
            placeBomb(character);
            break;
        }
    }
}

void World::resolveExplosion(Bomb& bomb) {
    const Vector2 bombPos = bomb.getPosition();
    const int radius = bomb.getRadius();
    const double cellWidth = 2.0 / World::kArenaCols;
    const double cellHeight = 2.0 / World::kArenaRows;

    auto owner = bomb.getOwner().lock();
    if (owner) {
        owner->onBombExploded();
    }

    flames_.push_back(factory_->createFlame(bombPos, cellSize(), FlamePart::Center));

    const int centerCol = static_cast<int>((bombPos.x + 1.0) / cellWidth);
    const int centerRow = static_cast<int>((bombPos.y + 1.0) / cellHeight);

    auto spawnFlameAt = [this](int row, int col, FlamePart part) {
        Vector2 cellPos = cellToPosition(row, col);
        auto flame = factory_->createFlame(cellPos, cellSize(), part);
        flames_.push_back(flame);
    };

    std::vector<std::shared_ptr<EntityModel> > spawnedPowerUps;

    // Helper lambda to propagate explosion flames and damage objects along cardinal directions
    auto explodeDirection = [this, &bomb, radius, bombPos, cellWidth, cellHeight, &spawnFlameAt, &spawnedPowerUps]
    (int rowDelta, int colDelta, FlamePart middlePart, FlamePart endPart) {

        int bombCol = static_cast<int>((bombPos.x + 1.0) / cellWidth);
        int bombRow = static_cast<int>((bombPos.y + 1.0) / cellHeight);

        for (int step = 1; step <= radius; ++step) {
            int cellRow = bombRow + step * rowDelta;
            int cellCol = bombCol + step * colDelta;

            if (cellRow < 0 || cellRow >= kArenaRows || cellCol < 0 || cellCol >= kArenaCols) {
                break;
            }

            Vector2 cellPos = cellToPosition(cellRow, cellCol);

            bool hitBlocker = false;
            bool isLastStep = (step == radius);

            for (const auto& entity : entities_) {
                if (!entity->isAlive())
                    continue;

                int entityCol = static_cast<int>((entity->getPosition().x + 1.0) / cellWidth);
                int entityRow = static_cast<int>((entity->getPosition().y + 1.0) / cellHeight);

                if (entityRow != cellRow || entityCol != cellCol)
                    continue;

                // Handle interactions with walls, characters, chain-reacting bombs, and power-ups
                if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
                    if (wall->isDestructible()) {
                        bool ownerIsPlayer = false;
                        if (auto owner = bomb.getOwner().lock()) {
                            ownerIsPlayer = owner->isPlayer();
                        }

                        wall->destroy(ownerIsPlayer);
                        if (Random::getInstance().chance(0.3)) {
                            int powerUpType = Random::getInstance().getInt(0, 2);
                            std::shared_ptr<PowerUp> powerUp;
                            switch (powerUpType) {
                            case 0:
                                powerUp = factory_->createPowerUp(cellPos, cellSize(), PowerUpType::Fire);
                                break;
                            case 1:
                                powerUp = factory_->createPowerUp(cellPos, cellSize(), PowerUpType::ExtraBomb);
                                break;
                            case 2:
                                powerUp = factory_->createPowerUp(cellPos, cellSize(), PowerUpType::Skates);
                                break;
                            }
                            if (powerUp) {
                                spawnedPowerUps.push_back(powerUp);
                            }
                        }
                    }
                    hitBlocker = true;
                } else if (auto character = std::dynamic_pointer_cast<Character>(entity)) {
                    auto owner = bomb.getOwner().lock();
                    if (owner && owner->isPlayer()) {
                        character->setKilledByPlayer();
                    }
                    character->kill();
                } else if (auto otherBomb = std::dynamic_pointer_cast<Bomb>(entity)) {
                    if (otherBomb.get() != &bomb) {
                        otherBomb->explode();
                    }
                } else if (auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
                    powerUp->kill();
                }
            }

            if (!hitBlocker) {
                spawnFlameAt(cellRow, cellCol, isLastStep ? endPart : middlePart);
            }

            if (hitBlocker)
                break;
        }
    };

    flames_.push_back(factory_->createFlame(bombPos, cellSize(), FlamePart::Center));

    // Fire explosion rays in all four directions plus center
    explodeDirection(0, 0, FlamePart::Center, FlamePart::Center);
    explodeDirection(-1, 0, FlamePart::Vertical, FlamePart::EndUp);
    explodeDirection(1, 0, FlamePart::Vertical, FlamePart::EndDown);
    explodeDirection(0, -1, FlamePart::Horizontal, FlamePart::EndLeft);
    explodeDirection(0, 1, FlamePart::Horizontal, FlamePart::EndRight);

    if (!spawnedPowerUps.empty()) {
        entities_.insert(entities_.end(), spawnedPowerUps.begin(), spawnedPowerUps.end());
    }
}

void World::removeDeadEntities() {
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                   [](const std::shared_ptr<EntityModel>& entity) {
                                       return !entity->isAlive();
                                   }),
                    entities_.end());
    bots_.erase(std::remove_if(bots_.begin(), bots_.end(),
                               [](const std::shared_ptr<Bot>& bot) { return !bot->isAlive(); }),
                bots_.end());
}

bool World::wouldCollideWithSolid(const EntityModel& mover, Vector2 nextPosition) const {
    const Vector2 moverSize = mover.getSize();
    const Vector2 moverOffset = mover.getHitboxOffset();

    // Helper lambda to test AABB bounding box collision between entities
    auto intersect = [&](const std::shared_ptr<EntityModel>& other) {
        const Vector2 otherP = other->getPosition();
        const Vector2 otherS = other->getSize();
        const Vector2 otherOffset = other->getHitboxOffset();

        const double aLeft = (nextPosition.x + moverOffset.x) - moverSize.x / 2.0;
        const double aRight = (nextPosition.x + moverOffset.x) + moverSize.x / 2.0;
        const double aTop = (nextPosition.y + moverOffset.y) - moverSize.y / 2.0;
        const double aBottom = (nextPosition.y + moverOffset.y) + moverSize.y / 2.0;

        const double bLeft = (otherP.x + otherOffset.x) - otherS.x / 2.0;
        const double bRight = (otherP.x + otherOffset.x) + otherS.x / 2.0;
        const double bTop = (otherP.y + otherOffset.y) - otherS.y / 2.0;
        const double bBottom = (otherP.y + otherOffset.y) + otherS.y / 2.0;

        return aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop;
    };

    for (const auto& entity : entities_) {
        if (!entity->isAlive())
            continue;
        if (std::dynamic_pointer_cast<Wall>(entity)) {
            if (intersect(entity)) {
                return true;
            }
        }
        if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            if (intersect(entity)) {
                if (auto character = dynamic_cast<const Character*>(&mover)) {
                    if (bomb->canPassThrough(character)) {
                        continue;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

} // namespace logic
