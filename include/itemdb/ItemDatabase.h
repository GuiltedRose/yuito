#pragma once

#include <QString>
#include <QJsonObject>
#include <QStringList>

enum class CompressionBackend {
    QtZlib
};

class ItemDatabase {
public:
    explicit ItemDatabase(const QString& itemDir);

    bool loadItem(const QString& id, QJsonObject& outItem) const;
    bool saveItem(const QString& id, const QJsonObject& item, CompressionBackend backend);
    QStringList listItemIDs() const;

    bool hasPlayerTouchedItem(const QString& playerId, const QJsonObject& item) const;
    QStringList getItemsTouchedByPlayer(const QString& playerId) const;

private:
    QString basePath;

    QString makeFilePath(const QString& id) const;
    QByteArray compressData(const QByteArray& raw) const;
    QByteArray decompressData(const QByteArray& raw) const;
};
