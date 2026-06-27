#ifndef RIGHTLAYOUT_H
#define RIGHTLAYOUT_H

#include <QWidget>
#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include "Includes/globalvariables.h"
#include "RightMenu/quickaccesstab.h"
#include "RightMenu/maplayertab.h"
#include "RightMenu/caretabletab.h"

class RightLayout : public QFrame
{
    Q_OBJECT

public:
    explicit RightLayout(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~RightLayout() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    QSplitter *vSplitter;
    QList<int> vSizes;
    QWidget *topBox;
    QTabWidget *tabWidgetTop;
    QuickAccessTab *quickAccessTab;
    MapLayerTab *mapLayerTab;
    QWidget *botBox;
    QTabWidget *tabWidgetBot;
    CareTableTab *careTableTab;

public:
    QuickAccessTab* getQuickAccess() { return quickAccessTab; }
    MapLayerTab* getMapLayerTab() { return mapLayerTab; }
    CareTableTab* getCareTableTab() { return careTableTab; }

private:
    void createTopBox();
    void createBotBox();

public slots:
    void slotThemeChange();

protected:
    void showEvent(QShowEvent *event) override;

signals:
};

#endif // RIGHTLAYOUT_H
