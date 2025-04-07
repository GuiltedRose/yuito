#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>

class EchoTouchLog;

class PlayerCharacter {
public:
    PlayerCharacter(const std::string& id, const std::string& worldID, int generationNumber);

    // Core identity
    const std::string& getID() const;
    const std::string& getWorldID() const;
    int getGeneration() const;

    // Lifecycle
    void setDeathInfo(const std::string& cause);
    bool isDead() const;
    const std::string& getDeathCause() const;

    // Traits and personality (could affect dreams or relics)
    void setTrait(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getTraits() const;

    // Time and stats
    std::chrono::system_clock::time_point getBirthTime() const;
    std::chrono::system_clock::time_point getDeathTime() const;

    //Echo Touch Log
    void recordRelicTouch(const std::string& itemID, const std::string& emotionalTag);
    bool hasTouchedItem(const std::string& itemID) const;


private:
    std::string id;
    std::string worldID;
    int generation;

    std::unordered_map<std::string, std::string> traits;

    bool dead = false;
    std::string deathCause;
    std::chrono::system_clock::time_point birthTime;
    std::chrono::system_clock::time_point deathTime;

    std::unique_ptr<EchoTouchLog> personalEchoLog;
};
