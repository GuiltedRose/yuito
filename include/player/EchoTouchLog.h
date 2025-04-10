#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <QJsonObject>

struct EchoTouch {
    std::string characterID;
    std::string worldID;
    int generation;
    std::string emotionalTag; // e.g. "vengeful", "hopeful"
    std::string timestamp; 
    QJsonObject toJson() const;
    static EchoTouch fromJson(const QJsonObject& obj);   // ISO 8601, or use time_point if preferred
};

class EchoTouchLog {
public:
    void logTouch(const std::string& itemID, const EchoTouch& touch);
    const std::vector<EchoTouch>& getTouchHistory(const std::string& itemID) const;
    std::optional<EchoTouch> getLastTouch(const std::string& itemID) const;

    bool wasTouchedBy(const std::string& itemID, const std::string& characterID) const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    std::unordered_map<std::string, std::vector<EchoTouch>> touchMap;
};
