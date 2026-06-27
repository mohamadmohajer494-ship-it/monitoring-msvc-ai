#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

#include <QWidget>
#include <QFrame>
#include <QStackedWidget>
#include "floatwindow.h"
#include "Includes/globalvariables.h"
#include "Windows/overview.h"
#include "Windows/panorama.h"
#include "Windows/analysis.h"
#include "Windows/position.h"
#include "Windows/handoff.h"
#include "Windows/direction.h"

class MainLayout : public QFrame
{
    Q_OBJECT

public:
    explicit MainLayout(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~MainLayout() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    FloatWindow *floatWindow;

    Overview *overview;
    Panorama *panorama;
    Analysis *analysis;
    Position *position;
    Handoff *handoff;
    static constexpr int directionPoolSize = 9;
    Direction *directionPages[directionPoolSize] = {};

    QStackedWidget *stack;
    QList<QWidget*> widgetsList;
    QMap<QString, int> nameToIndex;
    QMap<int, QString> indexToName;
    QMap<int, FloatWindow*> floatWins;
    const int maxDetached = 5;

public:
    Overview* getOverview() { return overview; }
    Panorama* getPanorama() { return panorama; }
    Analysis* getAnalysis() { return analysis; }
    Position* getPosition() { return position; }
    Handoff* getHandoff() { return handoff; }
    Direction* getDirectionPage(int dfId) const;

private:
    void addPage(const QString &name, QWidget *widget);
    void createWindows();
    void updatePagesRefreshRate();
    bool isPageVisibleToUser(int index) const;

public slots:
    void showWidget(QString name);
    void detachWidget(QString name);
    void slotThemeChange();

signals:
    void setActiveView(QString name);
    void setInView(QString name);
    void setOutView(QString name);
};

#endif // MAINLAYOUT_H
