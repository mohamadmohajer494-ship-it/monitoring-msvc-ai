#include "pchlib.h"

PchLib::PchLib(GlobalVariables *gVar, QObject *parent)
    : BasePlots(gVar, parent)
{
    int len = getDataLen();
    occHeat.resize(len * occYBins);
    lastPowerBuf.resize(len);
    std::fill(occHeat.begin(), occHeat.end(), 0);
    std::fill(lastPowerBuf.begin(), lastPowerBuf.end(), -180.0);
}

PchLib::~PchLib()
{

}

void PchLib::renderStandalone(const ImVec2 &size)
{
    if (!QOpenGLContext::currentContext())
        return;
    if (!ImGui::GetCurrentContext() || !ImPlot::GetCurrentContext())
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
    if (ImPlot::BeginPlot("##PCH", s, flags)) {
        drawAxes();
        drawPolychrome();
        drawLiveTrace();
        ImPlot::EndPlot();
    }
    else {

    }
    ImPlot::PopStyleColor(3);
}

void PchLib::setData(const quint8 *occ, int yBins, int newLen, const double *lastPower)
{
    if (getFreeze() || newLen <= 0 || yBins <= 1)
        return;
    if (newLen != getDataLen()) {
        setDataLen(newLen);
        lastPowerBuf.resize(newLen);
        occHeat.resize(newLen * yBins);
    }
    if (occYBins != yBins) {
        occYBins = yBins;
        occHeat.resize(getDataLen() * occYBins);
    }
    if (newLen != getDataLen()) {
        setDataLen(newLen);
        lastPowerBuf.resize(newLen);
        occHeat.resize(newLen * yBins);
    }
    if (occYBins != yBins) {
        occYBins = yBins;
        occHeat.resize(getDataLen() * occYBins);
    }
    if (occ)
        memcpy(occHeat.data(), occ, size_t(getDataLen()) * occYBins);
    if (lastPower)
        memcpy(lastPowerBuf.data(), lastPower, sizeof(double) * getDataLen());
}

void PchLib::drawPolychrome()
{
    if (occHeat.isEmpty() || getDataLen() <= 1 || occYBins <= 1)
        return;
    ImPlot::PushPlotClipRect();
    ImDrawList* dl = ImPlot::GetPlotDrawList();
    ImVec2 pos = ImPlot::GetPlotPos();
    ImVec2 size = ImPlot::GetPlotSize();
    int pixX = std::max(1, int(size.x));
    int stride = std::max(1, int(std::ceil(double(getDataLen()) / pixX)));
    double xSpan = getAxisXEnd() - getAxisXStart();
    double ySpan = getEndLevel() - getStartLevel();
    double rbw   = xSpan / getDataLen();
    double ybw   = ySpan / occYBins;
    auto toPx = [&](double fx, double fy){
        return ImVec2(
            pos.x + float((fx - getAxisXStart()) * size.x / xSpan),
            pos.y + size.y - float((fy - getAxisYStart()) * size.y / (getAxisYEnd() - getAxisYStart()))
            );
    };
    int denom = occMax;
    if (occAutoScale) {
        quint8 maxv = 0;
        int N = getDataLen() * occYBins;
        for (int i = 0; i < N; ++i)
            maxv = std::max(maxv, occHeat[i]);
        if (maxv > 0)
            denom = std::max(1, int(lround(maxv * occAutoAlpha)));
    }
    if (isShowColorMap) {
        for (int i0 = 0; i0 < getDataLen(); i0 += stride) {
            int i1 = std::min(i0 + stride - 1, getDataLen() - 1);
            double x0 = getAxisXStart() + i0 * rbw;
            double x1 = getAxisXStart() + (i1 + 1) * rbw;
            for (int b = 0; b < occYBins; ++b) {
                quint8 vmax = 0;
                for (int ix = i0; ix <= i1; ++ix)
                    vmax = std::max(vmax, occHeat[ix * occYBins + b]);
                if (!vmax)
                    continue;
                quint8 adj = vmax;
                if (denom != occMax) {
                    float scaled = vmax * float(occMax) / denom;
                    adj = quint8(std::clamp(int(scaled), 0, 255));
                }
                double y0 = getStartLevel() + b * ybw;
                double y1 = y0 + ybw;
                dl->AddRectFilled(toPx(x0,y0), toPx(x1,y1),
                                  colorForOcc(adj));
            }
        }
    }
    ImPlot::PopPlotClipRect();
}

void PchLib::drawLiveTrace()
{
    if (!liveTrace || getDataLen() <= 1)
        return;
    int pixX = std::max(1, int(ImPlot::GetPlotSize().x));
    int stride = std::max(1, int(std::ceil(double(getDataLen()) / pixX)));
    static thread_local QVector<double> xs, ys;
    int outN = (getDataLen() + stride - 1) / stride;
    xs.resize(outN);
    ys.resize(outN);
    for (int i = 0, k = 0; i < getDataLen(); i += stride, ++k) {
        xs[k] = getFreqList()[i];
        ys[k] = lastPowerBuf[i];
    }
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, liveWidth);
    ImPlot::PlotLine("Live", xs.data(), ys.data(), outN);
    ImPlot::PopStyleVar();
}

ImU32 PchLib::colorForOcc(quint8 v) const
{
    return colorForOccTheme(v, paletteKind);
}

ImU32 PchLib::colorForOccTheme(quint8 v, EnumPCHTheme th) const
{
    if (v == 0)
        return IM_COL32(0,0,0,0);
    float n = (float)v / (float)occMax;
    n = std::clamp(n, 0.0f, 1.0f);
    n = powf(n, occGamma);
    n = std::clamp(n, 0.0f, 1.0f);
    auto lerp = [](const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(a.x + (b.x - a.x) * t,
                      a.y + (b.y - a.y) * t,
                      a.z + (b.z - a.z) * t,
                      a.w + (b.w - a.w) * t);
    };
    auto sampleStops = [&](const ImVec4* cols, const float* pos, int count, float t) -> ImVec4 {
        if (count <= 0) return ImVec4(0,0,0,0);
        if (t <= pos[0]) return cols[0];
        if (t >= pos[count-1]) return cols[count-1];
        for (int i=0;i<count-1;++i) {
            if (t >= pos[i] && t <= pos[i+1]) {
                float dt = (t - pos[i]) / (pos[i+1] - pos[i] + 1e-12f);
                return lerp(cols[i], cols[i+1], dt);
            }
        }
        return cols[count-1];
    };
    ImVec4 c(0,0,0,0);
    switch (th)
    {
    case EnumPCHTheme::Current: {
        static const ImVec4 cols[] = {
            ImVec4(0.05f, 0.15f, 0.60f, 0.95f),
            ImVec4(0.00f, 0.80f, 0.85f, 0.98f),
            ImVec4(0.00f, 0.85f, 0.25f, 1.00f),
            ImVec4(0.98f, 0.90f, 0.10f, 1.00f),
            ImVec4(1.00f, 0.25f, 0.10f, 1.00f)
        };
        static const float pos[] = { 0.00f, 0.28f, 0.50f, 0.75f, 1.00f };
        c = sampleStops(cols, pos, 5, n);
        break;
    }
    case EnumPCHTheme::Viridis: {
        static const ImVec4 cols[] = {
            ImVec4(0.27f, 0.00f, 0.33f, 0.95f),
            ImVec4(0.23f, 0.32f, 0.55f, 0.98f),
            ImVec4(0.13f, 0.57f, 0.55f, 1.00f),
            ImVec4(0.48f, 0.82f, 0.32f, 1.00f),
            ImVec4(0.99f, 0.91f, 0.14f, 1.00f)
        };
        static const float pos[] = { 0.00f, 0.30f, 0.55f, 0.78f, 1.00f };
        c = sampleStops(cols, pos, 5, n);
        break;
    }
    case EnumPCHTheme::Inferno: {
        static const ImVec4 cols[] = {
            ImVec4(0.00f, 0.00f, 0.00f, 0.95f),
            ImVec4(0.20f, 0.00f, 0.35f, 0.98f),
            ImVec4(0.75f, 0.10f, 0.15f, 1.00f),
            ImVec4(1.00f, 0.60f, 0.05f, 1.00f),
            ImVec4(1.00f, 0.95f, 0.65f, 1.00f)
        };
        static const float pos[] = { 0.00f, 0.25f, 0.55f, 0.80f, 1.00f };
        c = sampleStops(cols, pos, 5, n);
        break;
    }
    case EnumPCHTheme::Turbo: {
        static const ImVec4 cols[] = {
            ImVec4(0.000f, 0.000f, 0.125f, 0.95f),
            ImVec4(0.000f, 0.000f, 0.627f, 0.98f),
            ImVec4(0.000f, 0.627f, 1.000f, 1.00f),
            ImVec4(1.000f, 1.000f, 0.000f, 1.00f),
            ImVec4(1.000f, 0.000f, 0.000f, 1.00f)
        };
        static const float pos[] = { 0.00f, 0.25f, 0.50f, 0.75f, 1.00f };
        c = sampleStops(cols, pos, 5, n);
        break;
    }
    case EnumPCHTheme::IceFire: {
        static const ImVec4 cols[] = {
            ImVec4(0.05f, 0.20f, 0.45f, 0.95f),
            ImVec4(0.10f, 0.65f, 0.95f, 0.98f),
            ImVec4(0.95f, 0.95f, 0.95f, 1.00f),
            ImVec4(1.00f, 0.55f, 0.10f, 1.00f),
            ImVec4(0.70f, 0.05f, 0.05f, 1.00f)
        };
        static const float pos[] = { 0.00f, 0.30f, 0.52f, 0.78f, 1.00f };
        c = sampleStops(cols, pos, 5, n);
        break;
    }
    case EnumPCHTheme::Gray: {
        float g = n;
        c = ImVec4(g, g, g, 0.95f);
        break;
    }
    }
    return ImGui::ColorConvertFloat4ToU32(c);
}

void PchLib::fillPalette256(EnumPCHTheme kind, quint32 out[]) const
{
    for (int i = 0; i < 256; ++i) {
        ImU32 c = colorForOccTheme((quint8)i, kind);
        out[i] = (quint32)c;
    }
}
