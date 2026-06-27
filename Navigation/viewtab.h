#ifndef VIEWTAB_H
#define VIEWTAB_H

#include <QWidget>
#include <QFrame>
#include "Includes/globalvariables.h"
#include "Widgets/imgbutton.h"

class ViewTab : public QFrame
{
    Q_OBJECT
public:
    explicit ViewTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~ViewTab() override;

private:
    GlobalVariables *gVar;
    QString style;
    QHBoxLayout *hLayout;
    QList<QLabel*> lblList;
    QList<QFrame*> linList;
    ImgButton *btnOverview;
    ImgButton *btnPanorama;
    ImgButton *btnAnalysis;
    ImgButton *btnPosition;
    ImgButton *btnHandoff;
    ImgButton *btnDirection01;
    ImgButton *btnDirection02;
    ImgButton *btnDirection03;
    ImgButton *btnDirection04;
    ImgButton *btnDirection05;
    ImgButton *btnDirection06;
    ImgButton *btnDirection07;
    ImgButton *btnDirection08;
    ImgButton *btnDirection09;

public:
    ImgButton* getBtnOverview() { return btnOverview; }
    ImgButton* getBtnPanorama() { return btnPanorama; }
    ImgButton* getBtnAnalysis() { return btnAnalysis; }
    ImgButton* getBtnPosition() { return btnPosition; }
    ImgButton* getBtnHandoff() { return btnHandoff; }
    ImgButton* getBtnDirection01() { return btnDirection01; }
    ImgButton* getBtnDirection02() { return btnDirection02; }
    ImgButton* getBtnDirection03() { return btnDirection03; }
    ImgButton* getBtnDirection04() { return btnDirection04; }
    ImgButton* getBtnDirection05() { return btnDirection05; }
    ImgButton* getBtnDirection06() { return btnDirection06; }
    ImgButton* getBtnDirection07() { return btnDirection07; }
    ImgButton* getBtnDirection08() { return btnDirection08; }
    ImgButton* getBtnDirection09() { return btnDirection09; }
    ImgButton* getViewButton(const QString& name);
    QList<ImgButton*> getAllViewButtons();

private:
    void createMenuLin();
    QHBoxLayout* createMenuBox(QString txt);
    void createViewsBox();

public slots:
    void slotThemeChange();

signals:
};

#endif // VIEWTAB_H
