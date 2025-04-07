#include "itemdb/ItemDatabase.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QString itemDir = "data/items";
    ItemDatabase db(itemDir);

    QStringList ids = db.listItemIDs();
    qDebug() << "Found" << ids.size() << "item(s) in" << itemDir;

    int relicCount = 0;
    int echoTotal = 0;

    for (const QString& id : ids) {
        QJsonObject item;
        if (!db.loadItem(id, item)) {
            qWarning() << "Failed to load item:" << id;
            continue;
        }

        if (item["is_relic"].toBool()) relicCount++;
        echoTotal += item["echo_level"].toInt();
    }

    qDebug() << "Relic count:" << relicCount;
    if (!ids.isEmpty())
        qDebug() << "Average echo level:" << (echoTotal / double(ids.size()));

    return 0;
}
