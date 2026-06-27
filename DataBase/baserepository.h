#ifndef BASEREPOSITORY_H
#define BASEREPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QMap>
#include <QDebug>
#include <QVector>
#include <QDateTime>
#include "Includes/GlobalStruct.h"

class BaseRepository
{
public:
    explicit BaseRepository(QSqlDatabase database);

protected:
    QSqlDatabase db;
    bool isDbOpen() const;
    bool execQuery(QSqlQuery &query) const;
    bool updateFields(const QString &tableName,
                      int id,
                      const QMap<QString, QVariant> &fields,
                      const QString &idColumn = "eID") const;
    bool softDelete(const QString &tableName,
                    int id,
                    const QString &idColumn = "eID",
                    const QString &deleteColumn = "isDeleted") const;
    bool softDeleteAll(const QString &tableName,
                       const QString &deleteColumn = "isDeleted") const;
    bool truncateTable(const QString &tableName) const;
};

#endif // BASEREPOSITORY_H
