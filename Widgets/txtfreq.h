#ifndef TXTFREQ_H
#define TXTFREQ_H

#include <QLineEdit>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class TxtFreq : public QLineEdit
{
    Q_OBJECT
public:
    explicit TxtFreq(GlobalVariables *gVar,
                     QWidget *parent = nullptr);
    ~TxtFreq();

private:
    GlobalVariables *gVar;
    QString style;
    double value = 0.0;

public:
    double getValue() const;
    void setValue(double num);

private:
    void updateDisplay();

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
};

#endif // TXTFREQ_H
