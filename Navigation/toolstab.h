#ifndef TOOLSTAB_H
#define TOOLSTAB_H

#include <QWidget>
#include <QFrame>
#include "Includes/globalvariables.h"

class ToolsTab : public QFrame
{
    Q_OBJECT
public:
    explicit ToolsTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~ToolsTab() override;

private:
    GlobalVariables *gVar;
    QString style;
    QHBoxLayout *hLayout;

public slots:
    void slotThemeChange();

signals:
};

#endif // TOOLSTAB_H
