#ifndef DIRECTION_H
#define DIRECTION_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "Windows/basewindows.h"
#include "Plots/dfgauge.h"
#include "Plots/psdlib.h"
#include "Plots/pchlib.h"
#include "Plots/wflib.h"
#include "Widgets/btntoggle.h"
#include "Widgets/txtfreq.h"

class Direction : public BaseWindows
{
    Q_OBJECT
public:
    explicit Direction(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Direction() override;

private:
    QFrame *controllBox = nullptr;
    BtnToggle *btnSync = nullptr;
    BtnToggle *btnPf = nullptr;
    TxtFreq *txtFreq = nullptr;
    QComboBox *cmbBw = nullptr;
    QComboBox *cmbRes = nullptr;
    QComboBox *cmbMod = nullptr;
    QComboBox *cmbIff = nullptr;
    QComboBox *cmbRfh = nullptr;

    bool isShowControllBox = true;
    float controllBoxScale = 0.50f;
    float controllBoxSize = 0.0;
    DFGauge *gauge = nullptr;
    bool isShowGauge = true;
    float gaugesScale = 0.50f;
    float gaugesSize = 0.0;
    PchLib *doa = nullptr;
    bool isShowDoa = true;
    float doaScale = 0.33f;
    float doaSize = 0.0;
    PsdLib *psd = nullptr;
    bool isShowPsd = true;
    float psdScale = 0.33f;
    float psdSize = 0.0;
    WfLib *wf = nullptr;
    bool isShowWf = true;
    float wfScale = 0.33f;
    float wfSize = 0.0;
    float leftColumnScale = 0.20f;
    float splitterHeight = 2.0;

private:
    void createControllBox(GlobalVariables *gVar);
    void createGauge(GlobalVariables *gVar);
    void createDoa(GlobalVariables *gVar);
    void createPsd(GlobalVariables *gVar);
    void createWf(GlobalVariables *gVar);

public:
    QFrame* getControllBox() { return controllBox; }
    DFGauge* getGauge() { return gauge; }
    PchLib* getDoa() { return doa; }
    PsdLib* getPsd() { return psd; }
    WfLib* getWf() { return wf; }
    void setShowControllBox(bool act);
    void setShowGauge(bool act);
    void setShowDoa(bool act);
    void setShowPsd(bool act);
    void setShowWf(bool act);
    void normalizeVisible();
    void resetScales();

private slots:
    void onClickSync(bool act);

public slots:
    void onSystemParam(SystemParametersStruct param) override;
    void onBandwidthChanged(QString str, double num) override;
    void onResolutionChanged(QString str, double num) override;
    void onModulationChanged(QString str) override;
    void onIFFilterChanged(QString str) override;
    void onRFHeadChanged(QString str) override;
    void onFrequencyChanged(double num) override;


protected:
    void render() override;

signals:
};

#endif // DIRECTION_H
