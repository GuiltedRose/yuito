#include "itemdb/ItemDatabase.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

ItemDatabase::ItemDatabase(const QString& itemDir)
    : basePath(itemDir)
{
    QDir dir(itemDir);
    if (!dir.exists())
        dir.mkpath(".");
}

QString ItemDatabase::makeFilePath(const QString& id) const {
    return basePath + "/" + id + ".jz";
}

QByteArray ItemDatabase::compressData(const QByteArray& raw) const {
    return qCompress(raw);
}

QByteArray ItemDatabase::decompressData(const QByteArray& raw) const {
    return qUncompress(raw);
}

bool ItemDatabase::loadItem(const QString& id, QJsonObject& outItem) const {
    QString filePath = makeFilePath(id);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray compressed = file.readAll();
    QByteArray jsonData = decompressData(compressed);

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) return false;

    outItem = doc.object();
    return true;
}

bool ItemDatabase::saveItem(const QString& id, const QJsonObject& item, CompressionBackend) {
    QString filePath = makeFilePath(id);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

    QJsonDocument doc(item);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QByteArray compressed = compressData(jsonData);

    if (compressed.isEmpty()) return false;

    file.write(compressed);
    return true;
}

QStringList ItemDatabase::listItemIDs() const {
    QDir dir(basePath);
    QStringList files = dir.entryList(QStringList() << "*.jz", QDir::Files);
    QStringList result;
    for (const QString& file : files) {
        QString id = file;
        id.chop(3); // remove ".jz"
        result << id;
    }
    return result;
}

bool ItemDatabase::hasPlayerTouchedItem(const QString& playerId, const QJsonObject& item) const {
    QJsonArray history = item.value("history").toArray();
    for (const QJsonValue& entryVal : history) {
        QJsonObject entry = entryVal.toObject();
        if (entry.value("player").toString() == playerId) {
            return true;
        }
    }
    return false;
}

QStringList ItemDatabase::getItemsTouchedByPlayer(const QString& playerId) const {
    QStringList result;
    QDir dir(basePath);
    QStringList itemFiles = dir.entryList(QStringList() << "*.jz", QDir::Files);

    for (const QString& fileName : itemFiles) {
        QString itemId = fileName;
        itemId.chop(3);

        QJsonObject item;
        if (!loadItem(itemId, item))
            continue;

        if (hasPlayerTouchedItem(playerId, item)) {
            result << itemId;
        }
    }

    return result;
}
