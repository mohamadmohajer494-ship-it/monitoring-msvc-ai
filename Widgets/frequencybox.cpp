#include "frequencybox.h"

FrequencyBox::FrequencyBox(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 5);
    vLayout->setSpacing(0);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    lblTitle = new QLabel(tr("Center Frequency"));
    lblTitle->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblTitle, Qt::AlignCenter);
    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);
    txtFreq = new QLineEdit();
    txtFreq->setAlignment(Qt::AlignCenter);
    QDoubleValidator* doubleValidator = new QDoubleValidator(0, 1e12, 4, txtFreq);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    doubleValidator->setDecimals(4);
    txtFreq->setValidator(doubleValidator);
    txtFreq->setFont(gVar->changeFont(":/Res/Fonts/DS-DIGIB.TTF"
                                      ,28
                                      ,true));
    txtFreq->setText(QString::number(centerFreq, 'f', 4));
    txtFreq->installEventFilter(this);
    connect(txtFreq, &QLineEdit::editingFinished,
            this, &FrequencyBox::freqEditingFinished);
    hLayout->addWidget(txtFreq);
    cmbUnit = new QComboBox();
    cmbUnit->setFont(gVar->changeFont(":/Res/Fonts/DS-DIGIB.TTF"
                                      ,18
                                      ,true));
    cmbUnit->setFixedWidth(60);
    cmbUnit->addItem("Hz");
    cmbUnit->addItem("KHz");
    cmbUnit->addItem("MHz");
    cmbUnit->addItem("GHz");
    cmbUnit->blockSignals(true);
    cmbUnit->setCurrentIndex(2);
    freqUnit = MHz;
    cmbUnit->blockSignals(false);
    connect(cmbUnit,&QComboBox::currentIndexChanged,
            this, &FrequencyBox::freqUnitChange);
    hLayout->addWidget(cmbUnit);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&FrequencyBox::slotThemeChange);
}

FrequencyBox::~FrequencyBox()
{

}

void FrequencyBox::setFrequency(double mhz)
{
    centerFreq = mhz;
    // lastFreq = mhz;
    txtFreq->setText(QString::number(mhz, 'f', 4));
}

void FrequencyBox::setMinFreq(double mhz)
{
    minFreq = mhz;
}

void FrequencyBox::setMaxFreq(double mhz)
{
    maxFreq = mhz;
}

void FrequencyBox::setFreqUnit(EnumFreqUnit unit)
{
    QString strUnit = "MHz";
    switch (unit) {
    case Hz:
        strUnit = "Hz";
        break;
    case KHz:
        strUnit = "KHz";
        break;
    case MHz:
        strUnit = "MHz";
        break;
    case GHz:
        strUnit = "GHz";
        break;
    default: break;
    }
    if (cmbUnit)
        cmbUnit->setCurrentText(strUnit);
}

void FrequencyBox::setUnitByShortcutChar(QChar key)
{
    key = key.toLower();
    if (key == 'h') setFreqUnit(Hz);
    else if (key == 'k') setFreqUnit(KHz);
    else if (key == 'm') setFreqUnit(MHz);
    else if (key == 'g') setFreqUnit(GHz);
}

QString FrequencyBox::setStyle(QString color)
{
    QString strStyle;
    strStyle = "QLineEdit { " ;
    strStyle += "background : transparent ;";
    strStyle += "padding-left : 10px ; " ;
    strStyle += "border: none ; " ;
    strStyle += "color : " + color + " ; " ;
    strStyle += "}" ;
    strStyle += "QComboBox { " ;
    strStyle += "background : transparent ;";
    strStyle += "margin-top : 0px ; " ;
    strStyle += "margin-bottom : 0px ; " ;
    strStyle += "padding-right : 10px ; " ;
    strStyle += "padding-top : 10px ; " ;
    strStyle += "border : none ; " ;
    strStyle += "color : " + color + " ; " ;
    strStyle += "; }";
    strStyle += "QComboBox::drop-down { " ;
    strStyle += "border : 0px ; " ;
    strStyle += "width : 0px ; " ;
    strStyle += "} " ;
    strStyle += "QComboBox::down-arrow {" ;
    strStyle += "image : none ; " ;
    strStyle += "}" ;
    return strStyle;
}

void FrequencyBox::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    setFixedWidth(height() * 3.7);
}

void FrequencyBox::mousePressEvent(QMouseEvent *event)
{

}

bool FrequencyBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == txtFreq && event->type() == QEvent::MouseButtonDblClick) {
        txtFreq->selectAll();
        return true;
    }
    return QFrame::eventFilter(watched, event);
}

void FrequencyBox::freqEditingFinished()
{
    bool ok = false;
    double value = txtFreq->text().toDouble(&ok);
    if (!ok || value < 0 || value == lastFreq) {
        txtFreq->blockSignals(true);
        txtFreq->setText(QString::number(lastFreq, 'f', 4));
        txtFreq->clearFocus();
        txtFreq->blockSignals(false);
        return;
    }
    switch (freqUnit) {
    case Hz:
        value /= 1000000;
        break;
    case KHz:
        value /= 1000;
        break;
    case MHz:
        value /= 1;
        break;
    case GHz:
        value *= 1000;
        break;
    default:
        break;
    }
    if (value < minFreq)
        value = minFreq;
    else if (value > maxFreq)
        value = maxFreq;
    txtFreq->blockSignals(true);
    txtFreq->setText(QString::number(value, 'f', 4));
    style = "QLineEdit { " ;
    style += "background : transparent ;";
    style += "padding-left : 10px ; " ;
    style += "border: none ; " ;
    style += "color : " + gVar->getClrAccent() + " ; " ;
    style += "}" ;
    txtFreq->setStyleSheet(style);
    txtFreq->clearFocus();
    txtFreq->blockSignals(false);
    lastFreq = value;
    // value = lastFreq;
    // pushHistory(value,freqUnit);
    // previewIndex=-1;
    emit changeFrequency(value);
}

void FrequencyBox::freqUnitChange()
{

}

void FrequencyBox::slotThemeChange()
{
    style = "FrequencyBox { ";
    style += "border-radius: 5px;";
    style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "background : transparent ;";
    style += "font-weight : bold ;";
    style += "color : " + gVar->getClrPrimary() + ";";
    lblTitle->setStyleSheet(style);
    txtFreq->setStyleSheet(setStyle(gVar->getClrPrimary()));
    cmbUnit->setStyleSheet(setStyle(gVar->getClrPrimary()));
}
