#include "player/EchoTouchLog.h"
#include <QJsonArray>

void EchoTouchLog::logTouch(const std::string& itemID, const EchoTouch& touch) {
    touchMap[itemID].push_back(touch);
}

const std::vector<EchoTouch>& EchoTouchLog::getTouchHistory(const std::string& itemID) const {
    static const std::vector<EchoTouch> empty;
    auto it = touchMap.find(itemID);
    return it != touchMap.end() ? it->second : empty;
}

std::optional<EchoTouch> EchoTouchLog::getLastTouch(const std::string& itemID) const {
    auto it = touchMap.find(itemID);
    if (it != touchMap.end() && !it->second.empty()) {
        return it->second.back();
    }
    return std::nullopt;
}

bool EchoTouchLog::wasTouchedBy(const std::string& itemID, const std::string& characterID) const {
    auto it = touchMap.find(itemID);
    if (it == touchMap.end()) return false;
    for (const auto& touch : it->second) {
        if (touch.characterID == characterID) return true;
    }
    return false;
}

QJsonObject EchoTouch::toJson() const {
    QJsonObject obj;
    obj["characterID"] = QString::fromStdString(characterID);
    obj["worldID"] = QString::fromStdString(worldID);
    obj["generation"] = generation;
    obj["emotionalTag"] = QString::fromStdString(emotionalTag);
    obj["timestamp"] = QString::fromStdString(timestamp);
    return obj;
}

EchoTouch EchoTouch::fromJson(const QJsonObject& obj) {
    EchoTouch t;
    t.characterID = obj["characterID"].toString().toStdString();
    t.worldID = obj["worldID"].toString().toStdString();
    t.generation = obj["generation"].toInt();
    t.emotionalTag = obj["emotionalTag"].toString().toStdString();
    t.timestamp = obj["timestamp"].toString().toStdString();
    return t;
}

QJsonObject EchoTouchLog::toJson() const {
    QJsonObject obj;
    for (const auto& [itemID, touches] : touchMap) {
        QJsonArray arr;
        for (const auto& t : touches)
            arr.append(t.toJson());
        obj[QString::fromStdString(itemID)] = arr;
    }
    return obj;
}

void EchoTouchLog::fromJson(const QJsonObject& json) {
    touchMap.clear();
    for (const auto& key : json.keys()) {
        std::vector<EchoTouch> touches;
        QJsonArray arr = json[key].toArray();
        for (const auto& entry : arr)
            touches.push_back(EchoTouch::fromJson(entry.toObject()));
        touchMap[key.toStdString()] = touches;
    }
}
