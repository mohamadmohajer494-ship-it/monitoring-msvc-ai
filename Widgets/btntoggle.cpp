#include "btntoggle.h"

BtnToggle::BtnToggle(GlobalVariables *gVar,
                     QWidget *parent)
    : QWidget{parent}
    , gVar(gVar)
    , isOn(false)
    , _circleX(3)
{
    setFixedSize(42, 24);
    animation = new QPropertyAnimation(this, "circleX", this);
    animation->setDuration(200);
}

BtnToggle::~BtnToggle()
{

}

void BtnToggle::setStatus(bool newActive)
{
    isOn = newActive;
    animation->stop();
    animation->setStartValue(_circleX);
    animation->setEndValue(isOn ? circleXEnd() : circleXStart());
    animation->start();
    emit toggled(isOn);
}

bool BtnToggle::getStatus()
{
    return isOn;
}

void BtnToggle::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect backgroundRect(0, 0, width(), height());
    painter.setBrush(isOn ? QColor(gVar->getClrAccent()) : QColor(gVar->getClrSecondary()));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(backgroundRect, height() / 2, height() / 2);
    QRect circleRect(_circleX, 3, 18, 18);
    painter.setBrush(QColor(gVar->getClrPrimary()));
    painter.drawEllipse(circleRect);
}

void BtnToggle::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    isOn = !isOn;
    animation->setStartValue(_circleX);
    animation->setEndValue(isOn ? circleXEnd() : circleXStart());
    animation->start();
    emit toggled(isOn);
}

int BtnToggle::circleXStart() const {
    return 3;
}

int BtnToggle::circleXEnd() const {
    return width() - 21;
}

void BtnToggle::setCircleX(int x) {
    _circleX = x;
    update();
}

int BtnToggle::circleX() const {
    return _circleX;
}
