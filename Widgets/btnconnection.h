#ifndef BTNCONNECTION_H
#define BTNCONNECTION_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include "Includes/globalvariables.h"

class BtnConnection : public QFrame
{
    Q_OBJECT
public:
    explicit BtnConnection(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~BtnConnection() override;

private:
    GlobalVariables *gVar;
    QString style;
    QString name = "Connection";
    QString icon = ":/Res/Images/Connection.png";
    QLabel *lblIcon;
    QLabel *lblText;

public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void leftClick();
    void rightClick();
};

#endif // BTNCONNECTION_H
