#ifndef CARETABLETAB_H
#define CARETABLETAB_H

#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include "Includes/globalvariables.h"
#include "Widgets/caretableitem.h"

class CareTableTab : public QFrame
{
    Q_OBJECT
public:
    explicit CareTableTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~CareTableTab() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    QScrollArea *scrollArea;
    QWidget *containerWidget;
    QVector<CareTableItem*> *itemList;

public slots:
    void slotThemeChange();

signals:
};

#endif // CARETABLETAB_H
