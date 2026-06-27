#include "wflib.h"

WfLib::WfLib(GlobalVariables *gVar, QObject *parent)
    : BasePlots(gVar, parent)
{
    int len = getDataLen();
    waveData.resize(len);
    fftData.resize(len);
    for (int i = 0; i < len; ++i) {
        waveData[i] = 0.0;
        fftData[i] = 0.0;
    }
    calcFrequency();
}

WfLib::~WfLib()
{

}

void WfLib::renderStandalone(const ImVec2 &size)
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
        drawWaveform();
        drawFFT();
        ImPlot::EndPlot();
    }
}

void WfLib::setData(const QVector<double> &newRow, int newLen)
{

}

void WfLib::drawWaveform()
{
    if (waveData.isEmpty())
        return;
    ImPlot::SetNextLineStyle(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 1.5f);
    ImPlot::PlotLine("Waveform", waveData.constData(), waveData.size());
}

void WfLib::drawFFT()
{
    if (fftData.isEmpty())
        return;
    ImPlot::SetNextLineStyle(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), 1.5f);
    ImPlot::PlotLine("FFT", fftData.constData(), fftData.size());
}
