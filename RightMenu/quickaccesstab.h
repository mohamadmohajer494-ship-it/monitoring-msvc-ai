#ifndef QUICKACCESSTAB_H
#define QUICKACCESSTAB_H

#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QCheckBox>
#include "Includes/globalvariables.h"

class QuickAccessTab : public QFrame
{
    Q_OBJECT
public:
    explicit QuickAccessTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~QuickAccessTab() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout = nullptr;
    QFrame *topBar = nullptr;
    QHBoxLayout *topLayout = nullptr;
    QPushButton *btnOptions = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *containerWidget = nullptr;

    QMenu *optionsMenu = nullptr;
    QCheckBox *chkShowFreq = nullptr;
    QCheckBox *chkShowBw = nullptr;
    QCheckBox *chkShowRes = nullptr;
    QCheckBox *chkShowMod = nullptr;
    QCheckBox *chkShowIff = nullptr;
    QCheckBox *chkShowRfh = nullptr;
    // QCheckBox *chkShowView = nullptr;
    // QCheckBox *chkShowView = nullptr;
    // QCheckBox *chkShowView = nullptr;

    bool isShowView = true;
    bool isShowFreq = true;
    bool isShowBw = true;
    bool isShowRes = false;
    bool isShowMod = false;
    bool isShowIff = false;
    bool isShowRfh = false;

private:
    void createUi();
    void createOptionsMenu();
    void showOptionsMenu();
    void applyOptionsMenuStyle();
    void updateQuickAccessView();
    QCheckBox* addOptionCheckBox(const QString &text, bool checked);

public slots:
    void slotThemeChange();


signals:
};

#endif // QUICKACCESSTAB_H
