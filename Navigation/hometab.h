#ifndef HOMETAB_H
#define HOMETAB_H

#include <QWidget>
#include <QFrame>
#include "Includes/globalvariables.h"
#include "Widgets/btnconnection.h"
#include "Widgets/imgtoggle.h"
#include "Widgets/frequencybox.h"
#include "Widgets/btntextflip.h"

class HomeTab : public QFrame
{
    Q_OBJECT
public:
    explicit HomeTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~HomeTab() override;

private:
    GlobalVariables *gVar;
    QString style;
    QHBoxLayout *hLayout;
    QList<QLabel*> lblList;
    QList<QFrame*> linList;
    BtnConnection *btnConnection;
    ImgToggle *btnPlayPause;
    FrequencyBox *freqBox;
    BtnTextFlip *btnBandwidth;
    BtnTextFlip *btnResolution;
    BtnTextFlip *btnModulation;
    BtnTextFlip *btnIFFilter;
    BtnTextFlip *btnRFHead;

public:
    BtnConnection* getBtnConnection() { return btnConnection; }
    ImgToggle* getBtnPlayPause() { return btnPlayPause; }
    FrequencyBox* getFreqBox() { return freqBox; }
    BtnTextFlip* getBtnBandwidth() { return btnBandwidth; }
    BtnTextFlip* getBtnResolution() { return btnResolution; }
    BtnTextFlip* getBtnModulation() { return btnModulation; }
    BtnTextFlip* getBtnIFFilter() { return btnIFFilter; }
    BtnTextFlip* getBtnRFHead() { return btnRFHead; }

private:
    void createMenuLin();
    QHBoxLayout* createMenuBox(QString txt);
    void createConnectionBox();
    void createFrequencyControl();
    void createSignalProcessing();

public slots:
    void slotThemeChange();

signals:
};

#endif // HOMETAB_H
