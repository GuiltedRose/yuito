#pragma once
#include <QJsonObject>

class Serializable {
public:
    virtual ~Serializable() = default;
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& json) = 0;
};
