#include "logic/Score.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace logic {

Score::Score(std::string highScoreFile)
    : highScoreFile_(std::move(highScoreFile)) {
    loadHighScores();
}

void Score::onNotify(const Event& event) {
    if (event.fromPlayer && !finalized_) {
        switch (event.type) {
        case EventType::BlockDestroyed:
            currentScore_ += kBlockBrokenBonus;
            break;
        case EventType::PowerUpCollected:
            currentScore_ += kPowerUpBonus;
            break;
        case EventType::Died:
            if (!event.isPlayer && event.killedByPlayer) {
                currentScore_ += kEnemyKilledBonus;
            }
            break;
        case EventType::Won:
            currentScore_ += kWinBonus;
            finalizeAndRecordScore();
            break;
        case EventType::Lost:
            currentScore_ -= kLossPenalty;
            finalizeAndRecordScore();
            break;
        default:
            break;
        }
    }
}

void Score::addAliveBonus() {
    if (!finalized_) {
        currentScore_ += kAliveTickBonus;
    }
}


void Score::loadHighScores() {
    highScores_.clear();
    std::ifstream file(highScoreFile_);
    if (!file) {
        return;
    }
    int value = 0;
    while (file >> value) {
        highScores_.push_back(value);
    }
    std::sort(highScores_.begin(), highScores_.end(), std::greater<>());
    if (highScores_.size() > kMaxHighScores) {
        highScores_.resize(kMaxHighScores);
    }
}

void Score::saveHighScores() const {
    std::ofstream file(highScoreFile_, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Could not open high score file for writing: " +
                                 highScoreFile_);
    }
    for (int value : highScores_) {
        file << value << '\n';
    }
}

void Score::finalizeAndRecordScore() {
    if (finalized_) {
        return;
    }
    finalized_ = true;

    highScores_.push_back(currentScore_);
    std::sort(highScores_.begin(), highScores_.end(), std::greater<>());
    if (highScores_.size() > kMaxHighScores) {
        highScores_.resize(kMaxHighScores);
    }
    saveHighScores();
}

} // namespace logic
