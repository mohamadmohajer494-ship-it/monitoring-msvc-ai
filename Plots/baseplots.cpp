#include "baseplots.h"

BasePlots::BasePlots(GlobalVariables *gVar, QObject *parent)
    : QObject{parent}
    , gVar(gVar)
{
    calcFrequency();
}

BasePlots::~BasePlots()
{

}

void BasePlots::setPlotName(QString txt)
{
    plotName = txt;
}

void BasePlots::setCenterFreq(double num)
{
    centerFreq = num;
    calcFrequency();
}

void BasePlots::setBandwidth(double num)
{
    bandwidth = num;
    calcFrequency();
}

void BasePlots::setResolution(int num)
{
    resolution = num;
    calcFrequency();
}

void BasePlots::setDataLen(int num)
{
    dataLen = num;
    calcFrequency();
}

void BasePlots::setStartFreq(double num)
{
    startFreq = num;
    calcFrequency();
}

void BasePlots::setEndFreq(double num)
{
    endFreq = num;
    calcFrequency();
}

void BasePlots::setRangeFreq(double min, double max)
{
    startFreq = min;
    endFreq = max;
    calcFrequency();
}

void BasePlots::setStartLevel(double num)
{
    startLevel = num;
}

void BasePlots::setEndLevel(double num)
{
    endLevel = num;
}

void BasePlots::setRangeLevel(double min, double max)
{
    startLevel = min;
    endLevel = max;
}

void BasePlots::setAxisXStart(double num)
{
    axisXStart = num;
}

void BasePlots::setAxisXEnd(double num)
{
    axisXEnd = num;
}

void BasePlots::setRangeAxisX(double min, double max)
{
    axisXStart = min;
    axisXEnd = max;
}

void BasePlots::setAxisYStart(double num)
{
    axisYStart = num;
}

void BasePlots::setAxisYEnd(double num)
{
    axisYEnd = num;
}

void BasePlots::setRangeAxisY(double min, double max)
{
    axisYStart = min;
    axisYEnd = max;
}

void BasePlots::setFreqList(QVector<double> list)
{
    freqList = list;
}

void BasePlots::setFreeze(bool act)
{
    isFreeze =  act;
}

void BasePlots::setShowGrid(bool act)
{
    isShowGrid = act;
}

void BasePlots::slotThemeChange()
{

}

void BasePlots::calcFrequency()
{
    if (dataLen <= 0) {
        freqList.clear();
        return;
    }
    double startFreqOffset = axisXStart - startFreq;
    double endFreqOffset = endFreq - axisXEnd;
    resolutionBandwidth = bandwidth / double(dataLen - 1);
    freqList.resize(dataLen);
    double sFreq = centerFreq - (bandwidth * 0.5);
    double eFreq = centerFreq + (bandwidth * 0.5);
    for (int var = 0; var < dataLen; ++var) {
        freqList[var] = sFreq + double(var) * resolutionBandwidth;
    }
    axisXStart = sFreq + startFreqOffset;
    axisXEnd = eFreq - endFreqOffset;
}

void BasePlots::drawAxes()
{
    ImPlotAxisFlags xflags = ImPlotAxisFlags_Lock
                             | ImPlotAxisFlags_NoHighlight
                             | (isShowGrid ? 0 : ImPlotAxisFlags_NoGridLines);
    ImPlotAxisFlags yflags = ImPlotAxisFlags_Lock
                             | ImPlotAxisFlags_NoHighlight
                             | (isShowGrid ? 0 : ImPlotAxisFlags_NoGridLines);
    ImPlot::SetupAxis(ImAxis_X1, nullptr, xflags);
    ImPlot::SetupAxis(ImAxis_Y1, nullptr, yflags);
    ImPlot::SetupAxisLimits(ImAxis_X1, axisXStart, axisXEnd, ImPlotCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, axisYStart, axisYEnd, ImPlotCond_Always);
}

void BasePlots::drawThreshold()
{
    if (!isShowThreshold) return;
    ImPlot::PushStyleColor(ImPlotCol_Line, clrThreshold);
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, SizeThreshold);
    ImPlot::PlotInfLines("", &thresholdY, 1, ImPlotInfLinesFlags_Horizontal);
    ImPlot::PopStyleColor();
    ImPlot::PopStyleVar();

    ImVec2 plotPos  = ImPlot::GetPlotPos();
    ImVec2 plotSize = ImPlot::GetPlotSize();
    ImVec2 mousePix = ImGui::GetMousePos();
    bool insidePlot = mousePix.x >= plotPos.x && mousePix.x <= plotPos.x + plotSize.x &&
                      mousePix.y >= plotPos.y && mousePix.y <= plotPos.y + plotSize.y;
    ImPlotPoint mousePlot = ImPlot::GetPlotMousePos();
    ImVec2 linePix = ImPlot::PlotToPixels(ImPlotPoint(mousePlot.x, thresholdY));
    float distPx = fabsf(mousePix.y - linePix.y);

    if (!thresholdDragging) {
        if (insidePlot && distPx <= thresholdGrabTolPx) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) thresholdDragging = true;
        }
    }

    if (thresholdDragging) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            double newY = mousePlot.y;
            newY = std::clamp(newY, (double)getAxisYStart(), (double)getAxisYEnd());
            if (newY != thresholdY) {
                thresholdY = newY;
                emit moveThreshold(thresholdY);
                // emit needsRepaint();
            }
        } else {
            thresholdDragging = false;
            emit changeThreshold(thresholdY);
        }
    }
}