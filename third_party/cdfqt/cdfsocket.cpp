#include <vector>
#include "cdfsocket.h"

CDFSocket::CDFSocket()
{
    qDebug() << "libCDF Version is : " << APP_VERSION;
    SystemParam.set_agcdata(Sysparam.agcData);
    SystemParam.set_mgcdata(Sysparam.mgcData);
    SystemParam.set_autothreshold(Sysparam.AutoThreshold);
    SystemParam.set_average(Sysparam.Average);
    SystemParam.set_phaseaverage(Sysparam.PhaseAverage);
    SystemParam.set_azimuthhistogram(Sysparam.AzimuthHistogram);
    SystemParam.set_bandwidth(Sysparam.bandwidth);
    SystemParam.set_demod(Sysparam.demod);
    SystemParam.set_rfhead(Sysparam.rfhead);
    SystemParam.set_fft(Sysparam.Fft);
    SystemParam.set_filter(Sysparam.filter);
    SystemParam.set_setsquelch(Sysparam.SetSquelch);
    SystemParam.set_volume(Sysparam.volume);
    SystemParam.set_analogfilter(Sysparam.AnalogFilterBand);
    SystemParam.set_antennapolarization(Sysparam.AntPolarization);
    SystemParam.set_antennalength(Sysparam.AntLength);
    SystemParam.set_antennaamp(Sysparam.AntAmp);
    SystemParam.set_rfattenator(Sysparam.RFAttenator);
    SystemParam.set_ifattenator(Sysparam.IFAttenator);
    opusDecoder = new OpusDecoderWrapper();
}

bool CDFSocket::isConnected()
{
    return socket->state() == QAbstractSocket::ConnectedState;
}
void CDFSocket::ConnectToSystem(ConnectionMode Connection, UDPType udptype, QString ip, short port, short PortSendUDP
                                , short PortReciveUPD, short mtu, QString Comport, int Baudrate)
{
    Q_UNUSED(udptype);
    MTU = mtu;
    switch (Connection) {
    case UDPMode:
        UDPModeState = true;
        break;
    case UDPSerialMode:
        UDPModeState = true;
        UartMode = true;
        break;
    case SerialSerialMode:
        UartMode = true;
        ReceiveUartState = true;
        break;
    case TCPMode:
    default:
        UDPModeState = false;
        UartMode = false;
        ReceiveUartState = false;
        break;
    }

    if(UartMode && !SerialConnected)
    {
        serialPort = new QSerialPort();
        serialPort->setPortName("/dev/" + Comport); // Use the correct serial port name for your system
        serialPort->setBaudRate(Baudrate);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        QAbstractSocket::OpenModeFlag flag = QIODevice::WriteOnly;
        if(ReceiveUartState)
            flag = QIODevice::ReadWrite;
        else
            flag = QIODevice::WriteOnly;
        if (serialPort->open(flag)) {
            SerialConnected = true;
            qDebug() << "Serial port for Send command data opened successfully is Comport = " << Comport << " and baudrate = " << Baudrate;
        } else {
            qCritical()<< "Failed to open serial port Send command data: " << serialPort->errorString();
        }
        if(ReceiveUartState)
            connect(serialPort, &QSerialPort::readyRead, this, &CDFSocket::readSerialData);
    }
    if(UDPModeState && !ReceiveUartState)
    {
        UDPSendPort = PortSendUDP;
        UDPSendIP = ip;
        socketUDP = new QUdpSocket(this);
        if (socketUDP->bind(QHostAddress::Any, PortReciveUPD)) {
            qDebug() << "UPD = " << socketUDP->errorString();
            //socketUDP->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 65536); // 64 KB buffer
            socketUDP->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 8 * 1024 * 1024);
            connect(socketUDP, &QUdpSocket::readyRead, this, &CDFSocket::readData);
        }
        else
            qDebug() << "Failed to bind to port " << port;
    }
    else if(!UDPModeState && !ReceiveUartState)
    {
        if(socket == nullptr)
        {
            socket = new QTcpSocket(this);
            connect(socket, &QTcpSocket::connected, this, &CDFSocket::onConnected);
            connect(socket, &QTcpSocket::disconnected, this, &CDFSocket::onDisconnected);
            socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
            socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 8*1024*1024);
            socket->setReadBufferSize(0);
            socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        }
        if(!socket->isOpen() && !socket->isValid())
        {
            socket->connectToHost(ip, port);

        }
    }

}

void CDFSocket::DisconnectTCP()
{
    if(socket != nullptr)
        if(socket->isOpen())
            socket->disconnect();
}
void CDFSocket::onConnected()
{
    qDebug() << "Connected to the server";
    //thread->start();
    emit ConnectedState(true);
    connect(socket, &QTcpSocket::readyRead, this, &CDFSocket::readData);
}

void CDFSocket::onDisconnected()
{
    qWarning() << "Disconnected from the server";
    socket->close();
    socket->deleteLater();
    socket->destroyed();
    socket = nullptr;
    emit ConnectedState(false);
}

void CDFSocket::readData()
{
    if(!UDPModeState)
    {
        while (socket->bytesAvailable() > 0) {
            if(ResumeFlag)
                data.append(socket->read(socket->bytesAvailable()));
            else
                data = socket->read(socket->bytesAvailable());
        }
        parsData(data);
    }
    else
    {
        if(MTU < 1400)
        {
            static QMap<quint16, PacketData> incompleteMessages;
            {
                QByteArray datagram;
                datagram.resize(socketUDP->pendingDatagramSize());

                QHostAddress sender;
                quint16 senderPort;
                socketUDP->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

                quint16 identifier;
                memcpy(&identifier, datagram.data(), sizeof(identifier));
                datagram.remove(0, sizeof(identifier));

                int sequenceNumber;
                memcpy(&sequenceNumber, datagram.data(), sizeof(sequenceNumber));
                datagram.remove(0, sizeof(sequenceNumber));

                int totalPackets;
                memcpy(&totalPackets, datagram.data(), sizeof(totalPackets));
                datagram.remove(0, sizeof(totalPackets));

                if (!incompleteMessages.contains(identifier)) {
                    incompleteMessages[identifier] = PacketData{totalPackets, QMap<int, QByteArray>()};
                }
                incompleteMessages[identifier].fragments[sequenceNumber] = datagram;

                if (incompleteMessages[identifier].fragments.size() == totalPackets) {
                    QByteArray completeData;
                    for (int i = 0; i < totalPackets; ++i) {
                        completeData.append(incompleteMessages[identifier].fragments[i]);
                    }

                    //qDebug() << "Received complete data:" << completeData.size();
                    parsData(completeData);
                    incompleteMessages.remove(identifier);
                }
            }
        }
        else
        {
            static thread_local QByteArray buf(9000, Qt::Uninitialized);

            while (socketUDP->hasPendingDatagrams()) {

                const qint64 pending = socketUDP->pendingDatagramSize();
                if (pending > buf.size())
                    buf.resize(static_cast<int>(pending));
                QHostAddress sender;
                quint16 senderPort;
                const qint64 n = socketUDP->readDatagram(buf.data(), buf.size(), &sender, &senderPort);
                data.append(buf);

            }
            parsData(data);
            // if(!ResumeFlag)
            data.clear();
        }

    }
}
void CDFSocket::readSerialData()
{
    buffer.append(serialPort->readAll());

    while(buffer.size() > 5)
    {
        if (buffer[0] == (char)0x23 && buffer[1] == (char)0x35)
        {
            QByteArray packet;
            char id = buffer.at(2);
            int index = 5;
            ushort len = 0;
            ushort hlen = 0;
            len = (buffer.at(3) & 0xff);
            hlen = (buffer.at(4) << 8);
            len |= hlen;
            if(len == 1)
            {
                buffer.remove(0,5);
                return;
            }
            if(buffer.length() >= len + 7)
                packet = buffer.left(len + 7);
            else
            {return;}
            if(packet.at(len + index) == FOOTER1 && packet.at(len + index +1 ) == FOOTER2)
            {
                ResumeFlag = false;
                switch (id) {
                case PROTOID:
                    ParseProtoData(packet.mid(index,len));
                    break;
                case GPSP:
                    GPSdata(packet.mid(index,len));
                    break;
                case COM:
                    Compassdata(packet.mid(index,len));
                    break;
                case PSDID:
                    PSDdata(packet.mid(index,len));
                    break;
                case FPSDID:
                    FULLPSDdata(data.mid(index,len));
                    break;
                case DOAID:
                    DoaData(packet.mid(index,len));
                    break;
                case DOACID:
                    DoaComData(data.mid(index,len));
                    break;
                case IQPDWID:
                    IQPDWData(data.mid(index,len));
                    break;
                case PDWID:
                    PDWData(packet.mid(index,len));
                    break;
                case PSDW:
                    PSDdataWide(packet.mid(index,len));
                    break;
                case DFPAR:
                    DFResult(packet.mid(index,len));
                    break;
                case AUDIO:
                    AudioData(packet.mid(index,len));
                    break;
                case ANT:
                    ANTData(packet.mid(index,len));
                    break;
                case TRG:
                    Targetdata(packet.mid(index,len));
                    break;
                case SCANRES:
                    ScanResultdata(packet.mid(index,len));
                    break;
                case AZIMITHS:
                    AzimuthsData(packet.mid(index,len));
                    break;
                case BITE:
                    BiteTestRes(packet.mid(index,len));
                    break;
                case CLASS:
                    Classificationdata(data.mid(index,len));
                    break;
                case CLASSE:
                    ClassificationdataElint(data.mid(index,len));
                    break;
                case AMRS:
                    AMRDataStereo(data.mid(index,len));
                    break;
                case AMRM:
                    AMRDataMono(data.mid(index,len));
                    break;
                case DOAHISID:
                    DoaHistData(data.mid(index,len));
                    break;
                case AZWIDID:
                    AzimuthAreaResult(data.mid(index,len));
                    break;
                case MXHLDID:
                     MaxHoldData(data.mid(index,len));
                    break;
                }
                buffer.remove(0, len);
            }
            else
            {buffer.remove(0, 1);}
        }
        else
            buffer.remove(0, 1);
    }

}
void CDFSocket::parsData(QByteArray &data)
{

    for(int i = 0 ;i < data.size();i++)
    {
        if(i + 4 >= data.size())
            break;
        if(data.at(i) == 0x23 && data.at(i+1) == 0x35)
        {
            int index = i + 2;
            char id = data.at(i+2);
            index++;
            ushort len = 0;
            ushort hlen = 0;
            len = (data.at(i+3) & 0xff); index++;
            hlen = (data.at(i+4) << 8); index++;
            len |= hlen;
            if(len + index + 1 >= data.size())
            {
                //  qDebug() << len + index << "  error2  " << data.size();
                data = data.mid(index - 5,data.size());
                ResumeFlag = true;
                break;
            }
            if(data.at(len + index) == FOOTER1 && data.at(len + index + 1) == FOOTER2)
            {
                ResumeFlag = false;
                switch (id) {
                case PROTOID:
                    ParseProtoData(data.mid(index,len));
                    break;
                case GPSP:
                    GPSdata(data.mid(index,len));
                    break;
                case COM:
                    Compassdata(data.mid(index,len));
                    break;
                case PSDID:
                    PSDdata(data.mid(index,len));
                    break;
                case FPSDID:
                    FULLPSDdata(data.mid(index,len));
                    break;
                case DOAID:
                    DoaData(data.mid(index,len));
                    break;
                case DOACID:
                    DoaComData(data.mid(index,len));
                    break;
                case IQPDWID:
                    IQPDWData(data.mid(index,len));
                    break;
                case PDWID:
                    PDWData(data.mid(index,len));
                    break;
                case PSDW:
                    PSDdataWide(data.mid(index,len));
                    break;
                case DFPAR:
                    DFResult(data.mid(index,len));
                    break;
                case AUDIO:
                    AudioData(data.mid(index,len));
                    break;
                case ANT:
                    ANTData(data.mid(index,len));
                    break;
                case TRG:
                    Targetdata(data.mid(index,len));
                    break;
                case SCANRES:
                    ScanResultdata(data.mid(index,len));
                    break;
                case AZIMITHS:
                    AzimuthsData(data.mid(index,len));
                    break;
                case BITE:
                    BiteTestRes(data.mid(index,len));
                    break;
                case CLASS:
                    Classificationdata(data.mid(index,len));
                    break;
                case CLASSE:
                    ClassificationdataElint(data.mid(index,len));
                    break;
                case AMRS:
                    AMRDataStereo(data.mid(index,len));
                    break;
                case AMRM:
                    AMRDataMono(data.mid(index,len));
                    break;
                case DOAHISID:
                    DoaHistData(data.mid(index,len));
                    break;
                case AZWIDID:
                    AzimuthAreaResult(data.mid(index,len));
                    break;
                case MXHLDID:
                    MaxHoldData(data.mid(index,len));
                    break;
                }
            }
            i = len + index;
        }
    }
    //data.clear();
}
void CDFSocket::DoaData(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());
    int shortArraySize = (data.size() -sizeof(double)) / sizeof(short);
    std::vector<double> DoaDt(shortArraySize);
    for(int i = 0; i < shortArraySize;i++)
        DoaDt[i] = shortArray[i] /10;
    int offset = shortArraySize * sizeof(short);
    const double *TIME  = reinterpret_cast<const double*>(data.constData() + offset);
    const double time = TIME[0];
    emit XferDOA(DoaDt.data(),time,shortArraySize);
}

void CDFSocket::MaxHoldData(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());
    int shortArraySize = data.size() / sizeof(short);
    std::vector<double> MaxHold(shortArraySize);
    for(int i = 0; i < shortArraySize;i++)
        MaxHold[i] = shortArray[i] /10;
    emit XferMaxHold(MaxHold.data(),shortArraySize);
}
void CDFSocket::DoaHistData(const QByteArray &data)
{
    const uint16_t *shortArray = reinterpret_cast<const uint16_t *>(data.constData());
    int shortArraySize = data.size() / sizeof(short);
    std::vector<double> DoaDt(shortArraySize);
    for(int i = 0; i < shortArraySize;i++)
        DoaDt[i] = shortArray[i];
    emit XferHistDOA(shortArray,shortArraySize);
}
void CDFSocket::DoaComData(const QByteArray &data)
{ 
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_5_15);

    double timeTag;
    short len;
    QMap<short, short> doa;

    stream >> timeTag;
    stream >> len;
    stream >> doa;

    std::fill(DoaDtCompressed, DoaDtCompressed + 2000, -1.0);

    for (auto i = doa.cbegin(), end = doa.cend(); i != end; ++i)
        DoaDtCompressed[i.key()] = i.value();

    emit XferDOA(DoaDtCompressed,timeTag,len);
}
void CDFSocket::DFResult(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());
    DFResultStruct dfs;
    dfs.azimuth = (double)(shortArray[0] / 10);
    dfs.level = (double)(shortArray[1] / 10);;
    dfs.quality = shortArray[2];

    emit XferDFResult(dfs);
}

void CDFSocket::AzimuthAreaResult(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());
    double azimuth = (double)(shortArray[0] / 10);

    emit XferAzimuthArea(azimuth);
}
void CDFSocket::PSDdata(const QByteArray &data)
{
    const char *byteArray = reinterpret_cast<const char *>(data.constData());
    int byteArraySize = data.size();// / sizeof(short);
    std::vector<double> PSDDt(byteArraySize);
    for(int i = 0; i < byteArraySize;i++)
        PSDDt[i] = (double)(byteArray[i]  - SHIFTPSD);
    //PSDDt[i] = (double)(shortArray[i] /10);

    emit XferPSD(PSDDt.data(),byteArraySize);
}
void CDFSocket::PSDdataWide(const QByteArray &data)
{
    const char *byteArray = reinterpret_cast<const char *>(data.constData());
    int byteArraySize = data.size();// / sizeof(short);
    std::vector<double> PSDDt(byteArraySize);
    for(int i = 0; i < byteArraySize;i++)
        PSDDt[i] = (double)(byteArray[i]  - SHIFTPSD);
    //PSDDt[i] = (double)(byteArray[i] /10);

    emit XferPSDWide(PSDDt.data(),byteArraySize);
}
void CDFSocket::PDWData(const QByteArray &data)
{
    QDataStream ds(data);
    ds.setByteOrder(QDataStream::BigEndian);
    ds.setVersion(QDataStream::Qt_5_15);
    if (ds.atEnd()) return;

    quint32 Count = 0;
    ds >> Count;

    if (Count > 64)
        Count = 64;

    for (quint32 i = 0; i < Count; ++i) {
        if (ds.atEnd()) break;



        qint16  q_Quality    = 0;
        qint8   q_FilterType = 0;
        quint64 q_TOA        = 0;
        float   q_PW         = 0;
        qint16  q_PA         = 0;
        float  q_Freq        = 0;
        qint16  q_DOA        = 0;
        qint8   q_CW_ALARM   = 0;
        qint32  q_GPS        = 0;

        ds >> q_Quality;
        ds >> q_FilterType;
        ds >> q_TOA;
        ds >> q_PW;
        ds >> q_PA;
        ds >> q_Freq;
        ds >> q_DOA;
        ds >> q_CW_ALARM;
        ds >> q_GPS;

        if (ds.status() != QDataStream::Ok) break;

        pdw[i].Quality    = static_cast<qint16>(q_Quality);
        pdw[i].FilterType = static_cast<qint8>(q_FilterType);
        pdw[i].TOA        = static_cast<quint64>(q_TOA);
        pdw[i].PW         = static_cast<float>(q_PW);
        pdw[i].PA         = static_cast<qint16>(q_PA);
        pdw[i].Freq       = static_cast<float>(q_Freq);
        pdw[i].DOA        = static_cast<qint16>(q_DOA);
        pdw[i].CW_ALARM   = static_cast<qint8>(q_CW_ALARM);
        pdw[i].GPS        = static_cast<qint32>(q_GPS);
    }

    emit XferPDW(pdw,Count);
}

void CDFSocket::IQPDWData(const QByteArray &data)
{
    QDataStream ds(data);

    ds.setByteOrder(QDataStream::BigEndian);
    ds.setVersion(QDataStream::Qt_5_15);

    if(ds.atEnd())
        return;

    quint32 iqCount = 0;

    ds >> iqCount;

    if(ds.status() != QDataStream::Ok)
        return;

    QVector<IQSample> iqData(iqCount);

    int iqBytes = iqCount * sizeof(IQSample);

    if(ds.readRawData(reinterpret_cast<char*>(iqData.data()),iqBytes) != iqBytes)
        return;

    PulseParametersStruct pulse;

    quint64 q_TOA      = 0;
    float   q_PW       = 0;
    qint16  q_PA       = 0;
    float   q_Freq     = 0;
    qint16  q_DOA      = 0;
    qint8   q_CW_ALARM = 0;
    qint16  q_Quality  = 0;
    qint8   q_FilterType = 0;
    qint32  q_GPS      = 0;
    quint8  q_Counter  = 0;

    ds >> q_TOA;
    ds >> q_PW;
    ds >> q_PA;
    ds >> q_Freq;
    ds >> q_DOA;
    ds >> q_CW_ALARM;
    ds >> q_Quality;
    ds >> q_FilterType;
    ds >> q_GPS;
    ds >> q_Counter;

    if(ds.status() != QDataStream::Ok)
        return;

    pulse.TOA        = q_TOA;
    pulse.PW         = q_PW;
    pulse.PA         = q_PA;
    pulse.Freq       = q_Freq;
    pulse.DOA        = q_DOA;
    pulse.CW_ALARM   = q_CW_ALARM;
    pulse.Quality    = q_Quality;
    pulse.FilterType = q_FilterType;
    pulse.GPS        = q_GPS;
    pulse.Counter    = q_Counter;

    emit XferIQPDW(iqData, pulse);
}
void CDFSocket::FULLPSDdata(const QByteArray &data)
{
    const char *byteArray = reinterpret_cast<const char *>(data.constData());
    int byteArraySize = (data.size() - 2); /// sizeof(short);
    std::vector<double> PSDDt(byteArraySize);
    short index = byteArray[byteArraySize] + SHIFTPSD;
    short cnt = byteArray[byteArraySize + 1] + SHIFTPSD;
    for(int i = 0; i < byteArraySize;i++)
        PSDDt[i] = (double)(byteArray[i]  - SHIFTPSD);
    //PSDDt[i] = (double)(byteArray[i] /10);

    emit XferPSDFullRange(PSDDt.data(),index,cnt,byteArraySize);
}
void CDFSocket::GPSdata(const QByteArray &data)
{
    const double *doublearray = reinterpret_cast<const double *>(data.constData());

    gpsparams.Latitude = doublearray[0];
    gpsparams.Longitude = doublearray[1];
    emit GPSDataChanged(gpsparams);
}
void CDFSocket::ClassificationdataElint(const QByteArray &data)
{
    QVector<SGroup> groups;

    QDataStream ds(data);
    ds.setByteOrder(QDataStream::BigEndian);
    ds.setVersion(QDataStream::Qt_5_15);

    if (ds.atEnd()) return;

    quint32 groupCount = 0;
    ds >> groupCount;
    groups.reserve(groupCount);

    for (quint32 i = 0; i < groupCount; ++i) {
        SGroup g;
        qint32 cnt = 0;
        ds >> cnt;
        g.Cnt = cnt;
        ds >> g.Sum;
        ds >> g.Mean;
        ds >> g.Std;

        // Freq
        quint32 fsz = 0;
        ds >> fsz;
        g.Freq.reserve(fsz);
        for (quint32 k = 0; k < fsz; ++k) {
            double f; ds >> f;
            g.Freq.push_back(f);
        }

        // GPS
        quint32 gpsz = 0;
        ds >> gpsz;
        g.GPS.reserve(gpsz);
        for (quint32 k = 0; k < gpsz; ++k) {
            double v; ds >> v;
            g.GPS.push_back(/*static_cast<int>*/(v));
        }

        groups.push_back(std::move(g));
    }
    emit ClassificationResultElint(groups,groupCount);

}

void CDFSocket::AMRDataStereo(const QByteArray &data)
{
    structAMRResultStereo d;

    QDataStream in(data);
    in.setVersion(QDataStream::Qt_5_15);

    in.readRawData(d.MRModulation, 50);

    in >> d.MRModulationAverageAcc;
    in >> d.fs;
    in >> d.symbol_rate;

    for (int i = 0; i < NUM_CLASSES; i++)
        in.readRawData(d.instantModulation[i], 50);

    for (int i = 0; i < NUM_CLASSES; i++)
        in >> d.instantAccuracy[i];

    emit AmrStereoDataChanged(d);
}
void CDFSocket::AMRDataMono(const QByteArray &data)
{
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_5_15);

    int count = 0;
    in >> count;

    std::vector<AmrTargetOutputSimple> result;
    result.resize(count);

    for (int idx = 0; idx < count; idx++)
    {
        auto &d = result[idx];

        in.readRawData(d.MRModulation, 50);

        in >> d.MRModulationAverageAcc;

        for (int i = 0; i < NUM_CLASSES; i++)
            in.readRawData(d.instantModulation[i], 50);

        for (int i = 0; i < NUM_CLASSES; i++)
            in >> d.instantAccuracy[i];

        in.readRawData(d.id, 50);

        in >> d.fb_iq;
        in >> d.fc_iq;
        in >> d.decimation_iq;
        in >> d.fs_iq;
        in >> d.bandwidth_iq;
        in >> d.cutoff_iq;

        quint64 bufsize;
        in >> bufsize;
        d.buffer_size = bufsize;

        in >> d.seen_count;

        qint64 tbuf;
        in >> tbuf;
        d.timeBuffer = tbuf;

        in >> d.snr;
        in >> d.sig_status;
        in >> d.symbol_rate;
    }

    emit AmrMonoDataChanged(result);
}
void CDFSocket::Classificationdata(const QByteArray &data)
{
    ClassifiedSignalStruct classi;
    const char* raw = data.constData();
    int offset = 0;
    const short* shorts = reinterpret_cast<const short*>(raw + offset);
    classi.Fmin = shorts[0];
    classi.Fmax = shorts[1];
    offset += 2 * sizeof(short);
    const double* doubles = reinterpret_cast<const double*>(raw + offset);
    classi.Direction = doubles[0];
    offset += 1 * sizeof(double);
    const char* c = reinterpret_cast<const char*>(raw + offset);
    classi.sig = (SignalClassified)*c;
    offset += sizeof(char);
    emit ClassificationResult(classi);



}
void CDFSocket::AzimuthsData(const QByteArray &data)
{
    const double *doublearray = reinterpret_cast<const double *>(data.constData());
    Azimuthsparams.PureAzimuth = doublearray[0];
    Azimuthsparams.AfterRevertCenterDoa = doublearray[1];
    Azimuthsparams.AfterNorthOffset = doublearray[2];
    Azimuthsparams.AfterMechanicalOffset = doublearray[3];
    Azimuthsparams.AfterHeadingCenterDoa = doublearray[4];
    Azimuthsparams.NorthOffset = doublearray[5];
    Azimuthsparams.MechanicalOffset = doublearray[6];
    Azimuthsparams.Heading = doublearray[7];
    emit AzimuthsMonitoringChanged(Azimuthsparams);
}

void CDFSocket::BiteTestRes(const QByteArray &data)
{
    emit BiteTestResult(data);
}
void CDFSocket::ScanResultdata(const QByteArray &data)
{
    const double *doublearray = reinterpret_cast<const double *>(data.constData());

    ScanRes.Freq = doublearray[0];
    ScanRes.Azimuth = doublearray[1];
    emit ScanResultData(ScanRes);
}
void CDFSocket::Targetdata(const QByteArray &data)
{
    const double *doublearray = reinterpret_cast<const double *>(data.constData());
    TargetData.Signalmode = (ClassificationDataRdy_SINGALMODE)doublearray[0];
    TargetData.StartFreq = doublearray[1];
    TargetData.StopFreq = doublearray[2];
    TargetData.LatFirst = doublearray[3];
    TargetData.LongFirst = doublearray[4];
    TargetData.TargetLat = doublearray[5];
    TargetData.TargetLong = doublearray[6];
    TargetData.Distance = doublearray[7];

    emit TargetAreaAndPositionChanged(TargetData);
}
void CDFSocket::Compassdata(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());

    gpsparams.Heading = shortArray[0];
    gpsparams.Roll = shortArray[1];
    gpsparams.pitch = shortArray[2];
    gpsparams.Speed = shortArray[3];
    gpsparams.Altitude = shortArray[4];
    gpsparams.IsLock = shortArray[5];
    emit CompassDataChanged(gpsparams);
}
void CDFSocket::ANTData(const QByteArray &data)
{
    const short *shortArray = reinterpret_cast<const short *>(data.constData());

    std::vector<double> AntennaMesuerdPHi(shortArray[0]);
    std::vector<double> AntennaPhase(shortArray[1]);
    std::vector<double> AntennaAmp(shortArray[2]);
    int i = 3;
    short cnt = 0;
    for(; i < shortArray[0] + 3;i++)
        AntennaMesuerdPHi[cnt++] = (double)(shortArray[i] /10);
    i = shortArray[0] + 3;
    cnt = 0;
    for(; i < shortArray[0] + shortArray[1] + 3 ;i++)
        AntennaPhase[cnt++] = (double)(shortArray[i] /10);
    i = shortArray[0] + shortArray[1] +3;
    cnt = 0;
    for(; i < shortArray[0] + shortArray[1] + shortArray[2] +3 ;i++)
        AntennaAmp[cnt++] = (double)(shortArray[i] /10);

    emit XferPhaseAmp(AntennaMesuerdPHi.data(),shortArray[0],AntennaPhase.data(),shortArray[1],AntennaAmp.data(),shortArray[2]);
}

void CDFSocket::AudioData(const QByteArray &data)
{
    // const char *chararray = reinterpret_cast<const char *>(data.constData());
    // int shortArraySize = data.size();
    if(acs.IsAudioCodeEnable)
        if(acs.CodeTools == AudioCodec_CodecTools__opus)
        {

            QByteArray pcm = opusDecoder->decode(data);
            emit XferAudioData(pcm,pcm.size());
            return;
        }
    emit XferAudioData(data,data.size());

}
void CDFSocket::ParseProtoData(const QByteArray &data)
{
    SystemParameters dummy;
    dummy.ParseFromArray(data.data(),data.size());
    if(GetFrequency(data,dummy.name()))
        qDebug() << "Freq Receive";
    if(GetThreshold(data,dummy.name()))
        qDebug() << "Threshold Recieve";
    if(GetSystemParams(data,dummy.name()))
        qDebug() << "System param receive";
    if(GetSystemParamsElint(data,dummy.name()))
        qDebug() << "System param Elint receive";
    if(GetDFParameters(data,dummy.name()))
        qDebug() << "Received DF Params";
    if(GetAntennaOffset(data,dummy.name()))
        qDebug() << "Received Antenna Offset";
    if(GetAntennaState(data,dummy.name()))
        qDebug() << "Received Antenna State";
    if(GetTransferDataRules(data,dummy.name()))
        qDebug() << "Received DataTransfer";
    if(GetGPSParams(data,dummy.name()))
        qDebug() << "GPS Data Rdy";
    if(GetAntennaParameters(data,dummy.name()))
        qDebug() << "Received Antenna Parameters";
    if(GetAudioState(data,dummy.name()))
        qDebug() << "Received Audio Parameters";
    if(GetComputerState(data,dummy.name()))
        qDebug() << "Received Computers Parameters";
    if(GetDFLogParams(data,dummy.name()))
        qDebug() << "Received DFLOG";
    if(GetManualmode(data,dummy.name()))
        qDebug() << "Received ManualModeFreq";
    if(GetDFmodeFinding(data,dummy.name()))
        qDebug() << "Received DFModeFinding";
    if(GetPositionFinding(data,dummy.name()))
        qDebug() << "Received PositionFinding";
    if(GetMemoryScanParam(data,dummy.name()))
        qDebug() << "Received MemoryScan Param";
    if(GetMemoryScanState(data,dummy.name()))
        qDebug() << "Received MemoryScan State";
    if(GetSubRangeScanParam(data,dummy.name()))
        qDebug() << "Received SubRangeScan Param";
    if(GetSubRangeScanState(data,dummy.name()))
        qDebug() << "Received SubRangeScan State";
    if(GetFullRangeScanParam(data,dummy.name()))
        qDebug() << "Received FullRangeScan Param";
    if(GetRemoveScanTable(data,dummy.name()))
        qDebug() << "Received Remove Scan Table";
    if(GetAllConfig(data,dummy.name()))
        qDebug() << "Received Get all config";
    if(GetResetSystem(data,dummy.name()))
        qDebug() << "Received Reset System";
    if(GetVersions(data,dummy.name()))
        qDebug() << "Received Version";
    if(GetError(data,dummy.name()))
        qDebug() << "Received Error";
    if(GetCalibrationCore(data,dummy.name()))
        qDebug() << "Received Calibration";
    if(GetBiteState(data,dummy.name()))
        qDebug() << "Received BiteState";
    if(GetAmrState(data,dummy.name()))
        qDebug() << "Received Amr State";
    if(GetClassificationState(data,dummy.name()))
        qDebug() << "Received ClassificationState";
    if(GetIFlogState(data,dummy.name()))
        qDebug() << "Received IFLogState";
    if(GetAudioCodec(data,dummy.name()))
        qDebug() << "Received AudioCodec";
    if(GetSystemState(data,dummy.name()))
        qDebug() << "Received SystemState";
    if(GetAzimuthHistogramMode(data,dummy.name()))
        qDebug() << "Received AzimuthHistogramMode";
    if(GetAzimuthBlankings(data,dummy.name()))
        qDebug() << "Received AzimuthBlankings";
    if(GetFrequencyOffset(data,dummy.name()))
        qDebug() << "Received FrequencyOffset";
    if(GetFrequencyOffsetMode(data,dummy.name()))
        qDebug() << "Received FrequencyOffsetMode";
    if(GetIQPDWState(data,dummy.name()))
        qDebug() << "Received IQPDWMode";
    if(GetMaxHoldParam(data,dummy.name()))
        qDebug() << "Received MaxHoldParams";

}
void CDFSocket::SendSystemParams()
{
    QByteArray data;
    SystemParam.set_name(_SYSPARAM);
    data.resize(SystemParam.ByteSizeLong());
    SystemParam.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendSystemElintParams()
{
    QByteArray data;
    SystemParamElint.set_name(_SYSPARAMELINT);
    data.resize(SystemParamElint.ByteSizeLong());
    SystemParamElint.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendByComport(const QByteArray& data)
{
    const int chunkSize = 9;

    for (int offset = 0; offset < data.length(); offset += chunkSize)
    {
        QByteArray chunk = data.mid(offset, chunkSize);
        //uint8_t chunksize  = chunk.length();
        if (chunk.length() < chunkSize)
        {
            chunk.append(QByteArray(chunkSize - chunk.length(), 0x00));
        }

        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::BigEndian);

        QByteArray parameters;

        ds << (quint8)(0xAA);  // Header byte 1
        ds << (quint8)(0xAA);  // Header byte 2
        ds << (quint8)(0x5D);  // Header byte 3
        ds << (quint8)(0x0B);  // Length byte (can be adjusted as needed)
        quint8 mSubsystemId = 0x20;
        if(counter == 255)
            counter = 0;
        parameters.append((quint8)counter++);
        parameters.append((quint8)(mSubsystemId));
        parameters.append(chunk);

        auto dataPtr = parameters.constData();
        ds.writeRawData(dataPtr, parameters.length());

        // Calculate checksum
        quint16 checksum = 0;
        for (int i = 0; i < parameters.length(); i++)
        {
            checksum += (quint8)parameters[i];
            //qDebug() << checksum;
        }
        //qDebug() << "checksum" <<  checksum;
        ds << (quint16)(checksum);  // Add checksum;

        // Emit the data to the COM port
        serialPort->write(ba);
        serialPort->waitForBytesWritten(0);
        commandcounter++;
    }
}
void CDFSocket::    SendData(QByteArray dt)
{
    const QByteArray data = framePayload(dt);
    if(UartMode)
    {
        QByteArray finaldata = dt;
        finaldata.append(0x55); // Append delimiter
        finaldata.append(0xAA); // Append delimiter
        //finaldata.append(0xBB); // Append delimiter
        //serialPort->write(finaldata);
        commandcounter = 0;
        SendByComport(finaldata);
    }
    else if(UDPModeState && !UartMode)
    {
        QHostAddress clientAddress = QHostAddress(UDPSendIP);
        if(socketUDP != nullptr)
            socketUDP->writeDatagram(data,clientAddress,UDPSendPort);
    }
    else
    {
        if(socket != nullptr){
            socket->write(data);
            socket->waitForBytesWritten(100);
        }
    }
    //socket->flush();
}
bool CDFSocket::GetSystemParams(QByteArray data,string name)
{
    if(name == _SYSPARAM)
    {
        SystemParam.ParseFromArray(data.data(),data.size());

        Sysparam.bandwidth = SystemParam.bandwidth();
        Sysparam.Fft =SystemParam.fft();
        Sysparam.SetSquelch = SystemParam.setsquelch();
        Sysparam.agcData = SystemParam.agcdata();
        Sysparam.demod = SystemParam.demod();
        Sysparam.filter = SystemParam.filter();
        Sysparam.mgcData = SystemParam.mgcdata();
        Sysparam.rfhead = SystemParam.rfhead();
        Sysparam.volume = SystemParam.volume();
        Sysparam.Average = SystemParam.average();
        Sysparam.PhaseAverage = SystemParam.phaseaverage();
        Sysparam.AutoThreshold = SystemParam.autothreshold();
        Sysparam.AzimuthHistogram = SystemParam.azimuthhistogram();
        Sysparam.Quality = SystemParam.quality();
        Sysparam.AnalogFilterBand = SystemParam.analogfilter();
        Sysparam.AntPolarization = SystemParam.antennapolarization();
        Sysparam.AntAmp = SystemParam.antennaamp();
        Sysparam.AntLength = SystemParam.antennalength();
        Sysparam.RFAttenator = SystemParam.rfattenator();
        Sysparam.IFAttenator = SystemParam.ifattenator();
        emit SystemParamEnvet(Sysparam);
        return true;
    }
    return false;
}
bool CDFSocket::GetSystemParamsElint(QByteArray data, string name)
{
    if(name == _SYSPARAMELINT)
    {
        SystemParamElint.ParseFromArray(data.data(),data.size());
        SysparamElint.ElintBandwidth = SystemParamElint.elintbandwidth();
        SysparamElint.ADCHighRef = SystemParamElint.adchighref();
        SysparamElint.ADCLowRef = SystemParamElint.adclowref();
        SysparamElint.ThresholdHigh = SystemParamElint.thresholdhigh();
        SysparamElint.ThresholdLimit = SystemParamElint.thresholdlimit();
        SysparamElint.ElintMaxhold = SystemParamElint.elintmaxhold();
        SysparamElint.ThresholdLow = SystemParamElint.thresholdlow();
        emit SystemParamElintEnvet(SysparamElint);
        return true;
    }
    return false;
}
bool CDFSocket::GetFrequency(QByteArray data, string name)
{
    if(name == _Frequency)
    {
        fr.ParseFromArray(data.data(),data.size());
        emit FrequencyChanged(fr.freq());
        return true;
    }
    return false;

}
bool CDFSocket::GetThreshold(QByteArray data, string name)
{
    if(name == _Threshold)
    {
        trh.ParseFromArray(data.data(),data.size());
        emit ThresholdChanged(trh.thr());
        return true;
    }
    return false;
}

bool CDFSocket::GetAntennaOffset(QByteArray data, string name)
{

    if(name == _AntOffset)
    {
        af.ParseFromArray(data.data(),data.size());
        QMap<char,double> offset;

        offset.insert(ADD050,af.add050());
        offset.insert(ADD070,af.add070());
        offset.insert(ADD078,af.add078());
        offset.insert(ADD107,af.add107());
        offset.insert(ADD150,af.add150());
        offset.insert(ADD207,af.add207());
        offset.insert(ADD226,af.add226());
        offset.insert(ADD327,af.add327());
        offset.insert(ADD297,af.add297());

        emit AntennaOffsetChanged(offset);
        return true;
    }
    return false;
}
bool CDFSocket::GetAntennaState(QByteArray data, string name)
{

    if(name == _AntState)
    {
        as.ParseFromArray(data.data(),data.size());
        QMap<char,bool> state;

        state.insert(ADD050,as.add050());
        state.insert(ADD070,as.add070());
        state.insert(ADD078,as.add078());
        state.insert(ADD107,as.add107());
        state.insert(ADD150,as.add150());
        state.insert(ADD207,as.add207());
        state.insert(ADD226,as.add226());
        state.insert(ADD327,as.add327());
        state.insert(ADD297,as.add297());

        emit AntennaStatesChanged(state,as.autodetect(),as.isdualpol());
        return true;
    }
    return false;
}

bool CDFSocket::GetTransferDataRules(QByteArray data, string name)
{
    if(name == _TransferData)
    {
        td.ParseFromArray(data.data(),data.size());
        DataTransferS.TransferAudio = td.transferaudio();
        DataTransferS.TransferDFParameters = td.transferdfparameters();
        DataTransferS.TransferDOA = td.transferdoa();
        DataTransferS.TransferDatarange = td.transferdatarange();
        DataTransferS.TransferPSD = td.transferpsd();
        DataTransferS.TransferAntennaResult = td.transferantennaresult();
        DataTransferS.TransferGPSParameters = td.transfergpsparameters();
        DataTransferS.TransferCompassParameters = td.transfercompassparameters();
        DataTransferS.TransferAzimuthCalculationParameters = td.transferazimuthcalculation();
        DataTransferS.TransferError = td.transfererror();
        DataTransferS.TransferFixWideBandPSDData = td.transferfixwidebandpsddata();
        DataTransferS.TransferPDWData = td.transferpdwdata();
        DataTransferS.TransferIQData = td.transferiqdata();
        DataTransferS.TransferIFData = td.transferifdata();
        DataTransferS.TransferAmrData = td.transferamrdata();
        DataTransferS.TransferHistogramDoa = td.transferhistogramdoa();
        emit TransferDataRate(DataTransferS);
        return true;
    }
    return false;
}

bool CDFSocket::GetGPSParams(QByteArray data, string name)
{
    if(name == _GPSPARAMs)
    {
        gp.ParseFromArray(data.data(),data.size());
        gpsparams.Latitude = gp.latitude();
        gpsparams.Longitude = gp.longitude();
        gpsparams.AutoGPSDetect = gp.autogps();
        gpsparams.AutoCompassDetect = gp.autogps();
        gpsparams.AngleOffset = gp.angleoffset();
        gpsparams.NorthOffset = gp.northoffset();
        gpsparams.Baudrate = gp.baudrate();
        gpsparams.Framework = gp.framework();
        gpsparams.IsFpgaUart = gp.isfpgauart();
        gpsparams.PortName = gp.comport().c_str();
        emit GPSParametersChanged(gpsparams);
        return true;
    }
    return false;
}
bool CDFSocket::GetAntennaParameters(QByteArray data, string name)
{
    if(name == _AntParam)
    {
        ap.ParseFromArray(data.data(),data.size());
        ao.PageNumbers = ap.pagenumber();
        ao.ChannelNumebr = ap.channelnumber();
        ao.SwitchSequnece = ap.switchsequence();
        ao.ElintMode = ap.elint();
        emit PageNumberChanged(ao.PageNumbers);
        QThread::msleep(100);
        emit ChannelNumberChanged(ao.ChannelNumebr);
        QThread::msleep(100);
        emit SwitchSequenceChanged(ao.SwitchSequnece);
        QThread::msleep(100);
        emit ElintAntennaChanged(ao.ElintMode);
        return true;
    }
    return false;

}

bool CDFSocket::GetAudioState(QByteArray data, string name)
{
    if(name == _AudioParam)
    {
        audios.ParseFromArray(data.data(),data.size());
        audioState = audios.audio();
        emit AudioParamChanged(audioState);
        return true;
    }
    return false;
}

bool CDFSocket::GetComputerState(QByteArray data, string name)
{
    if(name == _ComputerParam)
    {
        cp.ParseFromArray(data.data(),data.size());
        ComputerParams.IpAddress = QString::fromStdString(cp.ipaddress());
        ComputerParams.LanName = QString::fromStdString(cp.lanname());
        ComputerParams.mtu = cp.mtu();
        ComputerParams.port = cp.port();
        emit ComputerParametersChanged(ComputerParams);
        return true;
    }
    return false;
}
bool CDFSocket::GetDFLogParams(QByteArray data, string name)
{
    if(name == _DFLog)
    {
        dfl.ParseFromArray(data.data(),data.size());
        DFlogS.state = dfl.state();
        DFlogS.params = dfl.params();
        DFlogS.loglocation = dfl.addressfile().c_str();
        emit DFLogChanged(DFlogS);
        return true;
    }
    return false;
}

bool CDFSocket::GetManualmode(QByteArray data, string name)
{
    if(name == _ManualModeFreq)
    {
        mmp.ParseFromArray(data.data(),data.size());
        ManualMode.state = mmp.state();
        ManualMode.StartIndex = mmp.startindex();
        ManualMode.StopIndex = mmp.stopindex();
        emit ManualModeChanged(ManualMode);
        return true;
    }
    return false;
}

bool CDFSocket::GetDFmodeFinding(QByteArray data, string name)
{
    if(name == _DFModeFinding)
    {
        dmf.ParseFromArray(data.data(),data.size());
        DFmode = dmf.mode();
        emit DFModeChanged(DFmode);
        return true;
    }
    return false;
}
bool CDFSocket::GetPositionFinding(QByteArray data, string name)
{
    if(name == _PositionFinding)
    {
        pfm.ParseFromArray(data.data(),data.size());
        PFparam.state = pfm.state();
        PFparam.alg = pfm.alg();
        emit PositionFindingModeChanged(PFparam);
        return true;
    }
    return false;
}
bool CDFSocket::GetMemoryScanParam(QByteArray data, string name)
{
    if(name == _MemoryScanParam)
    {
        msp.ParseFromArray(data.data(),data.size());
        msps.Freq = msp.frequency();
        msps.Threshold = msp.level();
        emit MemoryScanParamChanged(msps);
        return true;
    }
    return false;
}

bool CDFSocket::GetMemoryScanState(QByteArray data, string name)
{
    if(name == _MemoryScanState)
    {
        mss.ParseFromArray(data.data(),data.size());
        msss.State = mss.state();
        msss.time = mss.time();
        emit MemoryScanStateChanged(msss);
        return true;
    }
    return false;
}

bool CDFSocket::GetSubRangeScanParam(QByteArray data, string name)
{
    if(name == _SubRangeScanParam)
    {
        ssp.ParseFromArray(data.data(),data.size());
        srsps.StartFrequencyMHz = ssp.startfrequencymhz();
        srsps.StopFrequencyMHz = ssp.stopfrequencymhz();
        srsps.StepFrequecnyKHz = ssp.stepfrequecnykhz();
        srsps.level = ssp.level();
        emit SubRangeScanParamChanged(srsps);
        return true;
    }
    return false;
}
bool CDFSocket::GetSubRangeScanState(QByteArray data, string name)
{
    if(name == _SubRangeScanState)
    {
        sss.ParseFromArray(data.data(),data.size());
        srsss.State = sss.state();
        srsss.time = sss.time();
        emit SubRangeScanStateChanged(srsss);
        return true;
    }
    return false;
}
bool CDFSocket::GetFullRangeScanParam(QByteArray data, string name)
{
    if(name == _FullRangeScan)
    {
        frsp.ParseFromArray(data.data(),data.size());
        frsps.State = frsp.state();
        frsps.StartFrequencyMHz= frsp.startfrequencymhz();
        frsps.StopFrequencyMHz= frsp.stopfrequencymhz();
        emit FullRangeParamChanged(frsps);
        return true;
    }
    return false;
}
bool CDFSocket::GetRemoveScanTable(QByteArray data, string name)
{
    if(name == _RemoveScanList)
    {
        ScanTable = rst.removedata();
        emit RemoveScanlistChanged(ScanTable);
        return true;
    }
    return false;
}
bool CDFSocket::GetAllConfig(QByteArray data, string name)
{
    Q_UNUSED(data);
    if(name == _GetAllSystem)
    {
        emit GetAllDFConfigEvent();
        return true;
    }
    return false;
}

bool CDFSocket::GetResetSystem(QByteArray data, string name)
{
    if(name == _ResetSystemConfig)
    {
        rsd.ParseFromArray(data.data(),data.size());
        emit ResetSystemConfigEvent(rsd.resetmode());
        return true;
    }
    return false;
}

bool CDFSocket::GetVersions(QByteArray data, string name)
{
    if(name == _FileVersions)
    {
        fv.ParseFromArray(data.data(),data.size());
        emit VersionCommingChanged(fv.fileversion());
        return true;
    }
    return false;
}
bool CDFSocket::GetError(QByteArray data, string name)
{
    if(name == _ErrorsRequest)
    {
        se.ParseFromArray(data.data(),data.size());
        emit ErrorChanged(se.error());
        return true;
    }
    return false;
}

bool CDFSocket::GetCalibrationCore(QByteArray data, string name)
{
    if(name == _Calibration)
    {
        cl.ParseFromArray(data.data(),data.size());
        cls.CoreOn = cl.coreon();
        cls.gain = cl.gain();
        emit CalibrationParamChanged(cls);
        return true;
    }
    return false;
}
bool CDFSocket::GetClassificationState(QByteArray data, string name)
{
    if(name == _Classification)
    {
        cfs.ParseFromArray(data.data(),data.size());
        cs.State = cfs.state();
        cs.classificationAgl = cfs.algoritm();
        emit ClassificationStateChanged(cs);
        return true;
    }
    return false;
}

bool CDFSocket::GetBiteState(QByteArray data, string name)
{
    if(name == _biteTestState)
    {
        bte.ParseFromArray(data.data(),data.size());
        bool val = bte.state();
        emit BiteStatechanged(val);
        return true;
    }
    return false;
}

bool CDFSocket::GetIQPDWState(QByteArray data, string name)
{
    if(name == _IQPDWMode)
    {
        ipm.ParseFromArray(data.data(),data.size());
        bool val = ipm.enableiqpdwmode();
        emit IQPDWModeChanged(val);
        return true;
    }
    return false;
}

bool CDFSocket::GetAmrState(QByteArray data, string name)
{
    if(name == _amrstate)
    {
        amr.ParseFromArray(data.data(),data.size());
        bool val = amr.state();
        emit AmrStatechanged(val);
        return true;
    }
    return false;
}
bool CDFSocket::GetIFlogState(QByteArray data, string name)
{
    if(name == _IFlog)
    {
        ifr.ParseFromArray(data.data(),data.size());
        ifl.name = ifr.savename().c_str();
        ifl.state = ifr.state();
        ifl.timems = ifr.timems();

        emit IFLogParamtersChanged(ifl);
        return true;

    }
    return false;
}

bool CDFSocket::GetAudioCodec(QByteArray data, string name)
{
    if(name == _audiocodec)
    {
        acc.ParseFromArray(data.data(),data.size());
        acs.IsAudioCodeEnable = acc.state();
        acs.CodeTools = acc.codectools();
        acs.CodedPower = acc.codecpower();

        emit AudioCodecParamtersChanged(acs);
        return true;

    }
    return false;
}

bool CDFSocket::GetSystemState(QByteArray data, string name)
{
    if(name == _systemstate)
    {
        spm.ParseFromArray(data.data(),data.size());
        DFMode mode = spm.dfmod();
        emit SystemStateChanged(mode);
        return true;

    }
    return false;
}
bool CDFSocket::GetDFParameters(QByteArray data, string name)
{

    if(name == _SystemDefault)
    {
        def.ParseFromArray(data.data(),data.size());
        SystemDefualtsStruct sds;
        sds.SystemType = def.systemtype();
        sds.BandwidthSize = def.widebandbanwith();
        sds.MaxRangeFreq = def.maximumfreqrange();
        sds.MinRangeFreq = def.minimumfreqrange();
        sds.Is6GHz = def.is6ghz();
        sds.IsSuperResolation = def.issuperres();
        sds.Revert = def.revert();
        sds.DFtype = def.dfstate();
        qDebug() << sds.SystemType << sds.BandwidthSize << sds.MaxRangeFreq << sds.MinRangeFreq << sds.DFtype;
        emit XferDefault(sds);
        return true;
    }
    return false;
}
bool CDFSocket::GetAzimuthHistogramMode(QByteArray data, string name)
{
    if(name == _histogramMode)
    {
        ah.ParseFromArray(data.data(),data.size());
        ahs.Mode = ah.hist();
        ahs.State = ah.state();
        ahs.StartIndex = ah.startindex();
        ahs.StopIndex = ah.stopindex();
        ahs.MinDoa = ah.mindoa();
        ahs.MaxDoa = ah.maxdoa();

        emit AzimuthHistogramModeChanged(ahs);
        return true;

    }
    return false;
}

bool CDFSocket::GetAzimuthBlankings(QByteArray data, string name)
{
    if(name == _AzimuthBlanking)
    {
        QList<Azimuthsblankingstruct> list;

        AzimuthBlankings azimuthbs;
        if (!azimuthbs.ParseFromArray(data.data(),
                                      data.size()))
        {
            qDebug() << "Azimuth blankings parse failed";
            return false;
        }
        AzimuthBlankingState =  azimuthbs.state();
        for (int i = 0; i < azimuthbs.azimuthblankings_size(); i++)
        {
            const AzimuthBlanking& pbazimuth = azimuthbs.azimuthblankings(i);

            Azimuthsblankingstruct p;

            p.StartAzimuth = pbazimuth.startazimuth();
            p.StopAzimuth = pbazimuth.stopazimuth();
            list.append(p);
        }
        emit AzimuthBlankingsChanged(list,AzimuthBlankingState);
    }
    return false;
}

bool CDFSocket::GetFrequencyOffset(QByteArray data, string name)
{
    if(name == _FrequencyOffset)
    {
        QList<AzimuthFrequencystruct> list;

        FrequencyOffsets freqoffsets;
        if (!freqoffsets.ParseFromArray(data.data(),
                                        data.size()))
        {
            qDebug() << "Frequency offset parse failed";
            return false;
        }

        for (int i = 0; i < freqoffsets.frequencyoffsets_size(); i++)
        {
            const FrequencyOffset& offset = freqoffsets.frequencyoffsets(i);

            AzimuthFrequencystruct p;

            p.StartFrequencyMHz = offset.startfrequencymhz();
            p.StopFrequencyMHz = offset.stopfrequencymhz();
            p.Offset = offset.offset();
            list.append(p);
        }
        emit FrequencyOffsetChanged(list);
    }
    return false;
}

bool CDFSocket::GetFrequencyOffsetMode(QByteArray data, string name)
{
    if(name == _FrequencyOffsetMode)
    {
        fom.ParseFromArray(data.data(),data.size());
        OffsetMode= fom.offsetmode();

        emit FrequencyOffsetModeChanged(OffsetMode);
        return true;

    }
    return false;
}

bool CDFSocket::GetMaxHoldParam(QByteArray data, string name)
{
    if(name == _Maxhold)
    {
        mxh.ParseFromArray(data.data(),data.size());
        Maxholdparams.state = mxh.state();
        Maxholdparams.DelayTime = mxh.delaytime();
        Maxholdparams.RefreshTime = mxh.refreshtime();
        emit MaxholdParamChanged(Maxholdparams);
        return true;

    }
    return false;
}
void CDFSocket::SendAGc(char agc)
{
    Sysparam.agcData = (SystemParameters_AGC)agc;
    SystemParam.set_agcdata((SystemParameters_AGC)agc);
    SendSystemParams();
}
void CDFSocket::SendMGc(short mgc)
{
    Sysparam.mgcData = mgc;
    SystemParam.set_mgcdata(mgc);
    SendSystemParams();
}
void CDFSocket::SendRFHead(char rfhead)
{
    Sysparam.rfhead = (SystemParameters_RFHEAD)rfhead;
    SystemParam.set_rfhead((::SystemParameters_RFHEAD)rfhead);
    SendSystemParams();
}

void CDFSocket::SendRFIFAttenator(short rfatt, short ifatt)
{
    Sysparam.RFAttenator = rfatt;
    SystemParam.set_rfattenator(rfatt);
    Sysparam.IFAttenator = rfatt;
    SystemParam.set_ifattenator(ifatt);
    SendSystemParams();
}
void CDFSocket::SendDemod(char demod)
{
    Sysparam.demod = (SystemParameters_DEMOD)demod;
    SystemParam.set_demod((SystemParameters_DEMOD)demod);
    SendSystemParams();
}
void CDFSocket::SendFFTPoint(short fft)
{
    Sysparam.Fft = (SystemParameters_FFTPOINT)fft;
    SystemParam.set_fft((SystemParameters_FFTPOINT)fft);
    SendSystemParams();
}
void CDFSocket::SendSquelch(bool state)
{
    Sysparam.SetSquelch = state;
    SystemParam.set_setsquelch(state);
    SendSystemParams();
}
void CDFSocket::SendVolume(short vol)
{
    Sysparam.volume = vol;
    SystemParam.set_volume(vol);
    SendSystemParams();
}
void CDFSocket::SendAverage(double val)
{
    Sysparam.Average = val;
    SystemParam.set_average(val);
    SendSystemParams();
}

void CDFSocket::SendQuality(double val)
{
    Sysparam.Quality = val;
    SystemParam.set_quality(val);
    SendSystemParams();
}
void CDFSocket::SendPhaseAverage(double val)
{
    Sysparam.PhaseAverage = val;
    SystemParam.set_phaseaverage(val);
    SendSystemParams();
}

void CDFSocket::SendAutoThreshold(bool val)
{
    Sysparam.AutoThreshold = val;
    SystemParam.set_autothreshold(val);
    SendSystemParams();
}

void CDFSocket::SendAzimuthHistogram(bool val)
{
    Sysparam.AzimuthHistogram = val;
    SystemParam.set_azimuthhistogram(val);
    SendSystemParams();
}

void CDFSocket::SendClassificationState(ClassificationStruct Classsification)
{
    cfs.set_name(_Classification);
    cfs.set_state(Classsification.State);
    cfs.set_algoritm(Classsification.classificationAgl);
    QByteArray data;
    data.resize(cfs.ByteSizeLong());
    cfs.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendBiteState(bool val)
{
    bte.set_name(_biteTestState);
    bte.set_state(val);
    QByteArray data;
    data.resize(bte.ByteSizeLong());
    bte.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendIQPDWMode(bool val)
{
    ipm.set_name(_IQPDWMode);
    ipm.set_enableiqpdwmode(val);
    QByteArray data;
    data.resize(ipm.ByteSizeLong());
    ipm.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAmrState(bool val)
{
    amr.set_name(_amrstate);
    amr.set_state(val);
    QByteArray data;
    data.resize(amr.ByteSizeLong());
    amr.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendAudioCodecOption(AudioCodecStruct acodec)
{
    acc.set_name(_audiocodec);
    acc.set_state(acodec.IsAudioCodeEnable);
    acc.set_codectools(acodec.CodeTools);
    acc.set_codecpower(acodec.CodedPower);
    QByteArray data;
    data.resize(acc.ByteSizeLong());
    acc.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendIFLogParam(IFlogParamStruct iflog)
{
    ifr.set_name(_IFlog);
    ifr.set_state(iflog.state);
    ifr.set_savename(iflog.name.toUtf8().constData());
    ifr.set_timems(iflog.timems);
    QByteArray data;
    data.resize(ifr.ByteSizeLong());
    ifr.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendBandwidth(char bw)
{
    Sysparam.bandwidth = (BANDWIDTH)bw;
    SystemParam.set_bandwidth((BANDWIDTH)bw);
    SendSystemParams();
}
void CDFSocket::SendFilterBand(char filter)
{
    Sysparam.AnalogFilterBand = (SystemParameters_AnalogBand)filter;
    SystemParam.set_analogfilter((SystemParameters_AnalogBand)filter);
    SendSystemParams();
}
void CDFSocket::SendFilter(char fil)
{
    Sysparam.filter = (SystemParameters_FILTERS)fil;
    SystemParam.set_filter((SystemParameters_FILTERS)fil);
    SendSystemParams();
}

void CDFSocket::SendAntennaPolarization(SystemParameters_AntPOL val)
{
    Sysparam.AntPolarization = val;
    SystemParam.set_antennapolarization(val);
    SendSystemParams();
}

void CDFSocket::SendAntennaAmp(SystemParameters_ANTAMP val)
{
    Sysparam.AntAmp = val;
    SystemParam.set_antennaamp(val);
    SendSystemParams();
}

void CDFSocket::SendAntennaLength(SystemParameters_ANTENNALENGTH val)
{
    Sysparam.AntLength = val;
    SystemParam.set_antennalength(val);
    SendSystemParams();
}
void CDFSocket::SendElintMaxHold(bool val)
{
    SysparamElint.ElintMaxhold = val;
    SystemParamElint.set_elintmaxhold(val);
    SendSystemElintParams();
}
void CDFSocket::SendElintADCRef(short lowRef,short HighRef)
{
    SysparamElint.ADCLowRef = lowRef;
    SysparamElint.ADCHighRef = HighRef;
    SystemParamElint.set_adchighref(HighRef);
    SystemParamElint.set_adclowref(lowRef);
    SendSystemElintParams();
}
void CDFSocket::SendThresholdLow(int val)
{
    SysparamElint.ThresholdLow = val;
    SystemParamElint.set_thresholdlow(val);
    SendSystemElintParams();
}
void CDFSocket::SendThresholdHigh(int val)
{
    SysparamElint.ThresholdHigh = val;
    SystemParamElint.set_thresholdhigh(val);
    SendSystemElintParams();
}
void CDFSocket::SendThresholdLimit(int val)
{
    SysparamElint.ThresholdLimit = val;
    SystemParamElint.set_thresholdlimit(val);
    SendSystemElintParams();
}
void CDFSocket::SendBandwidthElint(char bw)
{
    SysparamElint.ElintBandwidth = (BANDWIDTH)bw;
    SystemParamElint.set_elintbandwidth((BANDWIDTH)bw);
    SendSystemElintParams();
}
void CDFSocket::SendGPSData(GPSParameters gpsp)
{
    gp.set_name(_GPSPARAMs);
    gp.set_latitude(gpsp.Latitude);
    gp.set_longitude(gpsp.Longitude);
    gp.set_autogps(gpsp.AutoGPSDetect);
    gp.set_autocompass(gpsp.AutoCompassDetect);
    gp.set_angleoffset(gpsp.AngleOffset);
    gp.set_isfpgauart(gpsp.IsFpgaUart);
    gp.set_northoffset(gpsp.NorthOffset);
    gp.set_baudrate(gpsp.Baudrate);
    gp.set_framework(gpsp.Framework);
    std::string stdString = gpsp.PortName.toStdString();
    gp.set_comport(stdString.c_str());
    QByteArray data;
    data.resize(gp.ByteSizeLong());
    gp.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendAutoDetectGPS(bool state)
{
    gp.set_name(_GPSPARAMs);
    gp.set_autogps(state);
    QByteArray data;
    data.resize(gp.ByteSizeLong());
    gp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAutoDetectAntenna(bool state)
{
    as.set_name(_AntState);
    QByteArray data;
    as.set_autodetect(state);
    data.resize(as.ByteSizeLong());
    as.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendDualPolAntenna(bool state)
{
    as.set_name(_AntState);
    QByteArray data;
    as.set_isdualpol(state);
    data.resize(as.ByteSizeLong());
    as.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAutoDetectCompass(bool state)
{
    gp.set_name(_GPSPARAMs);
    gp.set_autocompass(state);
    QByteArray data;
    data.resize(gp.ByteSizeLong());
    gp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendTransferDataRate(TransferDataStruct Tds)
{
    DataTransferS = Tds;
    td.set_name(_TransferData);
    td.set_transferaudio(Tds.TransferAudio);
    td.set_transferantennaresult(Tds.TransferAntennaResult);
    td.set_transferdatarange(Tds.TransferDatarange);
    td.set_transferdfparameters(Tds.TransferDFParameters);
    td.set_transferdoa(Tds.TransferDOA);
    td.set_transferpsd(Tds.TransferPSD);
    td.set_transfergpsparameters(Tds.TransferGPSParameters);
    td.set_transfercompassparameters(Tds.TransferCompassParameters);
    td.set_transferazimuthcalculation(Tds.TransferAzimuthCalculationParameters);
    td.set_transfererror(Tds.TransferError);
    td.set_transferfixwidebandpsddata(Tds.TransferFixWideBandPSDData);
    td.set_transferpdwdata(Tds.TransferPDWData);
    td.set_transferiqdata(Tds.TransferIQData);
    td.set_transferifdata(Tds.TransferIFData);
    td.set_transferamrdata(Tds.TransferAmrData);
    td.set_transferhistogramdoa(Tds.TransferHistogramDoa);

    QByteArray data;
    data.resize(td.ByteSizeLong());
    td.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAtennaState(QMap<char, bool> state)
{
    as.set_name(_AntState);
    as.set_add050(state.value((char)DFAntennaType::ADD050));
    as.set_add070(state.value((char)DFAntennaType::ADD070));
    as.set_add078(state.value((char)DFAntennaType::ADD078));
    as.set_add107(state.value((char)DFAntennaType::ADD107));
    as.set_add150(state.value((char)DFAntennaType::ADD150));
    as.set_add207(state.value((char)DFAntennaType::ADD207));
    as.set_add226(state.value((char)DFAntennaType::ADD226));
    as.set_add327(state.value((char)DFAntennaType::ADD327));
    as.set_add297(state.value((char)DFAntennaType::ADD297));
    QByteArray data;
    data.resize(as.ByteSizeLong());
    as.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAntennaOffset(QMap<char, double> offsets)
{
    af.set_name(_AntOffset);
    af.set_add050(offsets.value((char)DFAntennaType::ADD050));
    af.set_add070(offsets.value((char)DFAntennaType::ADD070));
    af.set_add078(offsets.value((char)DFAntennaType::ADD078));
    af.set_add107(offsets.value((char)DFAntennaType::ADD107));
    af.set_add150(offsets.value((char)DFAntennaType::ADD150));
    af.set_add207(offsets.value((char)DFAntennaType::ADD207));
    af.set_add226(offsets.value((char)DFAntennaType::ADD226));
    af.set_add327(offsets.value((char)DFAntennaType::ADD327));
    af.set_add297(offsets.value((char)DFAntennaType::ADD297));
    QByteArray data;
    data.resize(af.ByteSizeLong());
    af.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAzimuthHistogramMode(AzimuthHistogramStruct hist)
{
    ahs = hist;
    ah.set_name(_histogramMode);
    ah.set_hist(hist.Mode);
    ah.set_state(hist.State);
    ah.set_startindex(hist.StartIndex);
    ah.set_stopindex(hist.StopIndex);
    ah.set_mindoa(hist.MinDoa);
    ah.set_maxdoa(hist.MaxDoa);
    QByteArray data;
    data.resize(ah.ByteSizeLong());
    ah.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendAzimuthBlankings(const QList<Azimuthsblankingstruct> &list,bool state)
{
    AzimuthBlankings azimuths;
    azimuths.set_name(_AzimuthBlanking);
    AzimuthBlankingState = state;
    azimuths.set_state(state);
    for (const auto& p : list)
    {
        AzimuthBlanking* azimuth = azimuths.add_azimuthblankings();

        azimuth->set_startazimuth(p.StartAzimuth);
        azimuth->set_stopazimuth(p.StopAzimuth);
    }

    QByteArray data;

    data.resize(azimuths.ByteSizeLong());

    azimuths.SerializeToArray(data.data(),
                              data.size());
    SendData(data);
}

void CDFSocket::SendFrequencyOffset(const QList<AzimuthFrequencystruct> &list)
{
    FrequencyOffsets freqoffsets;
    freqoffsets.set_name(_FrequencyOffset);
    for (const auto& p : list)
    {
        FrequencyOffset* freqoff = freqoffsets.add_frequencyoffsets();

        freqoff->set_startfrequencymhz(p.StartFrequencyMHz);
        freqoff->set_stopfrequencymhz(p.StopFrequencyMHz);
        freqoff->set_offset(p.Offset);
    }

    QByteArray data;

    data.resize(freqoffsets.ByteSizeLong());

    freqoffsets.SerializeToArray(data.data(),
                                 data.size());
    SendData(data);
}

void CDFSocket::SendFrequencyOffsetMode(FrequencyOffsetMode_OFFSETMODES ofmode)
{
    fom.set_name(_FrequencyOffsetMode);
    fom.set_offsetmode(ofmode);
    QByteArray data;
    data.resize(fom.ByteSizeLong());
    fom.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendFrequency(double freqMHz)
{
    QByteArray ba;
    fr.set_name(_Frequency);
    fr.set_freq(freqMHz);
    ba.resize(fr.ByteSizeLong());
    fr.SerializeToArray(ba.data(),ba.size());
    SendData(ba);
}
void CDFSocket::SendThreshold(double thr)
{
    QByteArray ba;
    trh.set_name(_Threshold);
    trh.set_thr(thr);
    ba.resize(trh.ByteSizeLong());
    trh.SerializeToArray(ba.data(),ba.size());
    SendData(ba);
}

void CDFSocket::SendDefaults(SystemDefualtsStruct SystemDefaults)
{
    def.set_name(_SystemDefault);
    def.set_revert(SystemDefaults.Revert);
    QByteArray data;
    data.resize(def.ByteSizeLong());
    def.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendChannelNumber(short channel)
{
    ao.ChannelNumebr = channel;
    ap.set_name(_AntParam);
    ap.set_channelnumber(channel);
    QByteArray data;
    data.resize(ap.ByteSizeLong());
    ap.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendPageNumber(short PageNumber)
{
    ao.PageNumbers = PageNumber;
    ap.set_name(_AntParam);
    ap.set_pagenumber(PageNumber);
    QByteArray data;
    data.resize(ap.ByteSizeLong());
    ap.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendElintAntenna(bool isElint)
{
    ao.ElintMode = isElint;
    ap.set_name(_AntParam);
    ap.set_elint(isElint);
    QByteArray data;
    data.resize(ap.ByteSizeLong());
    ap.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendSwitchSequence(short seq)
{
    ao.SwitchSequnece = seq;
    ap.set_name(_AntParam);
    ap.set_switchsequence(seq);
    QByteArray data;
    data.resize(ap.ByteSizeLong());
    ap.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendAudioSate(bool state)
{
    audios.set_name(_AudioParam);
    audios.set_audio(state);
    QByteArray data;
    data.resize(audios.ByteSizeLong());
    audios.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendComputerParameters(ComputersParametersStruct ComputerParams)
{
    cp.set_name(_ComputerParam);
    cp.set_ipaddress(ComputerParams.IpAddress.toStdString());
    cp.set_lanname(ComputerParams.LanName.toStdString());
    cp.set_mtu(ComputerParams.mtu);
    cp.set_port(ComputerParams.port);
    QByteArray data;
    data.resize(cp.ByteSizeLong());
    cp.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendManualMode(ManualModeStruct manual)
{
    mmp.set_name(_ManualModeFreq);
    mmp.set_state(manual.state);
    mmp.set_startindex(manual.StartIndex);
    mmp.set_stopindex(manual.StopIndex);
    QByteArray data;
    data.resize(mmp.ByteSizeLong());
    mmp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendPositionFinding(PFmodeStruct Pfparam)
{
    pfm.set_name(_PositionFinding);
    pfm.set_state(Pfparam.state);
    pfm.set_alg(Pfparam.alg);
    QByteArray data;
    data.resize(pfm.ByteSizeLong());
    pfm.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendMemoryScanParam(MemoryScanParamStruct MemPar)
{
    msp.set_name(_MemoryScanParam);
    msp.set_frequency(MemPar.Freq);
    msp.set_level(MemPar.Threshold);
    QByteArray data;
    data.resize(msp.ByteSizeLong());
    msp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendMemoryScanState(MemoryScanStateStruct MemState)
{
    mss.set_name(_MemoryScanState);
    mss.set_state(MemState.State);
    mss.set_time(MemState.time);
    QByteArray data;
    data.resize(mss.ByteSizeLong());
    mss.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendSubRangeScanParam(SubRangeScanParamStruct SubPar)
{
    ssp.set_name(_SubRangeScanParam);
    ssp.set_startfrequencymhz(SubPar.StartFrequencyMHz);
    ssp.set_stopfrequencymhz(SubPar.StopFrequencyMHz);
    ssp.set_stepfrequecnykhz(SubPar.StepFrequecnyKHz);
    ssp.set_level(SubPar.level);
    QByteArray data;
    data.resize(ssp.ByteSizeLong());
    ssp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendSubRangeScanState(SubRangeScanStateStruct SubState)
{
    sss.set_name(_SubRangeScanState);
    sss.set_state(SubState.State);
    sss.set_time(SubState.time);
    QByteArray data;
    data.resize(sss.ByteSizeLong());
    sss.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendFullRangeParam(FullRangeScanParamStruct Fullpar)
{
    frsp.set_name(_FullRangeScan);
    frsp.set_state(Fullpar.State);
    frsp.set_startfrequencymhz(Fullpar.StartFrequencyMHz);
    frsp.set_stopfrequencymhz(Fullpar.StopFrequencyMHz);
    QByteArray data;
    data.resize(frsp.ByteSizeLong());
    frsp.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendRemoveScanlist(RemoveScansTables_MODE scant)
{
    rst.set_name(_RemoveScanList);
    rst.set_removedata(scant);
    QByteArray data;
    data.resize(rst.ByteSizeLong());
    rst.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendGetAllDFConfig()
{
    gadc.set_name(_GetAllSystem);
    QByteArray data;
    data.resize(gadc.ByteSizeLong());
    gadc.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendResetSystemConfig(ResetSystem_RESET resettype)
{
    rsd.set_name(_ResetSystemConfig);
    rsd.set_resetmode(resettype);
    QByteArray data;
    data.resize(rsd.ByteSizeLong());
    rsd.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendVersionRequest(string vs)
{
    Q_UNUSED(vs);
    fv.set_name(_FileVersions);
    QByteArray data;
    data.resize(fv.ByteSizeLong());
    fv.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendCalibraionParam(CalibrationStruct calib)
{
    cl.set_name(_Calibration);
    cl.set_coreon(calib.CoreOn);
    cl.set_gain(calib.gain);
    QByteArray data;
    data.resize(cl.ByteSizeLong());
    cl.SerializeToArray(data.data(),data.size());
    SendData(data);
}

void CDFSocket::SendMaxHoldParam(MaxholdParamsstruct maxhold)
{
    mxh.set_name(_Maxhold);
    mxh.set_state(maxhold.state);
    mxh.set_refreshtime(maxhold.RefreshTime);
    mxh.set_delaytime(maxhold.DelayTime);
    QByteArray data;
    data.resize(mxh.ByteSizeLong());
    mxh.SerializeToArray(data.data(),data.size());
    SendData(data);
}
void CDFSocket::SendDFlog(DFlogStruct logs)
{
    dfl.set_name(_DFLog);
    dfl.set_state(logs.state);
    dfl.set_params(logs.params);
    dfl.set_addressfile(logs.loglocation.toStdString().c_str());
    QByteArray data;
    data.resize(dfl.ByteSizeLong());
    dfl.SerializeToArray(data.data(),data.size());
    SendData(data);

}
void CDFSocket::SendDFMode(DFModeFindings_DFMODFND DFmode)
{
    dmf.set_name(_DFModeFinding);
    dmf.set_mode(DFmode);
    QByteArray data;
    data.resize(dmf.ByteSizeLong());
    dmf.SerializeToArray(data.data(),data.size());
    SendData(data);
}
QByteArray CDFSocket::framePayload(const QByteArray& payload){
    const quint16 sz = quint16(payload.size());
    QByteArray data;
    data.reserve(payload.size() + 7);
    data.append(char(0x23));
    data.append(char(0x35));
    data.append((char)PROTOID);
    data.append(char(sz & 0xFF));
    data.append(char((sz >> 8) & 0xFF));
    data.append(payload);
    data.append(char(0x55));
    data.append(char(0x55));
    return data;
}
