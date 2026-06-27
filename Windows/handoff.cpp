#include "handoff.h"

Handoff::Handoff(GlobalVariables *gVar, QWidget *parent)
    : BaseWindows(gVar, "Handoff", parent)
{
    QLabel *lblTest = new QLabel("Handoff Windows",this);
}

Handoff::~Handoff()
{

}

void Handoff::render()
{
    ImVec2 s = ImGui::GetContentRegionAvail();
}