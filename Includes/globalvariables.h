#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <QObject>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QResizeEvent>
#include <QPixmap>
#include <QImage>
#include <QColor>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QFontDatabase>
#include <QContextMenuEvent>
#include "Includes/GlobalEnum.h"
#include "Includes/GlobalStruct.h"
#include "Includes/stationmodel.h"
#include "DataBase/dbmanager.h"
#include "DataBase/repositorystation.h"
#include "Connection/socketmanager.h"

class GlobalVariables : public QObject
{
    Q_OBJECT
public:
    explicit GlobalVariables(QObject *parent = nullptr);


    // ---------  ---------
private:
    DbConfig dbConfig;
    DbManager *dbManager = nullptr;
    QSqlDatabase dataBase;
    RepositoryStation *repStation;
    QVector<StationModel*> stationList;
    QVector<StationStruct> stationSelect;
    QVector<SocketManager*> socketList;
    EnumSystemMode mainSystem = Role_NoMode;

public:
    DbConfig getDbConfig() { return dbConfig; }
    void setDbConfig(DbConfig cfg) { dbConfig = cfg; }
    DbManager* getDbManager() { return dbManager; }
    void setDbManager(DbManager *dbm) { dbManager = dbm; }
    QSqlDatabase getDatabase() { return dataBase; }
    void setDatabase(QSqlDatabase db) { dataBase = db; }
    void getAllData();
    RepositoryStation* getRepStation() { return repStation; }
    void setRepStation(RepositoryStation rep) { repStation = &rep; }
    QVector<StationModel*> getStationList() { return stationList; }
    void setStationList(QVector<StationModel*> list) { stationList = list; }
    void addToStationList(StationModel* station);
    void removeStation(int stationId);
    StationModel* findStationById(int id);
    QVector<SocketManager*> getSocketList() { return socketList; }
    void setSocketList(QVector<SocketManager*> list) { socketList = list; }
    void addToSocketList(SocketManager* socket);
    SocketManager* findSocketByStationId(int id);
    EnumSystemMode getMainSystem() { return mainSystem; }
    void setMainSystem(EnumSystemMode sys) { mainSystem = sys; }

private:
    void createStationList();

    // --------- Screen ---------
private:
    QScreen *screen;
    int screenWidth = 800;
    int screenHeight = 600;
    double menuLyoutHeight = 130;
    double mainLyoutHeight = 0.89;
    double mainLyoutWidth = 0.85;
    double rightLyoutWidth = 0.15;
    double leftLyoutCloseWidth = 60;
    double leftLyoutOpenWidth = 220;
public:
    int getScreenWidth() { return screenWidth; }
    void setScreenWidth(int size) { screenWidth = size; }
    int getScreenHeight() { return screenHeight; }
    void setScreenHeight(int size) { screenHeight = size; }
    double getMenuLyoutHeight() { return menuLyoutHeight; }
    void setMenuLyoutHeight(double size) { menuLyoutHeight = size; }
    double getMainLyoutHeight() { return mainLyoutHeight; }
    void setMainLyoutHeight(double size) { mainLyoutHeight = size; }
    double getMainLyoutWidth() { return mainLyoutWidth; }
    void setMainLyoutWidth(double size) { mainLyoutWidth = size; }
    double getRightLyoutWidth() { return rightLyoutWidth; }
    void setRightLyoutWidth(double size) { rightLyoutWidth = size; }
    double getLeftLyoutCloseWidth() { return leftLyoutCloseWidth; }
    void setLeftLyoutCloseWidth(double size) { leftLyoutCloseWidth = size; }
    double getLeftLyoutOpenWidth() { return leftLyoutOpenWidth; }
    void setLeftLyoutOpenWidth(double size) { leftLyoutOpenWidth = size; }

    // --------- Theme ---------
private:
    QString clrBackground = "#182632";
    QString clrSurface = "#1f3340";
    QString clrGlow = "#375d76";
    QString clrPrimary = "#e1e1e1";
    QString clrSecondary = "#03DAC6";
    QString clrHighlight = "#6bb0e2";
    QString clrAccent = "#1af67a";
    QString clrDanger = "#1af67a";
    QString clrWarning  = "#ffff00";
    QString clrError  = "#aa0000";
    QString clrCorrect  = "#005500";
public:
    QString getClrBackground() { return clrBackground; }
    void setClrBackground(QString color) { clrBackground = color; ThemeChange(); }
    QString getClrSurface() { return clrSurface; }
    void setClrSurface(QString color) { clrSurface = color; ThemeChange(); }
    QString getClrGlow() { return clrGlow; }
    void setClrGlow(QString color) { clrGlow = color; ThemeChange(); }
    QString getClrPrimary() { return clrPrimary; }
    void setClrPrimary(QString color) { clrPrimary = color; ThemeChange(); }
    QString getClrSecondary() { return clrSecondary; }
    void setClrSecondary(QString color) { clrSecondary = color; ThemeChange(); }
    QString getClrHighlight() { return clrHighlight; }
    void setClrHighlight(QString color) { clrHighlight = color; ThemeChange(); }
    QString getClrAccent() { return clrAccent; }
    void setClrAccent(QString color) { clrAccent = color; ThemeChange(); }
    QString getClrWarning() { return clrWarning; }
    void setClrWarning(QString color) { clrWarning = color; ThemeChange(); }
    QString getClrError() { return clrError; }
    void setClrError(QString color) { clrError = color; ThemeChange(); }
    QString getClrCorrect() { return clrCorrect; }
    void setClrCorrect(QString color) { clrCorrect = color; ThemeChange(); }
signals:
    void ThemeChange();
    void LoadDataBaseStation();

    // --------- Functions ---------
public:
    QPixmap changeColorIcon(const QString &path, const QColor &color, const QSize &size);
    QFont changeFont(const QString &path, const int &size, const bool &isbold = false);

};

#endif // GLOBALVARIABLES_H
