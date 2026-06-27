#ifndef MENULAYOUT_H
#define MENULAYOUT_H

#include <QWidget>
#include <QFrame>
#include <QTabWidget>
#include "Includes/globalvariables.h"
#include "Navigation/hometab.h"
#include "Navigation/viewtab.h"
#include "Navigation/toolstab.h"

class MenuLayout : public QFrame
{
    Q_OBJECT

public:
    explicit MenuLayout(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~MenuLayout() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    QTabWidget *tabWidget;
    HomeTab *homeTab;
    ViewTab *viewTab;
    ToolsTab *toolsTab;

public slots:
    HomeTab* getHomeTab() { return homeTab; }
    ViewTab* getViewTab() { return viewTab; }
    ToolsTab* getToolsTab() { return toolsTab; }
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:
};

#endif // MENULAYOUT_H
