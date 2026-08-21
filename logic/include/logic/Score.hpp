#pragma once

#include <string>
#include <vector>

#include "logic/Observer.hpp"

namespace logic {
    /**
 * @brief Manages player scoring, bonuses, penalties, and high score persistence.
 *
 * Inherits from Observer to automatically award points based on game events
 * (such as destroying blocks, collecting power-ups, or defeating enemies).
 */
    class Score : public Observer {
    public:
        /**
     * @brief Constructs a new Score tracker instance.
     * @param highScoreFile Path to the file used for loading and saving high scores.
     */
        explicit Score(std::string highScoreFile = "highscores.txt");

        /**
     * @brief Callback method handling incoming game events to update the score.
     * @param event The event details used to calculate score adjustments.
     */
        void onNotify(const Event &event) override;

        /**
     * @brief Gets the player's current accumulated score.
     * @return The current score value.
     */
        int getCurrentScore() const { return currentScore_; }

        /**
     * @brief Gets the list of recorded high scores.
     * @return A constant reference to a vector containing high score entries.
     */
        const std::vector<int> &getHighScores() const { return highScores_; }

        /**
     * @brief Loads high scores from the designated persistent storage file.
     */
        void loadHighScores();

        /**
     * @brief Saves the current high scores list to the persistent storage file.
     */
        void saveHighScores() const;

        /**
     * @brief Awards a bonus score for staying alive during gameplay.
     */
        void addAliveBonus();

        /**
     * @brief Finalizes the current score and records it into the high score list if qualified.
     */
        void finalizeAndRecordScore();

    private:
        static constexpr int kAliveTickBonus = 1; ///< Points awarded per alive tick.
        static constexpr int kBlockBrokenBonus = 10; ///< Points awarded for destroying a block/wall.
        static constexpr int kPowerUpBonus = 25; ///< Points awarded for collecting a power-up.
        static constexpr int kEnemyKilledBonus = 100; ///< Points awarded for defeating an enemy.
        static constexpr int kWinBonus = 500; ///< Bonus points awarded for winning the game.
        static constexpr int kLossPenalty = 100; ///< Score deduction applied upon losing the game.
        static constexpr std::size_t kMaxHighScores = 5; ///< Maximum number of high scores retained.

        int currentScore_ = 0; ///< The player's active score in the current session.
        std::vector<int> highScores_; ///< Container holding top recorded scores.
        std::string highScoreFile_; ///< Filepath for saving and loading high scores.
        bool finalized_ = false; ///< Flag indicating whether the score has been finalized.
    };
} // namespace logic
