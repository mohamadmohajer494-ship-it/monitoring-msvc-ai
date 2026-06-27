#include "direction.h"

Direction::Direction(GlobalVariables *gVar, QWidget *parent)
    : BaseWindows(gVar, "Direction", parent)
{
    createControllBox(gVar);
    createGauge(gVar);
    createDoa(gVar);
    createPsd(gVar);
    createWf(gVar);
    resetScales();
    attachRenderer();
}

Direction::~Direction()
{

}



void Direction::createControllBox(GlobalVariables *gVar)
{
    controllBox = new QFrame(this);
    controllBox->setFrameShape(QFrame::NoFrame);
    controllBox->setFrameShadow(QFrame::Plain);
    controllBox->setCursor(Qt::ArrowCursor);
    controllBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout* gLayout = new QGridLayout();
    gLayout->setColumnStretch(0, 0);
    gLayout->setColumnStretch(1, 1);
    gLayout->setColumnStretch(2, 0);
    gLayout->setColumnStretch(3, 1);
    gLayout->setContentsMargins(10, 10, 10, 10);
    gLayout->setSpacing(10);
    controllBox->setLayout(gLayout);
    QLabel* lblSync = new QLabel("Sync : ");
    gLayout->addWidget(lblSync,0,0);
    btnSync = new BtnToggle(gVar);
    btnSync->setStatus(true);
    connect(btnSync, &BtnToggle::toggled,
            this, &Direction::onClickSync);
    gLayout->addWidget(btnSync,0,1);
    QLabel* lblPf = new QLabel("PF : ");
    gLayout->addWidget(lblPf,0,2);
    btnPf = new BtnToggle(gVar);
    btnPf->setStatus(true);
    gLayout->addWidget(btnPf,0,3);
    QLabel* lblFreq = new QLabel("Freq : ");
    gLayout->addWidget(lblFreq,1,0);
    txtFreq = new TxtFreq(gVar);
    txtFreq->setEnabled(false);
    txtFreq->setValue(300);
    gLayout->addWidget(txtFreq,1,1,1,3);

    QLabel* lblBw = new QLabel("BW : ");
    gLayout->addWidget(lblBw,2,0);
    cmbBw = new QComboBox();
    cmbBw->setEnabled(false);
    cmbBw->addItems({"50KHz","100KHz","150KHz","200KHz","250KHz","500KHz","1MHz","2MHz","4MHz","10MHz","20MHz","40MHz","80MHz","200MHz"});
    gLayout->addWidget(cmbBw,2,1);
    QLabel* lblRes = new QLabel("Res : ");
    gLayout->addWidget(lblRes,2,2);
    cmbRes = new QComboBox();
    cmbRes->setEnabled(false);
    cmbRes->addItems({"512","1024","2048"});
    gLayout->addWidget(cmbRes,2,3);

    QLabel* lblMod = new QLabel("MOD : ");
    gLayout->addWidget(lblMod,3,0);
    cmbMod = new QComboBox();
    cmbMod->setEnabled(false);
    cmbMod->addItems({"AM","FM","PM"});
    gLayout->addWidget(cmbMod,3,1);
    QLabel* lblIff = new QLabel("IFF : ");
    gLayout->addWidget(lblIff,3,2);
    cmbIff = new QComboBox();
    cmbIff->setEnabled(false);
    cmbIff->addItems({"1KHz","2Hz","3Hz","7.5Hz","15KHz","125KHz","250KHz"});
    gLayout->addWidget(cmbIff,3,3);

    QLabel* lblRfh = new QLabel("RFH : ");
    gLayout->addWidget(lblRfh,4,0);
    cmbRfh = new QComboBox();
    cmbRfh->setEnabled(false);
    cmbRfh->addItems({"Attenator", "High Linearity", "Intermediate", "High Sensitivity"});
    gLayout->addWidget(cmbRfh,4,1,1,3);

    QString style = "padding : 5px 10px;";
    txtFreq->setStyleSheet(style);
    cmbBw->setStyleSheet(style);
    cmbRes->setStyleSheet(style);
    cmbMod->setStyleSheet(style);
    cmbIff->setStyleSheet(style);
    cmbRfh->setStyleSheet(style);

}

void Direction::createGauge(GlobalVariables *gVar)
{
    gauge = new DFGauge();
    gauge->config().useHeatmap = true;
    gauge->config().threshold = 2.0f;
    gauge->config().smoothingFactor = 0.3f;
    gauge->config().peakDecayRate = 0.95f;
    gauge->config().rotationMode = DFGauge::RotationMode::NorthUp;

    std::array<float, 360> testData{};
    for (int i = 0; i < 360; i++) {
        float dist = std::min(std::abs(i - 60), std::abs(i - 60 + 360));
        dist = std::min(dist, (float)std::abs(i - 60 - 360));
        testData[i] = std::max(0.0f, 50.0f - dist * 2.0f);
    }
    gauge->setData(testData);
    gauge->setHeading(120.0f);
}

void Direction::createDoa(GlobalVariables *gVar)
{
    doa = new PchLib(gVar, this);
    connectRepaint(doa);
}

void Direction::createPsd(GlobalVariables *gVar)
{
    psd = new PsdLib(gVar, this);
    connectRepaint(psd);
}

void Direction::createWf(GlobalVariables *gVar)
{
    wf = new WfLib(gVar, this);
    connectRepaint(wf);
}

void Direction::setShowControllBox(bool act)
{
    isShowControllBox = act;
    normalizeVisible();
}

void Direction::setShowGauge(bool act)
{
    isShowGauge = act;
    normalizeVisible();
}

void Direction::setShowDoa(bool act)
{
    isShowDoa = act;
    normalizeVisible();
}

void Direction::setShowPsd(bool act)
{
    isShowPsd = act;
    normalizeVisible();
}

void Direction::setShowWf(bool act)
{
    isShowWf = act;
    normalizeVisible();
}

void Direction::normalizeVisible()
{
    float leftTotal = 0.0f;

    if (isShowControllBox)
        leftTotal += controllBoxScale;

    if (isShowGauge)
        leftTotal += gaugesScale;

    if (leftTotal > 0.0f)
    {
        if (isShowControllBox)
            controllBoxScale /= leftTotal;

        if (isShowGauge)
            gaugesScale /= leftTotal;
    }


    float rightTotal = 0.0f;

    if (isShowDoa)
        rightTotal += doaScale;

    if (isShowPsd)
        rightTotal += psdScale;

    if (isShowWf)
        rightTotal += wfScale;

    if (rightTotal > 0.0f)
    {
        if (isShowDoa)
            doaScale /= rightTotal;

        if (isShowPsd)
            psdScale /= rightTotal;

        if (isShowWf)
            wfScale /= rightTotal;
    }

    if (leftTotal <= 0.0f && rightTotal <= 0.0f)
        resetScales();
}

void Direction::resetScales()
{
    leftColumnScale = 0.20f;

    controllBoxScale = 0.50f;
    gaugesScale = 0.50f;

    doaScale = 0.33f;
    psdScale = 0.33f;
    wfScale = 0.33f;
}

void Direction::onClickSync(bool act)
{
    txtFreq->setEnabled(!act);
    cmbBw->setEnabled(!act);
    cmbRes->setEnabled(!act);
    cmbMod->setEnabled(!act);
    cmbIff->setEnabled(!act);
    cmbRfh->setEnabled(!act);
}

void Direction::onBandwidthChanged(QString str, double num)
{
    cmbBw->setCurrentText(str);
}

void Direction::onResolutionChanged(QString str, double num)
{
    cmbRes->setCurrentText(str);
}

void Direction::onModulationChanged(QString str)
{
    cmbMod->setCurrentText(str);
}

void Direction::onIFFilterChanged(QString str)
{
    cmbIff->setCurrentText(str);
}

void Direction::onRFHeadChanged(QString str)
{
    cmbRfh->setCurrentText(str);
}

void Direction::onSystemParam(SystemParametersStruct param)
{

}

void Direction::onFrequencyChanged(double num)
{
    txtFreq->setValue(num);
}

void Direction::render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (avail.x <= 1.0f || avail.y <= 1.0f)
        return;

    const ImGuiWindowFlags childFlags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    const float columnSplitterWidth = splitterHeight;

    const float minLeftColumnScale = 0.10f;
    const float maxLeftColumnScale = 0.50f;

    auto horizontalSplitter = [&](const char* id,
                                  float width,
                                  float usableHeight,
                                  float& scaleA,
                                  float& scaleB,
                                  float minA = 0.10f,
                                  float minB = 0.10f)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

        ImGui::Button(id, ImVec2(width, splitterHeight));

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            setCursor(Qt::SizeVerCursor);
        }

        if (ImGui::IsItemActive() && io.MouseDelta.y != 0.0f && usableHeight > 1.0f)
        {
            float sum = scaleA + scaleB;
            float delta = io.MouseDelta.y / usableHeight;

            float newA = scaleA + delta;
            float newB = scaleB - delta;

            if (newA < minA)
            {
                newA = minA;
                newB = sum - minA;
            }

            if (newB < minB)
            {
                newB = minB;
                newA = sum - minB;
            }

            scaleA = newA;
            scaleB = newB;
        }

        ImGui::PopStyleColor(3);
    };

    auto verticalSplitter = [&](const char* id)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

        ImGui::Button(id, ImVec2(columnSplitterWidth, avail.y));

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            setCursor(Qt::SizeHorCursor);
        }

        if (ImGui::IsItemActive() && io.MouseDelta.x != 0.0f && avail.x > 1.0f)
        {
            leftColumnScale += io.MouseDelta.x / avail.x;

            if (leftColumnScale < minLeftColumnScale)
                leftColumnScale = minLeftColumnScale;

            if (leftColumnScale > maxLeftColumnScale)
                leftColumnScale = maxLeftColumnScale;
        }

        ImGui::PopStyleColor(3);
    };

    float contentWidth = avail.x - columnSplitterWidth;

    if (contentWidth <= 1.0f)
        return;

    float leftWidth = contentWidth * leftColumnScale;
    float rightWidth = contentWidth - leftWidth;

    if (leftWidth < 1.0f)
        leftWidth = 1.0f;

    if (rightWidth < 1.0f)
        rightWidth = 1.0f;

    // ============================================================
    // Left column: Control + Gauge
    // ============================================================
    int leftSplitterCount = 0;

    if (isShowControllBox && isShowGauge)
        leftSplitterCount++;

    float leftUsableHeight = avail.y - (splitterHeight * leftSplitterCount);

    if (leftUsableHeight < 1.0f)
        leftUsableHeight = 1.0f;

    controllBoxSize = isShowControllBox ? controllBoxScale * leftUsableHeight : 0.0f;
    gaugesSize      = isShowGauge       ? gaugesScale      * leftUsableHeight : 0.0f;

    // ============================================================
    // Right column: Doa/PcH + Psd + Wf
    // ============================================================
    int rightSplitterCount = 0;

    if (isShowDoa && isShowPsd)
        rightSplitterCount++;

    if (isShowPsd && isShowWf)
        rightSplitterCount++;

    if (isShowDoa && isShowWf && !isShowPsd)
        rightSplitterCount++;

    float rightUsableHeight = avail.y - (splitterHeight * rightSplitterCount);

    if (rightUsableHeight < 1.0f)
        rightUsableHeight = 1.0f;

    doaSize = isShowDoa ? doaScale * rightUsableHeight : 0.0f;
    psdSize = isShowPsd ? psdScale * rightUsableHeight : 0.0f;
    wfSize  = isShowWf  ? wfScale  * rightUsableHeight : 0.0f;

    // ============================================================
    // Render left column
    // ============================================================
    ImGui::BeginChild("##direction_left_column",
                      ImVec2(leftWidth, avail.y),
                      false,
                      childFlags);

    if (isShowControllBox)
    {
        ImGui::BeginChild("##direction_control_box",
                          ImVec2(leftWidth, controllBoxSize),
                          false,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        QWidget* container = controllBox;
        container->setFixedSize(leftWidth, controllBoxSize);

        ImGui::EndChild();
    }

    if (isShowControllBox && isShowGauge)
    {
        horizontalSplitter("##direction_left_splitter",
                           leftWidth,
                           leftUsableHeight,
                           controllBoxScale,
                           gaugesScale);
    }

    if (isShowGauge)
    {
        ImGui::BeginChild("##direction_gauge_box",
                          ImVec2(leftWidth, gaugesSize),
                          false,
                          childFlags);

        if (gauge)
        {
            float deltaTime = 1.0f / 60.0f;
            gauge->update(deltaTime);
            gauge->render();
        }

        ImGui::EndChild();
    }

    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);

    // ============================================================
    // Main vertical splitter
    // ============================================================
    verticalSplitter("##direction_main_vertical_splitter");

    ImGui::SameLine(0.0f, 0.0f);

    // ============================================================
    // Render right column
    // ============================================================
    ImGui::BeginChild("##direction_right_column",
                      ImVec2(rightWidth, avail.y),
                      false,
                      childFlags);

    if (isShowDoa)
    {
        ImGui::BeginChild("##direction_doa_box",
                          ImVec2(rightWidth, doaSize),
                          false,
                          childFlags);

        if (doa)
            doa->renderStandalone(ImVec2(rightWidth, doaSize));

        ImGui::EndChild();
    }

    if (isShowDoa && isShowPsd)
    {
        horizontalSplitter("##direction_right_splitter_1",
                           rightWidth,
                           rightUsableHeight,
                           doaScale,
                           psdScale);
    }
    else if (isShowDoa && isShowWf && !isShowPsd)
    {
        horizontalSplitter("##direction_right_splitter_1",
                           rightWidth,
                           rightUsableHeight,
                           doaScale,
                           wfScale);
    }

    if (isShowPsd)
    {
        ImGui::BeginChild("##direction_psd_box",
                          ImVec2(rightWidth, psdSize),
                          false,
                          childFlags);

        if (psd)
            psd->renderStandalone(ImVec2(rightWidth, psdSize));

        ImGui::EndChild();
    }

    if (isShowPsd && isShowWf)
    {
        horizontalSplitter("##direction_right_splitter_2",
                           rightWidth,
                           rightUsableHeight,
                           psdScale,
                           wfScale);
    }

    if (isShowWf)
    {
        ImGui::BeginChild("##direction_wf_box",
                          ImVec2(rightWidth, wfSize),
                          false,
                          childFlags);

        if (wf)
            wf->renderStandalone(ImVec2(rightWidth, wfSize));

        ImGui::EndChild();
    }

    ImGui::EndChild();

    if (!ImGui::IsAnyItemHovered())
        setCursor(Qt::ArrowCursor);
}