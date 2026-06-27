#ifndef IMGTOGGLE_H
#define IMGTOGGLE_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class ImgToggle : public QFrame
{
    Q_OBJECT
public:
    explicit ImgToggle(GlobalVariables *gVar,
                       QString icon1,
                       QString name1,
                       QString icon2,
                       QString name2,
                       QWidget *parent = nullptr);
    ~ImgToggle() override;

private:
    GlobalVariables *gVar;
    QString style;
    QString icon1;
    QString name1;
    QString icon2;
    QString name2;
    QFrame *box1;
    QLabel *lbl1;
    QFrame *box2;
    QLabel *lbl2;
    bool isActive = false;

public slots:
    bool getActive() { return isActive; }
    void setActive(bool act);
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void click(bool act);
};

#endif // IMGTOGGLE_H
