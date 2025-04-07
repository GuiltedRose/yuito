#pragma once

#include <string>
#include <vector>
#include <optional>

namespace living_legacy::legacy {
    struct LegacyEntry {
        std::string characterName;
        std::string summary;
        int generation = 0;
    };

    struct Heir {
        std::string name;
        std::optional<std::string> parentName;
        int generation;
    };

    class LegacySystem {
    public:
        void startNewGame(const std::string& firstCharacterName);
        void recordDeath(const std::string& summary);
        void assignHeir(const std::string& heirName);

        const std::string& getCurrentCharacterName() const;

        const std::vector<LegacyEntry>& getLegacyLog() const;
        std::vector<std::string> generateLegacyRecap() const;
        
        int getCurrentGeneration() const;
        bool hasHeir() const;
        bool isGameOver() const;
    private:
        std::string currentCharacter_;
        std::optional<Heir> pendingHeir_;
        std::vector<LegacyEntry> legacyLog_;
        int currentGeneration_ = 0;
        bool gameOver_ = false;
    };
}