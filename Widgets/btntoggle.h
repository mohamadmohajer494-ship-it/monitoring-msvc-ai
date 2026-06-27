#ifndef BTNTOGGLE_H
#define BTNTOGGLE_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QPainter>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class BtnToggle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int circleX READ circleX WRITE setCircleX)
public:
    explicit BtnToggle(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~BtnToggle();

private:
    GlobalVariables *gVar;
    bool isOn;
    int _circleX;
    QPropertyAnimation *animation;

public:
    void setStatus(bool newActive);
    bool getStatus();

private:
    int circleXStart() const;
    int circleXEnd() const;
    void setCircleX(int x);
    int circleX() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void toggled(bool state);
};

#endif // BTNTOGGLE_H
