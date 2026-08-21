#include "logic/Bot.hpp"
#include "logic/World.hpp"
#include "logic/Bomb.hpp"
#include "logic/Wall.hpp"
#include "logic/PowerUp.hpp"
#include "logic/Character.hpp"
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>

namespace logic {

void Bot::decideAction(World& world, double deltaTime) {
    // Get world state and grid sizing
    const auto& entities = world.getEntitiesSafe();
    const int kRows = World::kArenaRows;
    const int kCols = World::kArenaCols;
    const double cellWidth = 2.0 / kCols;
    const double cellHeight = 2.0 / kRows;

    // Convert bot position to grid coordinates
    Vector2 botPos = getPosition();
    int botCol = static_cast<int>((botPos.x + 1.0) / cellWidth);
    int botRow = static_cast<int>((botPos.y + 1.0) / cellHeight);

    // If the bot goes out of bounds, halt movement and return immediately
    if (botRow < 0 || botRow >= kRows || botCol < 0 || botCol >= kCols) {
        world.moveIfPossible(*this, Direction::None, deltaTime);
        return;
    }

    // Build grid of cells threatened by active bombs
    std::vector<std::vector<bool> > dangerGrid(kRows, std::vector<bool>(kCols, false));

    for (const auto& entity : entities) {
        if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            if (!bomb->isAlive())
                continue;
            Vector2 bombPos = bomb->getPosition();
            int bCol = static_cast<int>((bombPos.x + 1.0) / cellWidth);
            int bRow = static_cast<int>((bombPos.y + 1.0) / cellHeight);
            int radius = bomb->getRadius();

            dangerGrid[bRow][bCol] = true;

            // Mark explosion path in four directions until blocked by a wall
            int dr[] = {-1, 1, 0, 0};
            int dc[] = {0, 0, -1, 1};
            for (int i = 0; i < 4; ++i) {
                for (int step = 1; step <= radius; ++step) {
                    int r = bRow + dr[i] * step;
                    int c = bCol + dc[i] * step;
                    if (r < 0 || r >= kRows || c < 0 || c >= kCols)
                        break;

                    bool blocked = false;
                    for (const auto& e : entities) {
                        if (auto wall = std::dynamic_pointer_cast<Wall>(e)) {
                            int wCol = static_cast<int>((wall->getPosition().x + 1.0) / cellWidth);
                            int wRow = static_cast<int>((wall->getPosition().y + 1.0) / cellHeight);
                            if (wRow == r && wCol == c) {
                                blocked = true;
                                break;
                            }
                        }
                    }
                    if (blocked)
                        break;
                    dangerGrid[r][c] = true;
                }
            }
        }
    }

    bool inDanger = dangerGrid[botRow][botCol];

    // Helper to check for impassable walls or solid bombs
    auto isBlocked = [&](int r, int c) {
        for (const auto& e : entities) {
            if (auto wall = std::dynamic_pointer_cast<Wall>(e)) {
                if (!wall->isAlive())
                    continue;
                int wCol = static_cast<int>((wall->getPosition().x + 1.0) / cellWidth);
                int wRow = static_cast<int>((wall->getPosition().y + 1.0) / cellHeight);
                if (wRow == r && wCol == c) {
                    return true;
                }
            }
            if (auto bomb = std::dynamic_pointer_cast<Bomb>(e)) {
                if (!bomb->isAlive())
                    continue;
                if (bomb->canPassThrough(this))
                    continue;
                int bCol = static_cast<int>((bomb->getPosition().x + 1.0) / cellWidth);
                int bRow = static_cast<int>((bomb->getPosition().y + 1.0) / cellHeight);
                if (bRow == r && bCol == c) {
                    return true;
                }
            }
        }
        return false;
    };

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    //Survival
    if (inDanger) {
        struct Node {
            int r, c;
        };
        std::queue<Node> q;
        std::vector<std::vector<bool> > visited(kRows, std::vector<bool>(kCols, false));
        std::vector<std::vector<int> > parentRow(kRows, std::vector<int>(kCols, -1));
        std::vector<std::vector<int> > parentCol(kRows, std::vector<int>(kCols, -1));

        visited[botRow][botCol] = true;
        q.push({botRow, botCol});

        int safeEndRow = -1;
        int safeEndCol = -1;
        bool foundSafe = false;

        // BFS to find closest safe tile
        while (!q.empty()) {
            Node curr = q.front();
            q.pop();

            if (!dangerGrid[curr.r][curr.c]) {
                safeEndRow = curr.r;
                safeEndCol = curr.c;
                foundSafe = true;
                break;
            }

            for (int i = 0; i < 4; ++i) {
                int nr = curr.r + dr[i];
                int nc = curr.c + dc[i];
                if (nr >= 0 && nr < kRows && nc >= 0 && nc < kCols && !visited[nr][nc]) {
                    if (!isBlocked(nr, nc)) {
                        visited[nr][nc] = true;
                        parentRow[nr][nc] = curr.r;
                        parentCol[nr][nc] = curr.c;
                        q.push({nr, nc});
                    }
                }
            }
        }

        // Reconstruct escape path
        if (foundSafe) {
            std::vector<std::pair<int, int> > computedPath;
            int currR = safeEndRow;
            int currC = safeEndCol;
            while (!(currR == botRow && currC == botCol)) {
                computedPath.emplace_back(currR, currC);
                int pr = parentRow[currR][currC];
                int pc = parentCol[currR][currC];
                currR = pr;
                currC = pc;
            }
            std::reverse(computedPath.begin(), computedPath.end());
            path_ = computedPath;
            hasTarget_ = !path_.empty();
        }
    }
    //Power-Up
    else if (!hasTarget_ || path_.empty()) {
        constexpr int kPowerUpMaxRange = 4;
        std::vector<std::pair<int, int> > powerUpCells;
        for (const auto& entity : entities) {
            if (auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
                if (!powerUp->isAlive())
                    continue;
                int puCol = static_cast<int>((powerUp->getPosition().x + 1.0) / cellWidth);
                int puRow = static_cast<int>((powerUp->getPosition().y + 1.0) / cellHeight);
                if (puRow >= 0 && puRow < kRows && puCol >= 0 && puCol < kCols) {
                    if (!dangerGrid[puRow][puCol]) {
                        int dist = std::abs(puRow - botRow) + std::abs(puCol - botCol);
                        if (dist <= kPowerUpMaxRange) {
                            powerUpCells.emplace_back(puRow, puCol);
                        }
                    }
                }
            }
        }

        // BFS to find path to closest power-up
        if (!powerUpCells.empty()) {
            struct Node {
                int r, c;
            };
            std::queue<Node> q;
            bool visited[13][15] = {false};
            int parentRow[13][15];
            int parentCol[13][15];

            visited[botRow][botCol] = true;
            q.push({botRow, botCol});

            int targetPuRow = -1;
            int targetPuCol = -1;
            bool foundPu = false;

            while (!q.empty() && !foundPu) {
                Node curr = q.front();
                q.pop();

                for (const auto& pu : powerUpCells) {
                    if (curr.r == pu.first && curr.c == pu.second) {
                        targetPuRow = curr.r;
                        targetPuCol = curr.c;
                        foundPu = true;
                        break;
                    }
                }

                if (foundPu)
                    break;

                for (int i = 0; i < 4; ++i) {
                    int nr = curr.r + dr[i];
                    int nc = curr.c + dc[i];
                    if (nr >= 0 && nr < kRows && nc >= 0 && nc < kCols && !visited[nr][nc]) {
                        if (!isBlocked(nr, nc) && !dangerGrid[nr][nc]) {
                            visited[nr][nc] = true;
                            parentRow[nr][nc] = curr.r;
                            parentCol[nr][nc] = curr.c;
                            q.push({nr, nc});
                        }
                    }
                }
            }

            // Reconstruct path to power-up
            if (foundPu) {
                std::vector<std::pair<int, int> > computedPath;
                int currR = targetPuRow;
                int currC = targetPuCol;
                while (!(currR == botRow && currC == botCol)) {
                    computedPath.emplace_back(currR, currC);
                    int pr = parentRow[currR][currC];
                    int pc = parentCol[currR][currC];
                    currR = pr;
                    currC = pc;
                }
                std::reverse(computedPath.begin(), computedPath.end());
                path_ = computedPath;
                hasTarget_ = !path_.empty();
            }
        }
    }

    //Wall Bombing & Enemy Trapping
    if (!inDanger && (!hasTarget_ || path_.empty()) && canPlaceBomb()) {
        bool anyDestructibleWalls = false;
        for (const auto& e : entities) {
            if (auto wall = std::dynamic_pointer_cast<Wall>(e)) {
                if (wall->isAlive() && wall->isDestructible()) {
                    anyDestructibleWalls = true;
                    break;
                }
            }
        }

        std::vector<std::pair<int, int> > enemyCells;
        for (const auto& e : entities) {
            if (auto character = std::dynamic_pointer_cast<Character>(e)) {
                if (!character->isAlive() || character.get() == this)
                    continue;
                int cCol = static_cast<int>((character->getPosition().x + 1.0) / cellWidth);
                int cRow = static_cast<int>((character->getPosition().y + 1.0) / cellHeight);
                if (cRow >= 0 && cRow < kRows && cCol >= 0 && cCol < kCols) {
                    enemyCells.emplace_back(cRow, cCol);
                }
            }
        }

        constexpr int kEnemyDetectionRange = 4;
        bool enemyInRange = false;
        for (const auto& ec : enemyCells) {
            int dist = std::abs(ec.first - botRow) + std::abs(ec.second - botCol);
            if (dist <= kEnemyDetectionRange) {
                enemyInRange = true;
                break;
            }
        }

        bool targetWalls = anyDestructibleWalls;
        bool targetEnemies = (!anyDestructibleWalls || enemyInRange) && !enemyCells.empty();

        if (targetWalls || targetEnemies) {
            // Check if already next to a target
            bool adjacentToTarget = false;
            for (int i = 0; i < 4; ++i) {
                int nr = botRow + dr[i];
                int nc = botCol + dc[i];
                if (nr >= 0 && nr < kRows && nc >= 0 && nc < kCols) {
                    if (targetWalls) {
                        for (const auto& e : entities) {
                            if (auto wall = std::dynamic_pointer_cast<Wall>(e)) {
                                if (!wall->isAlive())
                                    continue;
                                int wCol = static_cast<int>((wall->getPosition().x + 1.0) / cellWidth);
                                int wRow = static_cast<int>((wall->getPosition().y + 1.0) / cellHeight);
                                if (wRow == nr && wCol == nc && wall->isDestructible()) {
                                    adjacentToTarget = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (!adjacentToTarget && targetEnemies) {
                        for (const auto& ec : enemyCells) {
                            if (ec.first == nr && ec.second == nc) {
                                adjacentToTarget = true;
                                break;
                            }
                        }
                    }
                }
                if (adjacentToTarget)
                    break;
            }

            // If already next to a target, plant the bomb immediately
            if (adjacentToTarget) {
                world.requestPlaceBomb(this);
                hasTarget_ = false;
                path_.clear();
            } else {
                // Otherwise, pathfind toward a position adjacent to the target
                struct Node {
                    int r, c;
                };
                std::queue<Node> q;
                bool visited[13][15] = {false};
                int parentRow[13][15];
                int parentCol[13][15];

                visited[botRow][botCol] = true;
                q.push({botRow, botCol});

                int targetCellR = -1;
                int targetCellC = -1;
                bool foundTarget = false;

                while (!q.empty() && !foundTarget) {
                    Node curr = q.front();
                    q.pop();

                    for (int i = 0; i < 4; ++i) {
                        int nr = curr.r + dr[i];
                        int nc = curr.c + dc[i];
                        if (nr >= 0 && nr < kRows && nc >= 0 && nc < kCols) {
                            if (targetWalls) {
                                for (const auto& e : entities) {
                                    if (auto wall = std::dynamic_pointer_cast<Wall>(e)) {
                                        if (!wall->isAlive())
                                            continue;
                                        int wCol = static_cast<int>((wall->getPosition().x + 1.0) / cellWidth);
                                        int wRow = static_cast<int>((wall->getPosition().y + 1.0) / cellHeight);
                                        if (wRow == nr && wCol == nc && wall->isDestructible()) {
                                            targetCellR = curr.r;
                                            targetCellC = curr.c;
                                            foundTarget = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (!foundTarget && targetEnemies) {
                                for (const auto& ec : enemyCells) {
                                    if (ec.first == nr && ec.second == nc) {
                                        targetCellR = curr.r;
                                        targetCellC = curr.c;
                                        foundTarget = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (foundTarget)
                            break;
                    }

                    if (foundTarget)
                        break;

                    for (int i = 0; i < 4; ++i) {
                        int nr = curr.r + dr[i];
                        int nc = curr.c + dc[i];
                        if (nr >= 0 && nr < kRows && nc >= 0 && nc < kCols && !visited[nr][nc]) {
                            if (!isBlocked(nr, nc) && !dangerGrid[nr][nc]) {
                                visited[nr][nc] = true;
                                parentRow[nr][nc] = curr.r;
                                parentCol[nr][nc] = curr.c;
                                q.push({nr, nc});
                            }
                        }
                    }
                }

                // Reconstruct path to target approach cell
                if (foundTarget) {
                    std::vector<std::pair<int, int> > computedPath;
                    int currR = targetCellR;
                    int currC = targetCellC;
                    while (!(currR == botRow && currC == botCol)) {
                        computedPath.emplace_back(currR, currC);
                        int pr = parentRow[currR][currC];
                        int pc = parentCol[currR][currC];
                        currR = pr;
                        currC = pc;
                    }
                    std::reverse(computedPath.begin(), computedPath.end());
                    path_ = computedPath;
                    hasTarget_ = !path_.empty();
                }
            }
        }
    }

    //Execution
    if (hasTarget_ && !path_.empty()) {
        auto targetCell = path_.front();
        Vector2 targetCenter = {
            -1.0 + cellWidth * (targetCell.second + 0.5),
            -1.0 + cellHeight * (targetCell.first + 0.5)
        };

        Vector2 actualHitboxCenter = botPos + getHitboxOffset();

        double dx = targetCenter.x - actualHitboxCenter.x;
        double dy = targetCenter.y - actualHitboxCenter.y;

        // Pop tile once close enough to its center
        constexpr double kArrivalThreshold = 0.015;
        if (std::abs(dx) < kArrivalThreshold && std::abs(dy) < kArrivalThreshold) {
            path_.erase(path_.begin());
            if (path_.empty()) {
                hasTarget_ = false;
                world.moveIfPossible(*this, Direction::None, deltaTime);
                return;
            }
            targetCell = path_.front();
            targetCenter = {
                -1.0 + cellWidth * (targetCell.second + 0.5),
                -1.0 + cellHeight * (targetCell.first + 0.5)
            };
            actualHitboxCenter = getPosition() + getHitboxOffset();
            dx = targetCenter.x - actualHitboxCenter.x;
            dy = targetCenter.y - actualHitboxCenter.y;
        }

        // Determine cardinal direction to move
        Direction dir = Direction::None;

        constexpr double kAlignmentTolerance = 0.01;
        if (std::abs(dx) > std::abs(dy)) {
            if (std::abs(dy) > kAlignmentTolerance) {
                dir = (dy > 0) ? Direction::Down : Direction::Up;
            } else {
                dir = (dx > 0) ? Direction::Right : Direction::Left;
            }
        } else {
            if (std::abs(dx) > kAlignmentTolerance) {
                dir = (dx > 0) ? Direction::Right : Direction::Left;
            } else {
                dir = (dy > 0) ? Direction::Down : Direction::Up;
            }
        }

        // Move bot; clear path if stuck
        if (!world.moveIfPossible(*this, dir, deltaTime)) {
            hasTarget_ = false;
            path_.clear();
        }
        return;
    }

    // Idle if no path active
    world.moveIfPossible(*this, Direction::None, deltaTime);
}

} // namespace logic
