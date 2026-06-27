#include "dbmanager.h"
#include "migrationrunner.h"

DbManager::DbManager(QObject *parent)
    : QObject{parent}
{

}

DbManager::~DbManager()
{
    if (!QCoreApplication::instance())
        return;

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase d = QSqlDatabase::database(m_connectionName);
        if (d.isOpen()) d.close();
    }
}

bool DbManager::openToDb(const QString& dbName, QString* sqlStateOut)
{
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase old = QSqlDatabase::database(m_connectionName);
            if (old.isOpen()) old.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", m_connectionName);
    db.setHostName(m_cfg.host);
    db.setPort(m_cfg.port);
    db.setDatabaseName(dbName);
    db.setUserName(m_cfg.user);
    db.setPassword(m_cfg.password);
    db.setConnectOptions(buildConnectOptions(m_cfg));

    if (!db.open()) {
        QSqlError err = db.lastError();
        m_lastError = err.text();
        if (sqlStateOut) *sqlStateOut = err.nativeErrorCode();
        return false;
    }
    return true;
}

DbManager& DbManager::instance()
{
    static DbManager inst;
    return inst;
}

bool DbManager::init(const DbConfig& cfg, bool createIfMissing)
{
    m_cfg = cfg;
    bool ok = false;
    if (createIfMissing) {
        ok = createDatabaseIfNotExists();
    } else {
        ok = openToDb(m_cfg.dbName, nullptr);
    }
    if (!ok) return false;
    if (!ensureSchemaSearchPath()) return false;
    if (!ensureMigrations()) return false;
    if (!MigrationRunner::runPendingMigrations(db())) {
        m_lastError = "Migration failed";
        return false;
    }
    m_initialized = true;
    return true;
}

QSqlDatabase DbManager::db() const
{
    return QSqlDatabase::database(m_connectionName, false);
}

void DbManager::close()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        if (db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
    m_initialized = false;
}


bool DbManager::createDatabaseIfNotExists()
{
    QString sqlState;
    if (openToDb(m_cfg.dbName, &sqlState))
        return true;
    const bool dbMissing = (sqlState == "3D000") ||
                           m_lastError.contains("does not exist", Qt::CaseInsensitive);
    if (!dbMissing)
        return false;
    const QString adminConn = m_connectionName + "_admin";
    if (QSqlDatabase::contains(adminConn)) {
        {
            QSqlDatabase tmp = QSqlDatabase::database(adminConn);
            if (tmp.isOpen()) tmp.close();
        }
        QSqlDatabase::removeDatabase(adminConn);
    }
    {
        QSqlDatabase admin = QSqlDatabase::addDatabase("QPSQL", adminConn);
        admin.setHostName(m_cfg.host);
        admin.setPort(m_cfg.port);
        admin.setDatabaseName("postgres");
        admin.setUserName(m_cfg.user);
        admin.setPassword(m_cfg.password);
        admin.setConnectOptions(buildConnectOptions(m_cfg));
        if (!admin.open()) {
            m_lastError = admin.lastError().text();
            return false;
        }
        QSqlQuery q(admin);
        const QString createSql =
            QString("CREATE DATABASE \"%1\" WITH ENCODING 'UTF8' TEMPLATE template1;")
                .arg(m_cfg.dbName);
        if (!q.exec(createSql)) {
            m_lastError = q.lastError().text();
            admin.close();
            QSqlDatabase::removeDatabase(adminConn);
            return false;
        }
        admin.close();
    }
    QSqlDatabase::removeDatabase(adminConn);
    return openToDb(m_cfg.dbName, nullptr);
}

bool DbManager::ensureSchemaSearchPath()
{
    if (m_cfg.schema.isEmpty() || m_cfg.schema == "public")
        return true;
    QSqlDatabase d = db();
    if (!d.isOpen()) return false;
    QSqlQuery q(d);
    q.exec(QString("CREATE SCHEMA IF NOT EXISTS \"%1\";").arg(m_cfg.schema));
    const QString setPath =
        QString("SET search_path TO \"%1\", public;").arg(m_cfg.schema);
    if (!q.exec(setPath)) {
        m_lastError = q.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::ensureMigrations()
{
    QSqlDatabase d = db();
    if (!d.isOpen()) return false;
    QSqlQuery q(d);
    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS schema_migrations ("
            "  version INTEGER PRIMARY KEY,"
            "  applied_at TIMESTAMPTZ NOT NULL DEFAULT now()"
            ");"))
    {
        m_lastError = q.lastError().text();
        return false;
    }
    return true;
}
