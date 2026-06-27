#ifndef CARETABLEITEM_H
#define CARETABLEITEM_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class CareTableItem : public QFrame
{
    Q_OBJECT
public:
    explicit CareTableItem(GlobalVariables *gVar,
                           CareTableStruct itemData,
                           QWidget *parent = nullptr);
    ~CareTableItem() override;

private:
    GlobalVariables *gVar;
    QString style;
    CareTableStruct item;
    bool isActive = false;

    QGridLayout *gLayout = nullptr;
    QLabel *lblName = nullptr;
    QLabel *lblTitleFreq = nullptr;
    QLabel *lblFreq = nullptr;
    QLabel *lblTitleBw = nullptr;
    QLabel *lblBw = nullptr;
    QLabel *lblTitleThr = nullptr;
    QLabel *lblThr = nullptr;
    QLabel *lblTitleLvl = nullptr;
    QLabel *lblLvl = nullptr;
    QLabel *lblTitleAzm = nullptr;
    QLabel *lblAzm = nullptr;
    QLabel *lblTitleSeen = nullptr;
    QLabel *lblSeen = nullptr;

public:


public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
};

#endif // CARETABLEITEM_H
