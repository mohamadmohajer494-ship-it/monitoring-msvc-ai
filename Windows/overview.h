#ifndef OVERVIEW_H
#define OVERVIEW_H

#include <QWidget>
#include <QLabel>
#include "Windows/basewindows.h"
#include "Plots/psdlib.h"
#include "Plots/pchlib.h"
#include "Plots/wflib.h"


class Overview : public BaseWindows
{
    Q_OBJECT
public:
    explicit Overview(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Overview() override;

private:
    PsdLib *psd = nullptr;
    bool isShowPsd = true;
    float psdScale = 0.25f;
    float psdSize = 0.0;
    PchLib *pch = nullptr;
    bool isShowPch = true;
    float pchScale = 0.25f;
    float pchSize = 0.0;
    WfLib *wf = nullptr;
    bool isShowWf = true;
    float wfScale = 0.50f;
    float wfSize = 0.0;
    float splitterHeight = 1.5;

public:
    PsdLib* getPsd() { return psd; }
    PchLib* getPch() { return pch; }
    WfLib* getWf() { return wf; }
    void setShowPsd(bool act);
    void setShowPch(bool act);
    void setShowWf(bool act);
    void normalizeVisible();
    void resetScales();

private:
    void createPsd(GlobalVariables *gVar);
    void createPch(GlobalVariables *gVar);
    void createWf(GlobalVariables *gVar);

protected:
    void render() override;

signals:
};

#endif // OVERVIEW_H
