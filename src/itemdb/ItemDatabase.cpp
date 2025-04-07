#include "itemdb/ItemDatabase.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include "lz4.h"

ItemDatabase::ItemDatabase(const QString& itemDir)
    : basePath(itemDir)
{
    QDir dir(itemDir);
    if (!dir.exists())
        dir.mkpath(".");
}

QString ItemDatabase::makeFilePath(const QString& id, CompressionBackend backend) const {
    QString ext = (backend == CompressionBackend::QtZlib) ? ".jz" : ".jl4";
    return basePath + "/" + id + ext;
}

CompressionBackend ItemDatabase::detectBackend(const QString& filename) const {
    if (filename.endsWith(".jz"))
        return CompressionBackend::QtZlib;
    if (filename.endsWith(".jl4"))
        return CompressionBackend::LZ4;
    return CompressionBackend::QtZlib; // fallback
}

QByteArray ItemDatabase::compressData(const QByteArray& raw, CompressionBackend backend) const {
    if (backend == CompressionBackend::QtZlib) {
        return qCompress(raw);
    } else {
        int maxSize = LZ4_compressBound(raw.size());
        QByteArray output(maxSize + sizeof(int), 0);
        memcpy(output.data(), &raw.size(), sizeof(int)); // uncompressed size

        int compressedSize = LZ4_compress_default(
            raw.constData(), output.data() + sizeof(int),
            raw.size(), maxSize
        );

        if (compressedSize <= 0) return {};
        output.resize(compressedSize + sizeof(int));
        return output;
    }
}

QByteArray ItemDatabase::decompressData(const QByteArray& raw, CompressionBackend backend) const {
    if (backend == CompressionBackend::QtZlib) {
        return qUncompress(raw);
    } else {
        if (raw.size() < static_cast<int>(sizeof(int))) return {};
        int originalSize = 0;
        memcpy(&originalSize, raw.constData(), sizeof(int));
        QByteArray output(originalSize, 0);

        int result = LZ4_decompress_safe(
            raw.constData() + sizeof(int), output.data(),
            raw.size() - sizeof(int), originalSize
        );
        if (result < 0) return {};
        return output;
    }
}

bool ItemDatabase::loadItem(const QString& id, QJsonObject& outItem) {
    QDir dir(basePath);
    QStringList matches = dir.entryList({id + ".jz", id + ".jl4"}, QDir::Files);
    if (matches.isEmpty()) return false;

    QString filePath = basePath + "/" + matches.first();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray compressed = file.readAll();
    CompressionBackend backend = detectBackend(file.fileName());
    QByteArray jsonData = decompressData(compressed, backend);

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) return false;

    outItem = doc.object();
    return true;
}

bool ItemDatabase::saveItem(const QString& id, const QJsonObject& item, CompressionBackend backend) {
    QString filePath = makeFilePath(id, backend);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

    QJsonDocument doc(item);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QByteArray compressed = compressData(jsonData, backend);

    if (compressed.isEmpty()) return false;

    file.write(compressed);
    return true;
}

QStringList ItemDatabase::listItemIDs() const {
    QDir dir(basePath);
    QStringList files = dir.entryList(QStringList() << "*.jz" << "*.jl4", QDir::Files);
    QStringList result;
    for (const QString& file : files) {
        QString id = file;
        id.chop(file.endsWith(".jz") ? 3 : 4); // remove extension
        result << id;
    }
    return result;
}
