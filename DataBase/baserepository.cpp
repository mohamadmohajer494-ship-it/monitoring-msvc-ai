#include "baserepository.h"



BaseRepository::BaseRepository(QSqlDatabase database)
    : db(database)
{

}

bool BaseRepository::isDbOpen() const
{
    return db.isOpen();
}

bool BaseRepository::execQuery(QSqlQuery &query) const
{
    if (!query.exec()) {
        qWarning() << "SQL Error:" << query.lastError();
        return false;
    }
    return true;
}

bool BaseRepository::updateFields(const QString &tableName,
                                  int id,
                                  const QMap<QString, QVariant> &fields,
                                  const QString &idColumn) const
{
    if (!db.isOpen() || fields.isEmpty())
        return false;
    QStringList assignments;
    for (auto it = fields.begin(); it != fields.end(); ++it)
        assignments << QString("%1 = :%1").arg(it.key());
    QString sql = QString("UPDATE %1 SET %2 WHERE %3 = :id AND isDeleted = FALSE")
                      .arg(tableName, assignments.join(", "), idColumn);
    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":id", id);
    for (auto it = fields.begin(); it != fields.end(); ++it)
        query.bindValue(":" + it.key(), it.value());
    if (!execQuery(query))
        return false;
    return query.numRowsAffected() > 0;
}

bool BaseRepository::softDelete(const QString &tableName,
                                int id,
                                const QString &idColumn,
                                const QString &deleteColumn) const
{
    if (!db.isOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(QString("UPDATE %1 SET %2 = TRUE WHERE %3 = :id AND %2 = FALSE")
                      .arg(tableName, deleteColumn, idColumn));
    query.bindValue(":id", id);
    if (!execQuery(query))
        return false;
    return query.numRowsAffected() > 0;
}

bool BaseRepository::softDeleteAll(const QString &tableName,
                                   const QString &deleteColumn) const
{
    if (!db.isOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(QString("UPDATE %1 SET %2 = TRUE WHERE %2 = FALSE")
                      .arg(tableName, deleteColumn));
    return execQuery(query);
}

bool BaseRepository::truncateTable(const QString &tableName) const
{
    if (!db.isOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(QString("TRUNCATE TABLE %1 RESTART IDENTITY").arg(tableName));
    return execQuery(query);
}
