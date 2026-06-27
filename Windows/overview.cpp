#include "overview.h"

Overview::Overview(GlobalVariables *gVar, QWidget *parent)
    : BaseWindows(gVar, "Overview", parent)
{
    createPsd(gVar);
    createPch(gVar);
    createWf(gVar);
    resetScales();
    attachRenderer();
}

Overview::~Overview()
{

}

void Overview::setShowPsd(bool act)
{
    isShowPsd = act;
    normalizeVisible();
}

void Overview::setShowPch(bool act)
{
    isShowPch = act;
    normalizeVisible();
}

void Overview::setShowWf(bool act)
{
    isShowWf = act;
    normalizeVisible();
}

void Overview::normalizeVisible()
{
    float total = 0.0f;
    if (isShowPsd) total += psdScale;
    if (isShowPch) total += pchScale;
    if (isShowWf) total += wfScale;
    if (total > 0.0f)
    {
        if (isShowPsd) psdScale /= total;
        if (isShowPch) pchScale /= total;
        if (isShowWf) wfScale /= total;
    }
    else
    {
        resetScales();
    }
}

void Overview::resetScales()
{
    psdScale = 0.25f;
    pchScale = 0.25f;
    wfScale = 0.5f;
}

void Overview::createPsd(GlobalVariables *gVar)
{
    psd = new PsdLib(gVar, this);
    connectRepaint(psd);
}

void Overview::createPch(GlobalVariables *gVar)
{
    pch = new PchLib(gVar, this);
    connectRepaint(pch);
}

void Overview::createWf(GlobalVariables *gVar)
{
    wf = new WfLib(gVar, this);
    connectRepaint(wf);
}

void Overview::render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    int splitterCount = 0;
    if (isShowPsd && isShowPch) splitterCount++;
    if (isShowPch && isShowWf) splitterCount++;
    if (isShowPsd && isShowWf && !isShowPch) splitterCount++;
    float totalSplitterHeight = splitterHeight * splitterCount;
    float usable = avail.y - totalSplitterHeight;
    psdSize = isShowPsd ? psdScale * usable : 0.0f;
    pchSize = isShowPch ? pchScale * usable : 0.0f;
    wfSize  = isShowWf  ? wfScale  * usable : 0.0f;
    auto splitter = [&](const char* id, float& scaleA, float& scaleB, float minA = 0.1f, float minB = 0.1f)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f,0.3f,0.3f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f,0.5f,0.5f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f,0.6f,0.6f,1));
        ImGui::Button(id, ImVec2(avail.x, splitterHeight));
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            setCursor(Qt::SizeVerCursor);
        }
        if (ImGui::IsItemActive() && io.MouseDelta.y != 0.0f)
        {
            float delta = io.MouseDelta.y / usable;
            float newA = scaleA + delta;
            float newB = scaleB - delta;
            if (newA < minA)
            {
                newA = minA;
                newB = scaleA + scaleB - minA;
            }
            else if (newB < minB)
            {
                newB = minB;
                newA = scaleA + scaleB - minB;
            }
            scaleA = newA;
            scaleB = newB;
        }
        ImGui::PopStyleColor(3);
    };
    if (isShowPsd)
    {
        ImGui::BeginChild("##psd", ImVec2(avail.x, psdSize), false);
        psd->renderStandalone(ImVec2(avail.x, psdSize));
        ImGui::EndChild();
    }
    if (isShowPsd && isShowPch)
        splitter("##s1", psdScale, pchScale);
    else if (isShowPsd && isShowWf && !isShowPch)
        splitter("##s1", psdScale, wfScale);
    if (isShowPch)
    {
        ImGui::BeginChild("##pch", ImVec2(avail.x, pchSize), false);
        pch->renderStandalone(ImVec2(avail.x, pchSize));
        ImGui::EndChild();
    }
    if (isShowPch && isShowWf)
        splitter("##s2", pchScale, wfScale);
    if (isShowWf)
    {
        ImGui::BeginChild("##wf", ImVec2(avail.x, wfSize), false);
        wf->renderStandalone(ImVec2(avail.x, wfSize));
        ImGui::EndChild();
    }
    if (!ImGui::IsAnyItemHovered())
        setCursor(Qt::ArrowCursor);
}