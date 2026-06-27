#include "analysis.h"

Analysis::Analysis(GlobalVariables *gVar, QWidget *parent)
    : BaseWindows(gVar, "Analysis", parent)
{
    QLabel *lblTest = new QLabel("Analysis Windows",this);
}

Analysis::~Analysis()
{

}

void Analysis::render()
{
    ImVec2 s = ImGui::GetContentRegionAvail();
}
