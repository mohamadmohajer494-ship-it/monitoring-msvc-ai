// DFGauge.h
#pragma once

#include "imgui.h"
#include <array>
#include <cmath>
#include <string>

class DFGauge
{
public:
    enum class RotationMode { NorthUp, TrackUp };

    struct Theme
    {
        ImU32 background    = IM_COL32(24, 38, 50, 255);    // #182632
        ImU32 surface       = IM_COL32(31, 51, 64, 255);    // #1f3340
        ImU32 glow          = IM_COL32(55, 93, 118, 255);   // #375d76
        ImU32 primary       = IM_COL32(225, 225, 225, 255); // #e1e1e1
        ImU32 secondary     = IM_COL32(3, 218, 198, 255);   // #03DAC6
        ImU32 highlight     = IM_COL32(107, 176, 226, 255); // #6bb0e2
        ImU32 accent        = IM_COL32(26, 246, 122, 255);  // #1af67a
        ImU32 danger        = IM_COL32(255, 60, 60, 255);   // red marker
        ImU32 warning       = IM_COL32(255, 255, 0, 255);   // #ffff00
    };

    struct Config
    {
        bool         useHeatmap      = true;
        float        threshold       = 2.0f;
        float        smoothingFactor = 0.25f;
        float        peakDecayRate   = 0.4f;
        float        ringThickness   = 0.14f;
        RotationMode rotationMode    = RotationMode::NorthUp;
        bool         showInfoPanel   = true;
        bool         showPeakMarker  = true;
        bool         showArrow       = true;
    };

    DFGauge();
    ~DFGauge() = default;

    void setData(const std::array<float, 360>& data);
    void setData(int degree, float value);
    void setHeading(float headingDeg);
    void setRotationOffset(float offsetDeg);
    void update(float deltaTime);
    void render();

    Config& config() { return m_config; }
    const Config& config() const { return m_config; }
    Theme& theme() { return m_theme; }
    const Theme& theme() const { return m_theme; }

    float getHeading() const { return m_heading; }
    int   getPeakBearing() const { return m_peakBearing; }
    float getPeakValue() const { return m_peakValue; }

private:
    void drawBackground(ImDrawList* dl, ImVec2 center, float radius);
    void drawGrid(ImDrawList* dl, ImVec2 center, float radius);
    void drawHeatmapRing(ImDrawList* dl, ImVec2 center, float radius);
    void drawBearingArrow(ImDrawList* dl, ImVec2 center, float radius);
    void drawPeakMarker(ImDrawList* dl, ImVec2 center, float radius);
    void drawLabels(ImDrawList* dl, ImVec2 center, float radius);
    void drawInfoPanel(ImDrawList* dl, ImVec2 center, float radius);

    ImU32  valueToColor(float value) const;
    ImVec2 polar(ImVec2 center, float r, float angleDeg) const;
    float  norm(float deg) const;
    float  dispAngle(float dataDeg) const;
    void   updatePeak();

    Config  m_config;
    Theme   m_theme;
    float   m_heading        = 0.0f;
    float   m_rotationOffset = 0.0f;

    std::array<float, 360> m_rawData{};
    std::array<float, 360> m_smoothed{};
    std::array<float, 360> m_peak{};

    int   m_peakBearing = 0;
    float m_peakValue   = 0.0f;
};
