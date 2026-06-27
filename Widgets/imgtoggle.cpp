#include "imgtoggle.h"

ImgToggle::ImgToggle(GlobalVariables *gVar, QString icon1, QString name1, QString icon2, QString name2, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
    , icon1(icon1)
    , name1(name1)
    , icon2(icon2)
    , name2(name2)
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(5);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    box1 = new QFrame();
    box1->setObjectName("box1");
    box1->setCursor(Qt::PointingHandCursor);
    setToolTip(name1);
    vLayout->addWidget(box1);
    QVBoxLayout *vLay1 = new QVBoxLayout();
    vLay1->setContentsMargins(0, 0, 0, 0);
    vLay1->setSpacing(0);
    box1->setLayout(vLay1);
    lbl1 = new QLabel();
    lbl1->setAlignment(Qt::AlignCenter);
    vLay1->addWidget(lbl1, 0, Qt::AlignCenter);
    box2 = new QFrame();
    box2->setObjectName("box2");
    box2->setCursor(Qt::PointingHandCursor);
    setToolTip(name2);
    vLayout->addWidget(box2);
    QVBoxLayout *vLay2 = new QVBoxLayout();
    vLay2->setContentsMargins(0, 0, 0, 0);
    vLay2->setSpacing(0);
    box2->setLayout(vLay2);
    lbl2 = new QLabel();
    lbl2->setAlignment(Qt::AlignCenter);
    vLay2->addWidget(lbl2, 0, Qt::AlignCenter);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ImgToggle::slotThemeChange);
}

ImgToggle::~ImgToggle()
{

}

void ImgToggle::setActive(bool act)
{
    isActive = act;
    slotThemeChange();
}

void ImgToggle::slotThemeChange()
{
    style = "#box1 { ";
    style += "border-radius: 5px;";
    if (isActive)
        style += "background: " + gVar->getClrSurface() + ";";
    else
        style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    style += "#box2 { ";
    style += "border-radius: 5px;";
    style += "font-weight: bold;";
    if (isActive)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "#box1:hover, #box2:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    lbl1->setPixmap(gVar->changeColorIcon(icon1,
                                          gVar->getClrPrimary(),
                                          QSize(width()/2, width()/2)));
    style = "background : transparent ;";
    lbl1->setStyleSheet(style);
    lbl2->setPixmap(gVar->changeColorIcon(icon2,
                                          gVar->getClrPrimary(),
                                          QSize(width()/2, width()/2)));
    style = "background : transparent ;";
    lbl2->setStyleSheet(style);
}

void ImgToggle::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    setFixedWidth(height()/2);
    box1->setFixedWidth(height()/2 - 2.5);
    box1->setFixedHeight(height()/2 - 2.5);
    box2->setFixedWidth(height()/2 - 2.5);
    box2->setFixedHeight(height()/2 - 2.5);
    lbl1->setPixmap(gVar->changeColorIcon(icon1,
                                          gVar->getClrPrimary(),
                                          QSize(width()/2, width()/2)));
    lbl2->setPixmap(gVar->changeColorIcon(icon2,
                                          gVar->getClrPrimary(),
                                          QSize(width()/2, width()/2)));
}

void ImgToggle::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (box1->geometry().contains(event->pos()))
            setActive(false);
        else if (box2->geometry().contains(event->pos()))
            setActive(true);
        emit click(isActive);
    }
}
