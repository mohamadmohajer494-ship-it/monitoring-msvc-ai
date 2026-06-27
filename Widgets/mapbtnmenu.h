#ifndef MAPBTNMENU_H
#define MAPBTNMENU_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class MapBtnMenu : public QFrame
{
    Q_OBJECT
public:
    explicit MapBtnMenu(GlobalVariables *gVar,
                        QString icon,
                        QString name,
                        QStringList rightMenu = {},
                        QWidget *parent = nullptr);
    ~MapBtnMenu() override;

private:
    GlobalVariables *gVar;
    QString style;
    QString icon;
    QString name;
    QStringList rightMenu = {};
    QLabel *lblIcon;
    bool isActive = false;
    bool isLockStyle = false;

public:
    QString getName() { return name; }
    bool getActive() { return isActive; }
    void setActive(bool act);
    void setStyle(bool act);
    void setLockStyle(bool act);

private:
    void showUpwardMenu();

public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void leftClick(QString txt);
    void rightClick(QString txt);
};

#endif // MAPBTNMENU_H
