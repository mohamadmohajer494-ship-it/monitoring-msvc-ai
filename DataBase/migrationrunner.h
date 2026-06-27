#ifndef MIGRATIONRUNNER_H
#define MIGRATIONRUNNER_H

#include <QSqlDatabase>
#include <QVector>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>
#include <QDebug>

struct Migration {
    int version;
    QStringList sqlUp;
};

class MigrationRunner
{
public:
    static bool runPendingMigrations(QSqlDatabase db);

private:
    static QVector<Migration> migrations();
};

#endif // MIGRATIONRUNNER_H
