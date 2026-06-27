#include "psdlib.h"

PsdLib::PsdLib(GlobalVariables* gVar, QObject *parent)
    : BasePlots(gVar, parent)
{
    restartDatas(getDataLen());
}

PsdLib::~PsdLib()
{

}

void PsdLib::renderStandalone(const ImVec2 &size)
{
    if (!QOpenGLContext::currentContext())
        return;
    if (!ImPlot::GetCurrentContext() || !ImGui::GetCurrentContext())
        return;
    ImVec2 s = size;
    if (s.x <= 0) s.x = -1;
    if (s.y <= 0) s.y = 240;
    ImPlotFlags flags = ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText;
    ImPlot::PushStyleColor(ImPlotCol_PlotBg,     ImVec4(0,0,0,0));
    ImPlot::PushStyleColor(ImPlotCol_FrameBg,    ImVec4(0,0,0,0));
    ImPlot::PushStyleColor(ImPlotCol_PlotBorder, ImVec4(0,0,0,0));
    ImPlot::SetNextAxesLimits(getAxisXStart(), getAxisXEnd(),
                              getAxisYStart(), getAxisYEnd(),
                              ImGuiCond_Always);
    if (ImPlot::BeginPlot("##PSD", s, flags)) {
        drawAxes();
        drowMax();
        drowPsd();
        drowPrd();
        drowAvr();
        drowDif();
        drawThreshold();
        ImPlot::EndPlot();
    } else {

    }
    ImPlot::PopStyleColor(3);
}

void PsdLib::setPsdData(const QVector<double> &data)
{
    if (getFreeze() || data.isEmpty())
        return;
    if (data.size() != getDataLen()) {
        setDataLen(data.size());
        restartDatas(getDataLen());
    }
    psdData = data;
}

void PsdLib::setMaxData(const QVector<double> &data)
{
    if (getFreeze() || data.isEmpty())
        return;
    if (data.size() != getDataLen()) {
        setDataLen(data.size());
        restartDatas(getDataLen());
    }
    maxData = data;
}

void PsdLib::setAvrData(const QVector<double> &data)
{
    if (getFreeze() || data.isEmpty())
        return;
    if (data.size() != getDataLen()) {
        setDataLen(data.size());
        restartDatas(getDataLen());
    }
    avrData = data;
}

void PsdLib::setPrdData(const QVector<double> &data)
{
    if (getFreeze() || data.isEmpty())
        return;
    if (data.size() != getDataLen()) {
        setDataLen(data.size());
        restartDatas(getDataLen());
    }
    prdData = data;
}

void PsdLib::setDifData(const QVector<double> &data)
{
    if (getFreeze() || data.isEmpty())
        return;
    if (data.size() != getDataLen()) {
        setDataLen(data.size());
        restartDatas(getDataLen());
    }
    difData = data;
}

void PsdLib::setAllData(const QVector<double> &psd,
                        const QVector<double> &max,
                        const QVector<double> &avr,
                        const QVector<double> &prd,
                        const QVector<double> &dif)
{
    if (getFreeze() || psd.isEmpty() || max.isEmpty() || avr.isEmpty() || prd.isEmpty() || dif.isEmpty())
        return;
    if (psd.size() != max.size() != avr.size() != prd.size() != dif.size())
        return;
    if (psd.size() != getDataLen()) {
        setDataLen(psd.size());
        restartDatas(getDataLen());
    }
    psdData = psd;
    maxData = max;
    avrData = avr;
    prdData = prd;
    difData = dif;
}

void PsdLib::drowPsd()
{
    if (getDataLen() <= 1 || !isShowPsd)
        return;
    ImPlot::PushStyleColor(ImPlotCol_Fill, clrPsd);
    ImPlot::PushStyleColor(ImPlotCol_Line, clrPsd);
    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.35f);
    ImPlot::PushPlotClipRect();
    double barWidth = (getFreqList()[1] - getFreqList()[0]) * 0.8;
    ImPlot::PlotBars("PSD", getFreqList().data(), psdData.data(), getDataLen(), barWidth, 0);
    ImPlot::PopPlotClipRect();
    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor(2);
}

void PsdLib::drowMax()
{
    if (getDataLen() <= 1 || !isShowMax) return;
    ImPlot::PushStyleColor(ImPlotCol_Fill, clrMax);
    ImPlot::PushStyleColor(ImPlotCol_Line, clrMax);
    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.35f);
    ImPlot::PushPlotClipRect();
    ImPlot::PlotShaded("MaxHold", getFreqList().data(), maxData.data(), getDataLen(), -160.0);
    ImPlot::PopPlotClipRect();
    ImPlot::PlotLine("MaxHold", getFreqList().data(), maxData.data(), getDataLen());
    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor(2);
}

void PsdLib::drowAvr()
{
    if (getDataLen() <= 1 || !isShowAvr) return;
    ImPlot::PushStyleColor(ImPlotCol_Line, clrAvr);
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, sizeAvr);
    ImPlot::PlotLine("Average", getFreqList().data(), avrData.data(), getDataLen());
    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor();
}

void PsdLib::drowPrd()
{
    if (getDataLen() <= 1 || !isShowPrd) return;
    ImPlot::PushStyleColor(ImPlotCol_Line, clrPrd);
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, sizePrd);
    ImPlot::PlotLine("Period Buffer", getFreqList().data(), prdData.data(), getDataLen());
    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor();
}

void PsdLib::drowDif()
{
    if (getDataLen() <= 1 || !isShowDif) return;
    ImPlot::PushStyleColor(ImPlotCol_Line, clrDif);
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, sizeDif);
    ImPlot::PlotLine("Difference", getFreqList().data(), difData.data(), getDataLen());
    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor();
}

void PsdLib::restartDatas(int size)
{
    psdData.fill(-180, size);
    maxData.fill(-180, size);
    avrData.fill(-180, size);
    prdData.fill(-180, size);
    difData.fill(-180, size);
}