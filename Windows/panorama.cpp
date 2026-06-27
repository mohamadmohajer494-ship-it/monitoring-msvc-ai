#include "panorama.h"

Panorama::Panorama(GlobalVariables *gVar, QWidget *parent)
    : BaseWindows(gVar, "Panorama", parent)
{
    QLabel *lblTest = new QLabel("Panorama Windows",this);
}

Panorama::~Panorama()
{

}

void Panorama::render()
{
    ImVec2 s = ImGui::GetContentRegionAvail();
}
