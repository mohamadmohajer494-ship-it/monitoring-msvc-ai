#ifndef CDFSOCKET_H
#define CDFSOCKET_H


#include "CDFSocket_global.h"
#include <Connection.pb.h>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include "receivedata.h"
#include <zlib.h>
#include <QDateTime>
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <OpusDecoderWrapper.h>

#define NUM_CLASSES 17
#define SHIFTPSD  70

#define HEADERSIZE 2
#define FOOTERSIZE 2
#define HEADER1 0xAA
#define HEADER2 0xFF
#define FOOTER1 0x55
#define FOOTER2 0x55

#define PROTOID 0
#define PSDID   1
#define DOAID   2
#define DFPAR   3
#define AUDIO   4
#define GPSP    5
#define ANT     6
#define COM     7
#define TRG     8
#define SCANRES 9
#define AZIMITHS 10
#define FPSDID  11
#define PDWID   12
#define PSDW    13
#define BITE    14
#define CLASS   15
#define CLASSE  16
#define AMRS    17
#define AMRM    18
#define DOACID  19
#define DOAHISID  20
#define AZWIDID  21
#define MXHLDID  22
#define IQPDWID  23
#define NOTHING 24

#define _SYSPARAM           "sysp"
#define _SYSPARAMELINT      "syspe"
#define _Frequency          "frq"
#define _Threshold          "thr"
#define _AntOffset          "anto"
#define _AntState           "ants"
#define _AntParam           "antp"
#define _TransferData       "dft"
#define _SystemDefault      "def"
#define _GPSPARAMs          "gps"
#define _ComputerParam      "com"
#define _AudioParam         "ado"
#define _DFLog              "dfl"
#define _ManualModeFreq     "mmf"
#define _DFModeFinding      "dmf"
#define _PositionFinding    "pfm"
#define _MemoryScanParam    "msp"
#define _MemoryScanState    "mss"
#define _SubRangeScanParam  "ssp"
#define _SubRangeScanState  "sss"
#define _RemoveScanList     "rsl"
#define _FullRangeScan      "frs"
#define _GetAllSystem       "gas"
#define _ResetSystemConfig  "rsc"
#define _FileVersions       "fvs"
#define _ErrorsRequest      "eer"
#define _Calibration        "clr"
#define _Classification     "cfs"
#define _biteTestState      "bts"
#define _IFlog              "ifl"
#define _amrstate           "amr"
#define _audiocodec         "acc"
#define _systemstate        "sysm"
#define _histogramMode      "him"
#define _AzimuthBlanking    "abl"
#define _FrequencyOffset    "fof"
#define _FrequencyOffsetMode "fom"
#define _IQPDWMode          "ipm"
#define _Maxhold            "mxx"

using namespace std;
typedef struct SystemParametersStruct
{
    SystemParameters_AGC agcData = SystemParameters_AGC_AGC_OFF;
    double mgcData = 0;
    SystemParameters_RFHEAD rfhead = SystemParameters_RFHEAD_HS;
    SystemParameters_DEMOD demod = SystemParameters_DEMOD_FM;
    SystemParameters_FFTPOINT Fft = SystemParameters_FFTPOINT_FFTSample_2048;
    bool SetSquelch = false;
    int volume = 100;
    double Average = 0;
    BANDWIDTH bandwidth = _DFBW_WB;
    SystemParameters_FILTERS filter = SystemParameters_FILTERS__250KHz;
    double PhaseAverage = 0;
    bool AutoThreshold = false;
    bool AzimuthHistogram = false;
    double Quality = 100;
    SystemParameters_AnalogBand AnalogFilterBand = SystemParameters_AnalogBand_Band1;
    SystemParameters_AntPOL AntPolarization = SystemParameters_AntPOL__Vertical;
    SystemParameters_ANTAMP AntAmp = SystemParameters_ANTAMP__amp;
    SystemParameters_ANTENNALENGTH AntLength = SystemParameters_ANTENNALENGTH__long;
    short RFAttenator = 0;
    short IFAttenator = 0;
}SystemParametersStruct;
typedef struct AudioCodecStruct
{
    bool IsAudioCodeEnable = true;
    AudioCodec_CodecTools CodeTools = AudioCodec_CodecTools__opus;
    AudioCodec_CodecPower CodedPower = AudioCodec_CodecPower__MaximumPow;
}AudioCodecStruct;
typedef struct SystemParametersElintStruct
{
    bool ElintMaxhold = false;
    int ThresholdLow = 0;
    BANDWIDTH ElintBandwidth = _DFBW_WB;
    int ThresholdLimit =0;
    int ThresholdHigh = 0;
    int ADCLowRef = 0;
    int ADCHighRef = 0;
}SystemParametersElintStruct;
typedef struct SystemDefualtsStruct
{
    Defaults_SYSTEMTYPE SystemType;
    Defaults_BW BandwidthSize;
    int MinRangeFreq;
    int MaxRangeFreq;
    bool Revert = false;
    bool Is6GHz = false;
    bool IsSuperResolation = false;
    DFMode DFtype;
}SystemDefualtsStruct;
typedef struct TransferDataStruct
{
    TransferData_TRANSDATA TransferDatarange = ::TransferData_TRANSDATA__100Percent;
    bool TransferAudio = false;
    bool TransferPSD = true;
    bool TransferDOA = false;
    bool TransferAntennaResult = false;
    bool TransferDFParameters = true;
    bool TransferGPSParameters = false;
    bool TransferCompassParameters = false;
    bool TransferAzimuthCalculationParameters = false;
    bool TransferError = false;
    bool TransferFixWideBandPSDData = false;
    bool TransferPDWData = false;
    bool TransferIQData = false;
    bool TransferIFData =false;
    bool TransferAmrData = false;
    bool TransferHistogramDoa = false;
}TransferDataStruct;
typedef struct ComputersParametersStruct
{
    QString LanName;
    QString IpAddress;
    short mtu;
    short port;
    short UDPReceivePort;
    short UDPSendPort;
    QString UDPBroadCastIP;
    QString UDPMultiCastIP;
    bool UDPMode = false;
    bool UARTSendMode = false;
    bool UARTReceiveMode =false;
    bool _226AntOld=true;
}ComputersParametersStruct;
typedef struct GPSParameters
{
    bool AutoGPSDetect = false;
    bool AutoCompassDetect = false;
    bool IsFpgaUart = false;
    double AngleOffset = 0;
    double NorthOffset = 0;
    int  Baudrate = 9600;
    QString PortName = "/dev/ttyUSB0";
    GPSParameter_FRAMEWORK Framework;
    double Latitude = 0;
    double Longitude =0 ;
    short Altitude = 0;
    short Heading = 0;
    short Speed = 0;
    short Roll = 0;
    short pitch = 0;
    QDateTime dt;
    bool IsLock = false;
}GPSParameters;
typedef struct DFResultStruct
{
    double azimuth;
    double level;
    uint16_t quality;

}DFResultStruct;
typedef struct AntennaOptions
{
    short PageNumbers = 0;
    short ChannelNumebr = 0;
    short SwitchSequnece = 0;
    bool ElintMode = false;
}AntennaOptions;
typedef struct ClassificationStruct
{
    bool State = false;
    ClassificationState_ClassificationAlgoritm classificationAgl = ClassificationState::ExpertAlg;
}ClassificationStruct;
typedef struct DFlogStruct
{
    bool state;
    int params;
    QString loglocation;
}DFlogStruct;
typedef struct CalibrationStruct
{
    bool CoreOn;
    int gain;
}CalibrationStruct;
typedef struct ManualModeStruct
{
    bool state;
    short StartIndex;
    short StopIndex;
}ManualModeStruct;
typedef struct TargetDataS
{
    ClassificationDataRdy_SINGALMODE Signalmode;
    double StartFreq;
    double StopFreq;
    double LatFirst;
    double LongFirst;
    double TargetLat;
    double TargetLong;
    double Distance;
}TargetDataS;
typedef struct PFmodeStruct
{
    bool state = false;
    PositionFinding_ALGORITHM alg;
}PFmodeStruct;
typedef struct ScanResultStruct
{
    double Freq = 30;
    double Azimuth = -1;
}ScanResultStruct;
typedef struct MemoryScanParamStruct
{
    double Freq = 30;
    double Threshold = -100;
}MemoryScanParamStruct;
typedef struct MemoryScanStateStruct
{
    bool State = false;
    int32_t time = 0;
}MemoryScanStateStruct;
typedef struct SubRangeScanParamStruct
{
    double StartFrequencyMHz = 30;
    double StopFrequencyMHz = 60;
    int32_t StepFrequecnyKHz = 100;
    double level = 0;
}SubRangeScanParamStruct;
typedef struct SubRangeScanStateStruct
{
    bool State = false;
    int32_t time = 0;
}SubRangeScanStateStruct;
typedef struct FullRangeScanParamStruct
{
    double StartFrequencyMHz = 30;
    double StopFrequencyMHz = 60;
    bool State = false;
}FullRangeScanParamStruct;
typedef struct AzimuthsDataStruct
{
    double PureAzimuth = -1;
    double AfterRevertCenterDoa = -1;
    double AfterNorthOffset = -1;
    double AfterMechanicalOffset;
    double AfterHeadingCenterDoa = -1;
    double MechanicalOffset = -1;
    double Heading = -1;
    double NorthOffset = -1;

}AzimuthsDataStruct;
typedef enum SignalClassified
{
    _Fix = 0,
    _DS = 1,
    _Hopping = 2,
    _Link16 = 3
}SignalClassified;
typedef struct IFlogParamStruct
{
    bool state = false;
    QString name;
    short timems = 1000;
}IFlogParamStruct;
typedef struct ClassifiedSignalStruct
{
    short Fmin;
    short Fmax;
    double Direction;
    SignalClassified sig;

}ClassifiedSignalStruct;
struct PacketData {
    int totalPackets;
    QMap<int, QByteArray> fragments;
};
typedef enum DFAntennaType
{
    ADD050 = 0,
    ADD150 = 1,
    ADD327 = 2,
    ADD070 = 3,
    ADD078 = 4,
    ADD226 = 5,
    ADD207 = 6,
    ADD107 = 7,
    ADD297 = 8,
    ADDcount = 9
}DFAntennaType;
typedef enum AGCs
{
    AGC_OFF = 0,
    AGC_FAST = 1,
    AGC_MEDIUM = 2,
    AGC_SHORT = 3,
    AGC_LONG = 4,
    AGC_Count = 5
}AGCs;
typedef enum ConnectionMode
{
    TCPMode = 0,
    UDPMode = 1,
    UDPSerialMode=2,
    SerialSerialMode=3
}ConnectionMode;
typedef enum UDPType
{
    UniCastType =0,
    MultiCastType=1,
    BroadCastType=2
}UDPType;
typedef struct PulseParametersStruct
{
    quint64 TOA;//8
    float PW;//4
    short PA;//4
    float Freq;//4
    short DOA;//2
    bool CW_ALARM;//1
    bool Calibcom;//1
    char FilterType;//1
    short Quality;//2
    int GPS;//4
    quint8 Counter; //1 =32
}PulseParametersStruct;
typedef struct SGroup
{
    int				Cnt;
    double			Sum;
    double			Mean;
    double			Std;
    QVector<double>	Freq;
    QVector<double>	GPS;
}SGroup;
typedef struct structAMRResultStereo
{
    char MRModulation[50];
    double MRModulationAverageAcc;
    double fs;
    double symbol_rate;
    char instantModulation[NUM_CLASSES][50];
    float instantAccuracy[NUM_CLASSES];
}structAMRResultStereo;
typedef struct AmrTargetOutputSimple
{
    char MRModulation[50];
    double MRModulationAverageAcc;
    char instantModulation[17][50];
    float instantAccuracy[17];
    char id[50];
    double fb_iq;
    double fc_iq;
    int decimation_iq;
    double fs_iq;
    double bandwidth_iq;
    double cutoff_iq;
    size_t buffer_size;
    int seen_count;
    //char timeBuffer[50];
    int64_t timeBuffer;
    double snr;
    int sig_status;
    double symbol_rate;
}AmrTargetOutputSimple;
typedef struct AzimuthHistogramStruct
{
    bool State = false;
    int StartIndex=-1;
    int StopIndex= -1;
    int MinDoa = -1;
    int MaxDoa = -1;
    AzimuthHistogram_histogramMode Mode = AzimuthHistogram_histogramMode__CenterHistogramM;
}AzimuthHistogramStruct;
typedef struct Azimuthsblankingstruct
{
    double StartAzimuth;
    double StopAzimuth;
}Azimuthsblankingstruct;
typedef struct AzimuthFrequencystruct
{
    double StartFrequencyMHz;
    double StopFrequencyMHz;
    double Offset;
}AzimuthFrequencystruct;
typedef struct MaxholdParamsstruct
{
    bool state = false;
    double RefreshTime =0xffff;
    double DelayTime = 0xf;
}MaxholdParamsstruct;
#pragma pack(push,1)
struct IQSample
{
    qint16 I;
    qint16 Q;
};
#pragma pack(pop)
static_assert(sizeof(IQSample) == 4,
              "IQSample size must be 4 bytes");
class CDFSOCKET_EXPORT CDFSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap<char,bool> AntennaStates WRITE SendAtennaState NOTIFY AntennaStatesChanged)
    Q_PROPERTY(double Frequency WRITE SendFrequency NOTIFY FrequencyChanged)
    Q_PROPERTY(double Threshold WRITE SendThreshold NOTIFY ThresholdChanged)
    Q_PROPERTY(QMap<char,double> AntennaOffset WRITE SendAntennaOffset NOTIFY AntennaOffsetChanged)
    Q_PROPERTY(char AGC WRITE SendAGc NOTIFY AgcChanged)
    Q_PROPERTY(short MGC WRITE SendMGc NOTIFY MGcChange)
    Q_PROPERTY(char RFHead WRITE SendRFHead NOTIFY AntennaOffsetChanged)
    Q_PROPERTY(char Demod WRITE SendDemod NOTIFY DemodChanged)
    Q_PROPERTY(char FFTPoint WRITE SendFFTPoint NOTIFY FFTPointChanged)
    Q_PROPERTY(bool Squlench WRITE SendSquelch NOTIFY SquelchChanged)
    Q_PROPERTY(short Volume WRITE SendVolume NOTIFY VolumeChanged)
    Q_PROPERTY(double Average WRITE SendAverage NOTIFY AverageChange)
    Q_PROPERTY(double PhaseAverage WRITE SendPhaseAverage NOTIFY PhaseAverageChange)
    Q_PROPERTY(char Bandwidth WRITE SendBandwidth NOTIFY bandwidthChanged)
    Q_PROPERTY(char FilterBand WRITE SendFilterBand NOTIFY FilterBandChanged)
    Q_PROPERTY(char Filter WRITE SendFilter NOTIFY FilterChanged)
    Q_PROPERTY(char BandWidthElint WRITE SendBandwidthElint)
    Q_PROPERTY(int ThresholdLow WRITE SendThresholdLow)
    Q_PROPERTY(int ThresholdHigh WRITE SendThresholdHigh)
    Q_PROPERTY(int ThresholdLimit WRITE SendThresholdLimit)
    Q_PROPERTY(bool MaxHoldElint WRITE SendElintMaxHold)
    Q_PROPERTY(TransferDataStruct TransferData WRITE SendTransferDataRate NOTIFY TransferDataRate)
    Q_PROPERTY(GPSParameters GPSData WRITE SendGPSData NOTIFY GPSDataChanged)
    Q_PROPERTY(bool AutoDetectAntenna WRITE SendAutoDetectAntenna)
    Q_PROPERTY(bool AutoDetectOffset WRITE SendAutoDetectCompass)
    Q_PROPERTY(bool AutoDetectGPS WRITE SendAutoDetectGPS)
    Q_PROPERTY(short ChannelNumber WRITE SendChannelNumber NOTIFY ChannelNumberChanged)
    Q_PROPERTY(short PageNumber WRITE SendPageNumber NOTIFY PageNumberChanged)
    Q_PROPERTY(bool ElintAntenna WRITE SendElintAntenna NOTIFY ElintAntennaChanged)
    Q_PROPERTY(bool AudioState WRITE SendAudioSate  NOTIFY AudioParamChanged)
    Q_PROPERTY(bool AutoThreshold WRITE SendAutoThreshold  NOTIFY AutoThresholdChange)
    Q_PROPERTY(SystemDefualtsStruct Defaults WRITE SendDefaults  NOTIFY XferDefault)
    Q_PROPERTY(ComputersParametersStruct ComputerParameters WRITE SendComputerParameters NOTIFY ComputerParametersChanged)
    Q_PROPERTY(DFlogStruct DFlogs WRITE SendDFlog NOTIFY DFLogChanged)
    Q_PROPERTY(ManualModeStruct ManualMode WRITE SendManualMode  NOTIFY ManualModeChanged)
    Q_PROPERTY(DFModeFindings_DFMODFND DFMode WRITE SendDFMode  NOTIFY DFModeChanged)
    Q_PROPERTY(PFmodeStruct PositionFinding WRITE SendPositionFinding  NOTIFY PositionFindingModeChanged)
    Q_PROPERTY(MemoryScanParamStruct MemoryScanParam WRITE SendMemoryScanParam  NOTIFY MemoryScanParamChanged)
    Q_PROPERTY(MemoryScanStateStruct MemoryScanState WRITE SendMemoryScanState  NOTIFY MemoryScanStateChanged)
    Q_PROPERTY(SubRangeScanParamStruct SubRangeScanParam WRITE SendSubRangeScanParam  NOTIFY SubRangeScanParamChanged)
    Q_PROPERTY(SubRangeScanStateStruct SubrangeScanState WRITE SendSubRangeScanState  NOTIFY SubRangeScanStateChanged)
    Q_PROPERTY(FullRangeScanParamStruct FullRangeScanParam WRITE SendFullRangeParam  NOTIFY FullRangeParamChanged)
    Q_PROPERTY(RemoveScansTables_MODE RemoveScanTables WRITE SendRemoveScanlist  NOTIFY RemoveScanlistChanged)
    Q_PROPERTY(WRITE SendGetAllDFConfig  NOTIFY GetAllDFConfigEvent)
    Q_PROPERTY(ResetSystem_RESET resettype WRITE SendResetSystemConfig  NOTIFY ResetSystemConfigEvent)
    Q_PROPERTY(std::string Version WRITE SendVersionRequest NOTIFY VersionCommingChanged)
    Q_PROPERTY(CalibrationStruct CalibrationParam WRITE SendCalibraionParam NOTIFY CalibrationParamChanged)
    Q_PROPERTY(ClassificationStruct Classification WRITE SendClassificationState  NOTIFY ClassificationStateChanged)
    Q_PROPERTY(bool Bite WRITE SendBiteState  NOTIFY BiteStatechanged)
    Q_PROPERTY(bool Amr WRITE SendAmrState  NOTIFY AmrStatechanged)
    Q_PROPERTY(IFlogParamStruct IFLOG WRITE SendIFLogParam  NOTIFY IFLogParamtersChanged)
    Q_PROPERTY(AudioCodecStruct AudioCodec WRITE SendAudioCodecOption  NOTIFY AudioCodecParamtersChanged)
    Q_PROPERTY(DFMode SystemState NOTIFY SystemStateChanged)
    Q_PROPERTY(double Quality WRITE SendQuality NOTIFY QualityChanged)
    Q_PROPERTY(AzimuthHistogramStruct AzimuthHistogram WRITE SendAzimuthHistogramMode  NOTIFY AzimuthHistogramModeChanged)
    Q_PROPERTY(FrequencyOffsetMode_OFFSETMODES FrequencyOffsetMode WRITE SendFrequencyOffsetMode  NOTIFY FrequencyOffsetModeChanged)
    Q_PROPERTY(bool IQPDW WRITE SendIQPDWMode  NOTIFY IQPDWModeChanged)
    Q_PROPERTY(MaxholdParamsstruct MaxHold WRITE SendMaxHoldParam  NOTIFY MaxholdParamChanged)
public:
    CDFSocket();
private:
    ReceiveData *RecData;
    QTcpSocket *socket = nullptr;
    QUdpSocket *socketUDP;
    QSerialPort *serialPort;
    OpusDecoderWrapper *opusDecoder;
    QByteArray data;
    SystemParametersStruct Sysparam;
    SystemParametersElintStruct SysparamElint;
    double DoaDtCompressed[2000];
    bool ResumeFlag = false;
    QThread *thread = new QThread(this);
    void DoaData(const QByteArray &data);
    void MaxHoldData(const QByteArray &data);
    void DoaHistData(const QByteArray &data);
    void DoaComData(const QByteArray &data);
    void DFResult(const QByteArray &data);
    void AzimuthAreaResult(const QByteArray &data);
    void PSDdata(const QByteArray &data);
    void PSDdataWide(const QByteArray &data);
    void PDWData(const QByteArray &data);
    void IQPDWData(const QByteArray &data);
    void FULLPSDdata(const QByteArray &data);
    void GPSdata(const QByteArray &data);
    void Classificationdata(const QByteArray &data);
    void ClassificationdataElint(const QByteArray &data);
    void AMRDataStereo(const QByteArray &data);
    void AMRDataMono(const QByteArray &data);
    void AzimuthsData(const QByteArray &data);
    void BiteTestRes(const QByteArray &data);
    void ScanResultdata(const QByteArray &data);
    void Targetdata(const QByteArray &data);
    void Compassdata(const QByteArray &data);
    void ANTData(const QByteArray &data);
    void AudioData(const QByteArray &data);
    void ParseProtoData(const QByteArray &data);
    void SendSystemParams();
    void SendSystemElintParams();
    void SendByComport(const QByteArray& data);
    bool GetSystemParams(QByteArray data, string name);
    bool GetSystemParamsElint(QByteArray data, string name);
    bool GetFrequency(QByteArray data, string name);
    bool GetThreshold(QByteArray data, string name);
    bool GetDFParameters(QByteArray data, string name);
    bool GetAntennaOffset(QByteArray data, string name);
    bool GetAntennaState(QByteArray data, string name);
    bool GetTransferDataRules(QByteArray data,string name);
    bool GetGPSParams(QByteArray data,string name);
    bool GetAntennaParameters(QByteArray data,string name);
    bool GetAudioState(QByteArray data,string name);
    bool GetComputerState(QByteArray data,string name);
    bool GetDFLogParams(QByteArray data, string name);
    bool GetManualmode(QByteArray data, string name);
    bool GetDFmodeFinding(QByteArray data, string name);
    bool GetPositionFinding(QByteArray data, string name);
    bool GetMemoryScanParam(QByteArray data, string name);
    bool GetMemoryScanState(QByteArray data, string name);
    bool GetSubRangeScanParam(QByteArray data, string name);
    bool GetSubRangeScanState(QByteArray data, string name);
    bool GetFullRangeScanParam(QByteArray data, string name);
    bool GetRemoveScanTable(QByteArray data, string name);
    bool GetAllConfig(QByteArray data, string name);
    bool GetResetSystem(QByteArray data, string name);
    bool GetVersions(QByteArray data,string name);
    bool GetError(QByteArray data,string name);
    bool GetCalibrationCore(QByteArray data,string name);
    bool GetClassificationState(QByteArray data,string name);
    bool GetBiteState(QByteArray data,string name);
    bool GetIQPDWState(QByteArray data,string name);
    bool GetAmrState(QByteArray data,string name);
    bool GetIFlogState(QByteArray data,string name);
    bool GetAudioCodec(QByteArray data,string name);
    bool GetSystemState(QByteArray data,string name);
    bool GetAzimuthHistogramMode(QByteArray data,string name);
    bool GetAzimuthBlankings(QByteArray data,string name);
    bool GetFrequencyOffset(QByteArray data,string name);
    bool GetFrequencyOffsetMode(QByteArray data,string name);
    bool GetMaxHoldParam(QByteArray data,string name);
    void SendData(QByteArray dt);
    void parsData(QByteArray &data);
    QByteArray framePayload(const QByteArray& payload);
    DFModeFindings_DFMODFND DFmode;
    ComputersParametersStruct ComputerParams;
    SystemParameters SystemParam;
    SystemParametersElint SystemParamElint;
    DFlogStruct DFlogS;
    PFmodeStruct PFparam;
    ManualModeStruct ManualMode;
    GPSParameters gpsparams;
    AzimuthsDataStruct Azimuthsparams;
    MaxholdParamsstruct Maxholdparams;
    Frequency fr;
    SystemError se;
    Threshold trh;
    AntennaOFFset af;
    AntennaState as;
    Defaults def;
    MemScanParam msp;
    MemScanState mss;
    SubScanParam ssp;
    SubScanState sss;
    FullRangeScanParams frsp;
    RemoveScansTables rst;
    GetAllDFConfig gadc;
    ResetSystem rsd;
    FileVersion fv;
    Calibration cl;
    ClassificationState cfs;
    Bite bte;
    IQPDWMode ipm;
    Amr amr;
    AzimuthHistogram ah;
    FrequencyOffsetMode fom;
    AudioCodec acc;
    IFrecord ifr;
    TransferDataStruct DataTransferS;
    MemoryScanParamStruct msps;
    MemoryScanStateStruct msss;
    SubRangeScanParamStruct srsps;
    SubRangeScanStateStruct srsss;
    FullRangeScanParamStruct frsps;
    AzimuthHistogramStruct ahs;
    TransferData td;
    GPSParameter gp;
    ScanResultStruct ScanRes;
    ComputerParameters cp;
    ManualModeParams mmp;
    DFModeFindings dmf;
    PositionFinding pfm;
    DFlog dfl;
    AudioState audios;
    AntennaParameters ap;
    TargetDataS TargetData;
    CalibrationStruct cls;
    ClassificationStruct cs;
    IFlogParamStruct ifl;
    AudioCodecStruct acs;
    PulseParametersStruct pdw[64];
    AntennaOptions ao;
    Maxholds mxh;
    bool audioState = false;
    short UDPSendPort;
    bool UDPModeState = false;
    bool UartMode = false;
    bool ReceiveUartState = false;
    bool SerialConnected = false;
    QString UDPSendIP;
    int MTU = 0;
    int commandcounter = 0;
    QByteArray buffer;
    short counter = 0;
    QByteArray rxBuf;
    RemoveScansTables_MODE ScanTable;
    SystemMode spm;
    bool AzimuthBlankingState = false;
    FrequencyOffsetMode_OFFSETMODES OffsetMode = FrequencyOffsetMode_OFFSETMODES__AntennaOffset;
public :
    bool isConnected();
    void ConnectToSystem(ConnectionMode Connection,UDPType udptype,QString ip, short port
                         , short PortSendUDP, short PortReciveUPD, short mtu = 1000, QString Comport = "tty1", int Baudrate = 115200);
    void DisconnectTCP();
    void SendFrequency(double freqMHz);
    void SendThreshold(double thr);
    void SendDefaults(SystemDefualtsStruct SystemDefaults);
    void SendAntennaOffset(QMap<char, double> offsets);
    void SendAzimuthHistogramMode(AzimuthHistogramStruct hist);
    void SendAzimuthBlankings(const QList<Azimuthsblankingstruct>& list, bool state);
    void SendFrequencyOffset(const QList<AzimuthFrequencystruct>& list);
    void SendFrequencyOffsetMode(FrequencyOffsetMode_OFFSETMODES ofmode);
    void SendAtennaState(QMap<char,bool> state);
    //void SendGPSData();
    void SendAGc(char agc);
    void SendMGc(short mgc);
    void SendRFHead(char rfhead);
    void SendRFIFAttenator(short rfatt,short ifatt);
    void SendDemod(char demod);
    void SendFFTPoint(short fft);
    void SendSquelch(bool state);
    void SendVolume(short vol);
    void SendAverage(double val);
    void SendQuality(double val);
    void SendPhaseAverage(double val);
    void SendAutoThreshold(bool val);
    void SendAzimuthHistogram(bool val);
    void SendClassificationState(ClassificationStruct Classsification);
    void SendBiteState(bool val);
    void SendIQPDWMode(bool val);
    void SendAmrState(bool val);
    void SendAudioCodecOption(AudioCodecStruct acodec);
    void SendIFLogParam(IFlogParamStruct iflog);
    void SendBandwidth(char bw);
    void SendFilterBand(char filter);
    void SendFilter(char fil);
    void SendAntennaPolarization(SystemParameters_AntPOL val);
    void SendAntennaAmp(SystemParameters_ANTAMP val);
    void SendAntennaLength(SystemParameters_ANTENNALENGTH val);
    void SendBandwidthElint(char bw);
    void SendThresholdLow(int val);
    void SendThresholdHigh(int val = 25000000);
    void SendElintADCRef(short lowRef,short HighRef);
    void SendElintMaxHold(bool state);
    void SendThresholdLimit(int val);
    void SendDFParameters(double Azimuth, double level, double Quality);
    void SendTransferDataRate(TransferDataStruct Tds);
    void SendAutoDetectAntenna(bool state);
    void SendDualPolAntenna(bool state);
    void SendAutoDetectCompass(bool state);
    void SendGPSData(GPSParameters gpsp);
    void SendAutoDetectGPS(bool state);
    void SendChannelNumber(short channel);
    void SendPageNumber(short PageNumber);
    void SendElintAntenna(bool isElint);
    void SendSwitchSequence(short seq);
    void SendAudioSate(bool state);
    void SendComputerParameters(ComputersParametersStruct ComputerParams);
    void SendManualMode(ManualModeStruct manual);
    void SendDFlog(DFlogStruct logs);
    void SendDFMode(DFModeFindings_DFMODFND DFmode );
    void SendPositionFinding(PFmodeStruct Pfparam);
    void SendMemoryScanParam(MemoryScanParamStruct MemPar);
    void SendMemoryScanState(MemoryScanStateStruct MemState);
    void SendSubRangeScanParam(SubRangeScanParamStruct SubPar);
    void SendSubRangeScanState(SubRangeScanStateStruct SubState);
    void SendFullRangeParam(FullRangeScanParamStruct Fullpar);
    void SendRemoveScanlist(RemoveScansTables_MODE scant);
    void SendGetAllDFConfig();
    void SendResetSystemConfig(ResetSystem_RESET resettype);
    void SendVersionRequest(std::string vs);
    void SendCalibraionParam(CalibrationStruct calib);
    void SendMaxHoldParam(MaxholdParamsstruct maxhold);

public slots:
    void onConnected();
    void onDisconnected();
    void readData();
    void readSerialData();
signals:
    void SystemParamEnvet(SystemParametersStruct);
    void SystemParamElintEnvet(SystemParametersElintStruct);
    void XferDFResult(DFResultStruct);
    void XferAzimuthArea(double);
    void XferPSD(const double*,short);
    void XferPSDWide(const double*,short);
    void XferPDW(PulseParametersStruct*,short);
    void XferIQPDW(QVector<IQSample>,PulseParametersStruct);
    void XferPSDFullRange(const double* data,short index,short cnt,short len);
    void XferDOA(const double*,double,short);
    void XferMaxHold(const double*,short);
    void XferHistDOA(const uint16_t*,short);
    void XferDefault(SystemDefualtsStruct);
    void XferAudioData(QByteArray,short);
    void XferPhaseAmp(double measuredPH1[],short len,double phaseAddr[],short lenPhase,double AmpAddr[],short lenAmp);
    void ConnectedState(bool);
    void AntennaStatesChanged(QMap<char,bool>,bool,bool);
    void AntennaOffsetChanged(QMap<char,double>);
    void AgcChanged(char);
    void MGcChange(short);
    void RFHeadChanged(char);
    void DemodChanged(char);
    void QualityChanged(double);
    void FFTPointChanged(char);
    void SquelchChanged(bool);
    void VolumeChanged(short);
    void AverageChange(double);
    void PhaseAverageChange(double);
    void bandwidthChanged(char);
    void FilterBandChanged(char);
    void FilterChanged(char);
    void FrequencyChanged(double);
    void ThresholdChanged(double);
    void TransferDataRate(TransferDataStruct);
    void GPSDataChanged(GPSParameters);
    void AzimuthsMonitoringChanged(AzimuthsDataStruct);
    void GPSParametersChanged(GPSParameters);
    void CompassDataChanged(GPSParameters);
    void ChannelNumberChanged(short);
    void SwitchSequenceChanged(short);
    void PageNumberChanged(short);
    void ElintAntennaChanged(bool);
    void AudioParamChanged(bool);
    void ComputerParametersChanged(ComputersParametersStruct);
    void DFLogChanged(DFlogStruct);
    void AutoThresholdChange(bool);
    void ManualModeChanged(ManualModeStruct);
    void DFModeChanged(DFModeFindings_DFMODFND);
    void PositionFindingModeChanged(PFmodeStruct);
    void TargetAreaAndPositionChanged(TargetDataS TDs);
    void MemoryScanParamChanged(MemoryScanParamStruct);
    void MemoryScanStateChanged(MemoryScanStateStruct);
    void SubRangeScanParamChanged(SubRangeScanParamStruct);
    void SubRangeScanStateChanged(SubRangeScanStateStruct);
    void FullRangeParamChanged(FullRangeScanParamStruct);
    void RemoveScanlistChanged(RemoveScansTables_MODE);
    void GetAllDFConfigEvent();
    void ResetSystemConfigEvent(ResetSystem_RESET);
    void ScanResultData(ScanResultStruct);
    void VersionCommingChanged(std::string);
    void ErrorChanged(std::string);
    void CalibrationParamChanged(CalibrationStruct);
    void ClassificationStateChanged(ClassificationStruct);
    void BiteStatechanged(bool);
    void IQPDWModeChanged(bool);
    void AmrStatechanged(bool);
    void IFLogParamtersChanged(IFlogParamStruct);
    void AudioCodecParamtersChanged(AudioCodecStruct);
    void SystemStateChanged(DFMode mode);
    void BiteTestResult(QByteArray);
    void ClassificationResult(ClassifiedSignalStruct);
    void ClassificationResultElint(QVector<SGroup>,short size);
    void AmrStereoDataChanged(structAMRResultStereo);
    void AmrMonoDataChanged( std::vector<AmrTargetOutputSimple>);
    void AzimuthBlankingsChanged(QList<Azimuthsblankingstruct>,bool);
    void FrequencyOffsetChanged(QList<AzimuthFrequencystruct>);
    void AzimuthHistogramModeChanged(AzimuthHistogramStruct);
    void FrequencyOffsetModeChanged(FrequencyOffsetMode_OFFSETMODES);
    void MaxholdParamChanged(MaxholdParamsstruct);
};

#endif // CDFSOCKET_H
