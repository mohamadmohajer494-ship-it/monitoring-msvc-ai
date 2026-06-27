#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

struct DbConfig {
    QString host = "127.0.0.1";
    int port = 5432;
    QString dbName = "dbMonitoring";
    QString user = "postgres";
    QString password = "1234";
    QString schema = "public";
    QString applicationName = "Monitoring";
    int connectTimeoutSec = 5;
    bool useSSL = false;
};

class DbManager : public QObject
{
    Q_OBJECT
public:
    explicit DbManager(QObject *parent = nullptr);
    ~DbManager();

private:
    Q_DISABLE_COPY_MOVE(DbManager)
    bool openToDb(const QString& dbName, QString* sqlStateOut = nullptr);
    bool createDatabaseIfNotExists();
    bool ensureSchemaSearchPath();
    bool ensureMigrations();

public:
    static DbManager& instance();
    bool init(const DbConfig& cfg, bool createIfMissing = true);
    QSqlDatabase db() const;
    QString lastError() const { return m_lastError; }
    void close();

private:
    QString m_connectionName = "app_main_pg";
    DbConfig m_cfg;
    QString m_lastError;
    bool m_initialized = false;

private:
    static QString buildConnectOptions(const DbConfig& cfg) {
        QStringList opts;
        opts << QString("connect_timeout=%1").arg(cfg.connectTimeoutSec);
        if (!cfg.applicationName.isEmpty())
            opts << QString("application_name=%1").arg(cfg.applicationName);
        return opts.join(';');
    }
};

#endif // DBMANAGER_H
