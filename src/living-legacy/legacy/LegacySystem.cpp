#include "living-legacy/legacy/LegacySystem.h"

namespace living_legacy::legacy {

    void LegacySystem::startNewGame(const std::string& firstCharacterName) {
        currentCharacter_ = firstCharacterName;
        currentGeneration_ = 1;
        legacyLog_.clear();
        gameOver_ = false;
        pendingHeir_.reset();
    }

    void LegacySystem::recordDeath(const std::string& summary) {
        // Log the current character
        legacyLog_.push_back(LegacyEntry{
            currentCharacter_,
            summary,
            currentGeneration_
        });

        if (pendingHeir_) {
            currentCharacter_ = pendingHeir_->name;
            currentGeneration_ = pendingHeir_->generation;
            pendingHeir_.reset();
        } else {
            gameOver_ = true;
        }
    }

    void LegacySystem::assignHeir(const std::string& heirName) {
        pendingHeir_ = Heir{
            heirName,
            currentCharacter_,
            currentGeneration_ + 1
        };
    }

    const std::string& LegacySystem::getCurrentCharacterName() const {
        return currentCharacter_;
    }

    int LegacySystem::getCurrentGeneration() const {
        return currentGeneration_;
    }

    bool LegacySystem::hasHeir() const {
        return pendingHeir_.has_value();
    }

    bool LegacySystem::isGameOver() const {
        return gameOver_;
    }

    const std::vector<LegacyEntry>& LegacySystem::getLegacyLog() const {
        return legacyLog_;
    }

    std::vector<std::string> LegacySystem::generateLegacyRecap() const {
        // Stub — will be expanded later to show world impact
        std::vector<std::string> recap;
        for (const auto& entry : legacyLog_) {
            recap.push_back("Gen " + std::to_string(entry.generation) +
                            ": " + entry.characterName + " — " + entry.summary);
        }
        return recap;
    }

}
