#pragma once

#include <QString>
#include <QJsonObject>
#include <QStringList>

enum class CompressionBackend {
    QtZlib,
    LZ4
};

class ItemDatabase {
public:
    explicit ItemDatabase(const QString& itemDir);

    bool loadItem(const QString& id, QJsonObject& outItem);
    bool saveItem(const QString& id, const QJsonObject& item, CompressionBackend backend);
    QStringList listItemIDs() const;

private:
    QString basePath;

    QString makeFilePath(const QString& id, CompressionBackend backend) const;
    CompressionBackend detectBackend(const QString& filename) const;

    QByteArray compressData(const QByteArray& raw, CompressionBackend backend) const;
    QByteArray decompressData(const QByteArray& raw, CompressionBackend backend) const;
};