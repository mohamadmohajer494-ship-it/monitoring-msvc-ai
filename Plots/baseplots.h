#ifndef BASEPLOTS_H
#define BASEPLOTS_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QImage>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <cmath>
#include <QColor>
#include <algorithm>
#include <limits>
#include <QRectF>
#include <QHash>
#include <cstring>
#include "Includes/globalvariables.h"
#include "implot.h"
#include "imgui.h"

class BasePlots : public QObject
{
    Q_OBJECT
public:
    explicit BasePlots(GlobalVariables *gVar, QObject *parent = nullptr);
    ~BasePlots() override;

private:
    GlobalVariables* gVar;
    QString plotName = "PSD";
    double centerFreq = 300.0;
    double bandwidth = 80.0;
    int resolution = 2048;
    int dataLen = 820;
    double resolutionBandwidth = bandwidth / dataLen;
    double startFreq = 260.0;
    double endFreq = 340.0;
    double startLevel = -180.0;
    double endLevel = 40.0;
    double axisXStart = 260.0;
    double axisXEnd = 340.0;
    double axisYStart = -160.0;
    double axisYEnd = 20.0;
    QVector<double> freqList;
    bool isFreeze = false;
    bool isShowGrid = false;
    //--- Threshold ---
    double thresholdY = -100.0;
    bool thresholdDragging = false;
    bool isShowThreshold = true;
    float SizeThreshold = 2.0f;
    ImVec4 clrThreshold = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    float thresholdGrabTolPx   = 6.0f;

public slots:
    virtual void renderStandalone(const ImVec2& size = ImVec2(-1,-1)) = 0;
    QString getPlotName() { return plotName; }
    void setPlotName(QString txt);
    double getCenterFreq() { return centerFreq; }
    void setCenterFreq(double num);
    double getBandwidth() { return bandwidth; }
    void setBandwidth(double num);
    int getResolution() { return resolution; }
    void setResolution(int num);
    int getDataLen() { return dataLen; }
    void setDataLen(int num);
    double getStartFreq() { return startFreq; }
    void setStartFreq(double num);
    double getEndFreq() { return endFreq; }
    void setEndFreq(double num);
    void setRangeFreq(double min, double max);
    double getStartLevel() { return startLevel; }
    void setStartLevel(double num);
    double getEndLevel() { return endLevel; }
    void setEndLevel(double num);
    void setRangeLevel(double min, double max);
    double getAxisXStart() { return axisXStart; }
    void setAxisXStart(double num);
    double getAxisXEnd() { return axisXEnd; }
    void setAxisXEnd(double num);
    void setRangeAxisX(double min, double max);
    double getAxisYStart() { return axisYStart; }
    void setAxisYStart(double num);
    double getAxisYEnd() { return axisYEnd; }
    void setAxisYEnd(double num);
    void setRangeAxisY(double min, double max);
    QVector<double> getFreqList() { return freqList; }
    void setFreqList(QVector<double> list);
    bool getFreeze() { return isFreeze; }
    void setFreeze(bool act);
    bool getShowGrid() { return isShowGrid; }
    void setShowGrid(bool act);
    virtual void slotThemeChange();

private slots:

protected:
    void calcFrequency();
    void drawAxes();
    void drawThreshold();

signals:
    void needsRepaint();
    void moveThreshold (int num);
    void changeThreshold (int num);
};

#endif // BASEPLOTS_H
