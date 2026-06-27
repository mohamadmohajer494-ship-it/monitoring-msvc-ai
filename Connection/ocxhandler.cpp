#include "ocxhandler.h"

OCXHandler::OCXHandler(QObject *parent)
    : ConnectionInterface{parent}
{

}


bool OCXHandler::isConnected()
{
    if (!activex)
        return false;
    return false;
}

void OCXHandler::ConnectToSystem(ConnectionMode Connection,
                                 UDPType udptype,
                                 QString ip,
                                 short port,
                                 short PortSendUDP,
                                 short PortReciveUPD,
                                 short mtu,
                                 QString Comport,
                                 int Baudrate)
{
    if (!activex)
        return;
}

void OCXHandler::SendFrequency(double freqMHz)
{
    if (!activex)
        return;
}

void OCXHandler::SendThreshold(double thr)
{
    if (!activex)
        return;
}

void OCXHandler::SendDefaults(SystemDefualtsStruct SystemDefaults)
{
    if (!activex)
        return;
}

void OCXHandler::SendAntennaOffset(QMap<char, double> offsets)
{
    if (!activex)
        return;
}

void OCXHandler::SendAtennaState(QMap<char, bool> state)
{
    if (!activex)
        return;
}

void OCXHandler::SendAGc(char agc)
{
    if (!activex)
        return;
}

void OCXHandler::SendMGc(short mgc)
{
    if (!activex)
        return;
}

void OCXHandler::SendRFHead(char rfhead)
{
    if (!activex)
        return;
}

void OCXHandler::SendDemod(char demod)
{
    if (!activex)
        return;
}

void OCXHandler::SendFFTPoint(short fft)
{
    if (!activex)
        return;
}

void OCXHandler::SendSquelch(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendVolume(short vol)
{
    if (!activex)
        return;
}

void OCXHandler::SendAverage(double val)
{
    if (!activex)
        return;
}

void OCXHandler::SendQuality(double val)
{
    if (!activex)
        return;
}

void OCXHandler::SendPhaseAverage(double val)
{
    if (!activex)
        return;
}

void OCXHandler::SendAutoThreshold(bool val)
{
    if (!activex)
        return;
}

void OCXHandler::SendAzimuthHistogram(bool val)
{
    if (!activex)
        return;
}

void OCXHandler::SendClassificationState(ClassificationStruct Classsification)
{
    if (!activex)
        return;
}

void OCXHandler::SendBiteState(bool val)
{
    if (!activex)
        return;
}

void OCXHandler::SendAmrState(bool val)
{
    if (!activex)
        return;
}

void OCXHandler::SendAudioCodecOption(AudioCodecStruct acodec)
{
    if (!activex)
        return;
}

void OCXHandler::SendIFLogParam(IFlogParamStruct iflog)
{
    if (!activex)
        return;
}

void OCXHandler::SendBandwidth(char bw)
{
    if (!activex)
        return;
}

void OCXHandler::SendFilterBand(char filter)
{
    if (!activex)
        return;
}

void OCXHandler::SendFilter(char fil)
{
    if (!activex)
        return;
}

void OCXHandler::SendBandwidthElint(char bw)
{
    if (!activex)
        return;
}

void OCXHandler::SendThresholdLow(int val)
{
    if (!activex)
        return;
}

void OCXHandler::SendThresholdHigh(int val)
{
    if (!activex)
        return;
}

void OCXHandler::SendElintADCRef(short lowRef, short HighRef)
{
    if (!activex)
        return;
}

void OCXHandler::SendElintMaxHold(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendThresholdLimit(int val)
{
    if (!activex)
        return;
}

void OCXHandler::SendDFParameters(double Azimuth, double level, double Quality)
{
    if (!activex)
        return;
}

void OCXHandler::SendTransferDataRate(TransferDataStruct Tds)
{
    if (!activex)
        return;
}

void OCXHandler::SendAutoDetectAntenna(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendAutoDetectCompass(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendGPSData(GPSParameters Tds)
{
    if (!activex)
        return;
}

void OCXHandler::SendAutoDetectGPS(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendChannelNumber(short channel)
{
    if (!activex)
        return;
}

void OCXHandler::SendPageNumber(short PageNumber)
{
    if (!activex)
        return;
}

void OCXHandler::SendElintAntenna(bool isElint)
{
    if (!activex)
        return;
}

void OCXHandler::SendSwitchSequence(short seq)
{
    if (!activex)
        return;
}

void OCXHandler::SendAudioSate(bool state)
{
    if (!activex)
        return;
}

void OCXHandler::SendComputerParameters(ComputersParametersStruct ComputerParams)
{
    if (!activex)
        return;
}

void OCXHandler::SendManualMode(ManualModeStruct manual)
{
    if (!activex)
        return;
}

void OCXHandler::SendDFlog(DFlogStruct logs)
{
    if (!activex)
        return;
}

void OCXHandler::SendDFMode(DFModeFindings_DFMODFND DFmode)
{
    if (!activex)
        return;
}

void OCXHandler::SendPositionFinding(PFmodeStruct Pfparam)
{
    if (!activex)
        return;
}

void OCXHandler::SendMemoryScanParam(MemoryScanParamStruct MemPar)
{
    if (!activex)
        return;
}

void OCXHandler::SendMemoryScanState(MemoryScanStateStruct MemState)
{
    if (!activex)
        return;
}

void OCXHandler::SendSubRangeScanParam(SubRangeScanParamStruct SubPar)
{
    if (!activex)
        return;
}

void OCXHandler::SendSubRangeScanState(SubRangeScanStateStruct SubState)
{
    if (!activex)
        return;
}

void OCXHandler::SendFullRangeParam(FullRangeScanParamStruct Fullpar)
{
    if (!activex)
        return;
}

void OCXHandler::SendRemoveScanlist(RemoveScansTables_MODE scant)
{
    if (!activex)
        return;
}

void OCXHandler::SendGetAllDFConfig()
{
    if (!activex)
        return;
}

void OCXHandler::SendResetSystemConfig(ResetSystem_RESET resettype)
{
    if (!activex)
        return;
}

void OCXHandler::SendVersionRequest(string vs)
{
    if (!activex)
        return;
}

void OCXHandler::SendCalibraionParam(CalibrationStruct calib)
{
    if (!activex)
        return;
}

