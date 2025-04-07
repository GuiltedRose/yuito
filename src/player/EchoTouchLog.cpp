#include "player/EchoTouchLog.h"

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
