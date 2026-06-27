// DFGauge.cpp
#include "DFGauge.h"
#include <algorithm>
#include <cstdio>

static constexpr float PI = 3.14159265358979f;
static constexpr float DEG2RAD = PI / 180.0f;

DFGauge::DFGauge()
{
    m_rawData.fill(0.0f);
    m_smoothed.fill(0.0f);
    m_peak.fill(0.0f);
}

void DFGauge::setData(const std::array<float, 360>& data)
{
    m_rawData = data;
}

void DFGauge::setData(int degree, float value)
{
    if (degree >= 0 && degree < 360)
        m_rawData[degree] = std::clamp(value, 0.0f, 50.0f);
}

void DFGauge::setHeading(float headingDeg)
{
    m_heading = norm(headingDeg);
}

void DFGauge::setRotationOffset(float offsetDeg)
{
    m_rotationOffset = norm(offsetDeg);
}

void DFGauge::update(float deltaTime)
{
    float alpha = m_config.smoothingFactor;
    for (int i = 0; i < 360; ++i)
        m_smoothed[i] += alpha * (m_rawData[i] - m_smoothed[i]);
    for (int i = 0; i < 360; ++i)
    {
        if (m_smoothed[i] > m_peak[i])
            m_peak[i] = m_smoothed[i];
        else
            m_peak[i] = std::max(0.0f, m_peak[i] - m_config.peakDecayRate * deltaTime);
    }
    updatePeak();
}

void DFGauge::render()
{
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float side = std::min(avail.x, avail.y);
    if (side < 60.0f) return;
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImVec2 center(cursor.x + avail.x * 0.5f, cursor.y + side * 0.5f);
    float radius = side * 0.40f;
    ImGui::Dummy(ImVec2(avail.x, side));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    drawBackground(dl, center, radius);
    drawGrid(dl, center, radius);
    drawHeatmapRing(dl, center, radius);
    if (m_config.showPeakMarker) drawPeakMarker(dl, center, radius);
    if (m_config.showArrow) drawBearingArrow(dl, center, radius);
    drawLabels(dl, center, radius);
    if (m_config.showInfoPanel) drawInfoPanel(dl, center, radius);
}

void DFGauge::drawBackground(ImDrawList* dl, ImVec2 center, float radius)
{
    dl->AddCircleFilled(center, radius * 1.08f, m_theme.surface, 128);
    dl->AddCircleFilled(center, radius * 0.98f, m_theme.background, 128);
    ImU32 ringCol = IM_COL32(55, 93, 118, 40);
    dl->AddCircle(center, radius * 0.25f, ringCol, 96, 0.8f);
    dl->AddCircle(center, radius * 0.50f, ringCol, 96, 0.8f);
    dl->AddCircle(center, radius * 0.75f, ringCol, 96, 0.8f);
    dl->AddCircle(center, radius * 1.08f, m_theme.glow, 128, 1.5f);
}

void DFGauge::drawGrid(ImDrawList* dl, ImVec2 center, float radius)
{
    float outerR = radius * 1.0f;
    ImU32 lineCol = IM_COL32(55, 93, 118, 90);
    ImU32 dashCol = IM_COL32(55, 93, 118, 50);
    const float cardinals[] = {0.0f, 90.0f, 180.0f, 270.0f};
    for (float a : cardinals)
    {
        float da = dispAngle(a);
        ImVec2 p = polar(center, outerR, da);
        dl->AddLine(center, p, lineCol, 1.0f);
    }
    const float intercard[] = {45.0f, 135.0f, 225.0f, 315.0f};
    for (float a : intercard)
    {
        float da = dispAngle(a);
        int segs = 14;
        for (int s = 0; s < segs; s += 2)
        {
            float r0 = outerR * (float)s / segs;
            float r1 = outerR * (float)(s + 1) / segs;
            dl->AddLine(polar(center, r0, da), polar(center, r1, da), dashCol, 0.8f);
        }
    }
    float tickIn  = radius * 1.00f;
    float tickOut = radius * 1.05f;
    for (int deg = 0; deg < 360; deg += 10)
    {
        float da = dispAngle((float)deg);
        float thick = (deg % 30 == 0) ? 1.4f : 0.7f;
        float tIn = (deg % 30 == 0) ? tickIn - radius * 0.02f : tickIn;
        dl->AddLine(polar(center, tIn, da), polar(center, tickOut, da),
                    IM_COL32(150, 160, 170, 160), thick);
    }
}

void DFGauge::drawHeatmapRing(ImDrawList* dl, ImVec2 center, float radius)
{
    float outerR = radius * 0.98f;
    float innerR = outerR * (1.0f - m_config.ringThickness);
    for (int i = 0; i < 360; ++i)
    {
        float value = m_smoothed[i];
        if (value < m_config.threshold) continue;
        float da0 = dispAngle((float)i) - 0.6f;
        float da1 = dispAngle((float)i) + 0.6f;
        float rad0 = (da0 - 90.0f) * DEG2RAD;
        float rad1 = (da1 - 90.0f) * DEG2RAD;
        ImVec2 p1(center.x + innerR * cosf(rad0), center.y + innerR * sinf(rad0));
        ImVec2 p2(center.x + outerR * cosf(rad0), center.y + outerR * sinf(rad0));
        ImVec2 p3(center.x + outerR * cosf(rad1), center.y + outerR * sinf(rad1));
        ImVec2 p4(center.x + innerR * cosf(rad1), center.y + innerR * sinf(rad1));
        ImU32 col = valueToColor(value);
        dl->AddQuadFilled(p1, p2, p3, p4, col);
    }
}

void DFGauge::drawBearingArrow(ImDrawList* dl, ImVec2 center, float radius)
{
    float da = dispAngle(m_heading);
    float len = radius * 0.82f;
    ImVec2 tip = polar(center, len, da);
    dl->AddLine(center, tip, IM_COL32(26, 246, 122, 60), 6.0f);
    dl->AddLine(center, tip, m_theme.accent, 2.5f);
    float headLen = radius * 0.08f;
    float headW   = radius * 0.03f;
    float rad     = (da - 90.0f) * DEG2RAD;
    float perpX   = -sinf(rad) * headW;
    float perpY   = cosf(rad) * headW;
    ImVec2 back = polar(center, len - headLen, da);
    ImVec2 left(back.x + perpX, back.y + perpY);
    ImVec2 right(back.x - perpX, back.y - perpY);
    dl->AddTriangleFilled(tip, left, right, m_theme.accent);
    dl->AddCircleFilled(center, 4.0f, m_theme.accent, 16);
    dl->AddCircle(center, 4.0f, IM_COL32(26, 246, 122, 100), 16, 2.0f);
}

void DFGauge::drawPeakMarker(ImDrawList* dl, ImVec2 center, float radius)
{
    if (m_peakValue < m_config.threshold) return;
    float da = dispAngle((float)m_peakBearing);
    float heatmapOuter = radius * 1.08f;
    float heatmapInner = radius * 0.84f;
    float triHeight = 8.0f;
    float w = 5.0f;
    ImVec2 tip = polar(center, heatmapOuter + triHeight, da);
    ImVec2 base = polar(center, heatmapOuter, da);
    float rad = (da - 90.0f) * DEG2RAD;
    ImVec2 l(base.x - sinf(rad) * w, base.y + cosf(rad) * w);
    ImVec2 r(base.x + sinf(rad) * w, base.y - cosf(rad) * w);
    dl->AddTriangleFilled(tip, l, r, m_theme.danger);
    ImVec2 lineEnd = polar(center, heatmapInner, da);
    dl->AddLine(base, lineEnd, m_theme.danger, 2.0f);
}


void DFGauge::drawLabels(ImDrawList* dl, ImVec2 center, float radius)
{
    float numR = radius * 1.12f;
    struct CL { float angle; const char* text; };
    CL cards[] = {
        {0.0f,   "N"},
        {90.0f,  "E"},
        {180.0f, "S"},
        {270.0f, "W"}
    };
    ImFont* font = ImGui::GetFont();
    // float bigSize = ImGui::GetFontSize() * 2.0f;
    float bigSize = radius * 0.12f;
    bigSize = std::clamp(bigSize, 6.0f, 20.0f);
    for (auto& c : cards)
    {
        float da = dispAngle(c.angle);
        ImVec2 pos = polar(center, numR, da);
        ImVec2 ts = font->CalcTextSizeA(bigSize, FLT_MAX, 0.0f, c.text);
        pos.x -= ts.x * 0.5f;
        pos.y -= ts.y * 0.5f;
        dl->AddText(font, bigSize, pos, m_theme.primary, c.text);
    }
    for (int deg = 0; deg < 360; deg += 30)
    {
        if (deg == 0 || deg == 90 || deg == 180 || deg == 270) continue;
        float da = dispAngle((float)deg);
        ImVec2 pos = polar(center, numR, da);
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", deg);
        ImVec2 ts = ImGui::CalcTextSize(buf);
        pos.x -= ts.x * 0.5f;
        pos.y -= ts.y * 0.5f;
        dl->AddText(pos, IM_COL32(150, 160, 170, 180), buf);
    }
}


void DFGauge::drawInfoPanel(ImDrawList* dl, ImVec2 center, float radius)
{
    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();
    ImU32 borderColor = m_theme.primary;
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%03.0f\xC2\xB0", m_heading);
        ImVec2 ts = font->CalcTextSizeA(fontSize * 1.4f, FLT_MAX, 0.0f, buf);
        float boxW = ts.x + 16.0f;
        float boxH = ts.y + 10.0f;
        float boxX = center.x - boxW * 0.5f;
        float boxY = center.y - radius * 0.35f;
        ImVec2 tl(boxX, boxY);
        ImVec2 br(boxX + boxW, boxY + boxH);
        dl->AddRectFilled(tl, br, IM_COL32(20, 40, 30, 200), 4.0f);
        dl->AddRect(tl, br, borderColor, 4.0f, 0, 1.5f);
        ImVec2 textPos(boxX + (boxW - ts.x) * 0.5f, boxY + (boxH - ts.y) * 0.5f);
        dl->AddText(font, fontSize * 1.4f, textPos, m_theme.accent, buf);
    }
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%03d\xC2\xB0", m_peakBearing);
        ImVec2 ts = font->CalcTextSizeA(fontSize * 1.4f, FLT_MAX, 0.0f, buf);
        float boxW = ts.x + 16.0f;
        float boxH = ts.y + 10.0f;
        float boxX = center.x - boxW * 0.5f;
        float boxY = center.y - radius * 0.35f - boxH - 8.0f;
        ImVec2 tl(boxX, boxY);
        ImVec2 br(boxX + boxW, boxY + boxH);
        dl->AddRectFilled(tl, br, IM_COL32(40, 20, 20, 200), 4.0f);
        dl->AddRect(tl, br, borderColor, 4.0f, 0, 1.5f);
        ImVec2 textPos(boxX + (boxW - ts.x) * 0.5f, boxY + (boxH - ts.y) * 0.5f);
        dl->AddText(font, fontSize * 1.4f, textPos, m_theme.danger, buf);
    }
    {
        bool trackUp = (m_config.rotationMode == RotationMode::TrackUp);
        float lowestBoxY = center.y - radius * 0.35f;
        float fontSize = ImGui::GetFontSize();
        float boxH = fontSize * 1.4f + 10.0f;
        float belowBoxesY = lowestBoxY + boxH * 2 + 8.0f + 12.0f;
        ImVec2 labelSize = ImGui::CalcTextSize("TrackUp");
        float checkboxSize = ImGui::GetFrameHeight();
        float totalW = checkboxSize + ImGui::GetStyle().ItemInnerSpacing.x + labelSize.x;
        ImGui::SetCursorScreenPos(ImVec2(center.x - totalW * 0.5f, belowBoxesY));
        ImGui::PushStyleColor(ImGuiCol_CheckMark,      ImVec4(107/255.f, 176/255.f, 226/255.f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImVec4( 31/255.f,  51/255.f,  64/255.f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4( 55/255.f,  93/255.f, 118/255.f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImVec4( 55/255.f,  93/255.f, 118/255.f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(107/255.f, 176/255.f, 226/255.f, 0.9f));
        if (ImGui::Checkbox("TrackUp", &trackUp))
            m_config.rotationMode = trackUp ? RotationMode::TrackUp : RotationMode::NorthUp;
        ImGui::PopStyleColor(5);
    }
}

ImU32 DFGauge::valueToColor(float value) const
{
    float t = std::clamp(value / 50.0f, 0.0f, 1.0f);
    if (!m_config.useHeatmap)
    {
        int v = (int)(t * 255.0f);
        return IM_COL32(v, v, v, (int)(t * 240.0f + 15.0f));
    }
    struct Stop { float pos; float r, g, b, a; };
    static const Stop stops[] = {
                                 {0.00f, 0.00f, 0.00f, 0.30f, 0.3f},
                                 {0.10f, 0.00f, 0.10f, 0.60f, 0.7f},
                                 {0.25f, 0.00f, 0.30f, 0.90f, 0.85f},
                                 {0.40f, 0.00f, 0.70f, 0.80f, 0.90f},
                                 {0.55f, 0.10f, 0.85f, 0.20f, 0.92f},
                                 {0.70f, 0.85f, 0.85f, 0.00f, 0.95f},
                                 {0.82f, 1.00f, 0.60f, 0.00f, 0.97f},
                                 {0.92f, 1.00f, 0.85f, 0.70f, 1.0f},
                                 {1.00f, 1.00f, 1.00f, 1.00f, 1.0f},
                                 };
    constexpr int N = 9;
    int idx = 0;
    for (int i = 0; i < N - 1; ++i)
    {
        if (t >= stops[i].pos && t <= stops[i + 1].pos) { idx = i; break; }
    }
    float seg = (t - stops[idx].pos) / (stops[idx + 1].pos - stops[idx].pos);
    seg = std::clamp(seg, 0.0f, 1.0f);
    float r = stops[idx].r + (stops[idx + 1].r - stops[idx].r) * seg;
    float g = stops[idx].g + (stops[idx + 1].g - stops[idx].g) * seg;
    float b = stops[idx].b + (stops[idx + 1].b - stops[idx].b) * seg;
    float a = stops[idx].a + (stops[idx + 1].a - stops[idx].a) * seg;
    return IM_COL32((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
}

ImVec2 DFGauge::polar(ImVec2 center, float r, float angleDeg) const
{
    float rad = (angleDeg - 90.0f) * DEG2RAD;
    return ImVec2(center.x + r * cosf(rad), center.y + r * sinf(rad));
}

float DFGauge::norm(float deg) const
{
    deg = fmodf(deg, 360.0f);
    if (deg < 0.0f) deg += 360.0f;
    return deg;
}

float DFGauge::dispAngle(float dataDeg) const
{
    float offset = m_rotationOffset;
    if (m_config.rotationMode == RotationMode::TrackUp)
        offset -= m_heading;
    return norm(dataDeg + offset);
}

void DFGauge::updatePeak()
{
    m_peakValue = 0.0f;
    m_peakBearing = 0;
    for (int i = 0; i < 360; ++i)
    {
        if (m_smoothed[i] > m_peakValue)
        {
            m_peakValue = m_smoothed[i];
            m_peakBearing = i;
        }
    }
}
