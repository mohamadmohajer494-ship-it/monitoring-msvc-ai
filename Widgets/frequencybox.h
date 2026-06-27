#ifndef FREQUENCYBOX_H
#define FREQUENCYBOX_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMouseEvent>
#include <QDoubleValidator>
#include "Includes/globalvariables.h"

class FrequencyBox : public QFrame
{
    Q_OBJECT
public:
    explicit FrequencyBox(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~FrequencyBox() override;

private:
    GlobalVariables *gVar;
    QString style;
    QLabel *lblTitle;
    QLineEdit *txtFreq;
    QComboBox *cmbUnit;
    double centerFreq = 300;
    EnumFreqUnit freqUnit = MHz;
    double minFreq = 30;
    double maxFreq = 6000;
    double lastFreq = 300;

public:
    QLineEdit* getTxtFreq() { return txtFreq; }
    QComboBox* getCmbUnit() { return cmbUnit; }
    double getFrequency() { return centerFreq; }
    void setFrequency(double mhz);
    double getMinFreq() { return minFreq; }
    void setMinFreq(double mhz);
    double getMaxFreq() { return maxFreq; }
    void setMaxFreq(double mhz);
    EnumFreqUnit getFreqUnit() { return freqUnit; }
    void setFreqUnit(EnumFreqUnit unit);

private:
    void setUnitByShortcutChar(QChar key);
    QString setStyle(QString color);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void freqEditingFinished();
    void freqUnitChange();

public slots:
    void slotThemeChange();

signals:
    void changeFrequency(double mhz);
};

#endif // FREQUENCYBOX_H
