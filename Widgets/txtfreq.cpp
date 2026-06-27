#include "txtfreq.h"

TxtFreq::TxtFreq(GlobalVariables *gVar, QWidget *parent)
    : QLineEdit{parent}
    , gVar(gVar)
{
    auto *validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    setValidator(validator);
    updateDisplay();
}

TxtFreq::~TxtFreq()
{

}

double TxtFreq::getValue() const
{
    return value;
}

void TxtFreq::setValue(double num)
{
    value = num;
    updateDisplay();
}

void TxtFreq::updateDisplay()
{
    setText(QString("%1 MHz").arg(QString::number(value, 'f', 4)));
}

void TxtFreq::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    setText(QString::number(value, 'f', 4));
    selectAll();
}

void TxtFreq::focusOutEvent(QFocusEvent *event)
{
    bool ok;
    double val = text().trimmed().toDouble(&ok);
    if(ok)
        value = val;
    updateDisplay();
    QLineEdit::focusOutEvent(event);
}

void TxtFreq::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLineEdit::mouseDoubleClickEvent(event);
    selectAll();
}
