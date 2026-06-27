#ifndef IMGBUTTON_H
#define IMGBUTTON_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class ImgButton : public QFrame
{
    Q_OBJECT
public:
    explicit ImgButton(GlobalVariables *gVar,
                       QString icon,
                       QString name,
                       QWidget *parent = nullptr);
    ~ImgButton() override;

private:
    GlobalVariables *gVar;
    QString style;
    QString icon;
    QString name;
    QLabel *lblIcon;
    bool isActive = false;
    bool isLockStyle = false;

public:
    QString getName() { return name; }
    void setName(QString text) { name = text; }
    bool getActive() { return isActive; }
    void setActive(bool act);
    void setStyle(bool act);
    void setLockStyle(bool act);

public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void leftClick(QString txt);
    void rightClick(QString txt);
};

#endif // IMGBUTTON_H
