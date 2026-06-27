#include "receivedata.h"

ReceiveData::ReceiveData(QTcpSocket *TcpSocket,QObject *parent)
    : QObject{parent},TcpSocket(TcpSocket)
{

}

void ReceiveData::started()
{
    forever
    {
        while (TcpSocket->bytesAvailable() > 0) {
            //if(ResumeFlag)
             //   data.append(TcpSocket->read(TcpSocket->bytesAvailable()));
            //else
                data= TcpSocket->read(TcpSocket->bytesAvailable());
        }
        if(!data.isEmpty())
        {

            for(int i = 0;i < data.size();i++)
            {
                if(i+3 >= data.size())
                    break;
                if(data.at(i) == 0x23 && data.at(i+1) == 0x35)
                {
                    int index = i + 2;
                    ushort len = 0;
                    ushort hlen = 0;
                    len = (data.at(i+2) & 0xff); index++;
                    hlen = (data.at(i+3) << 8); index++;
                    len |= hlen;
                    if(len + index + 1 >= data.size())
                    {
                       qDebug() << len + index << "  error2  " << data.size();
                        data = data.mid(index - 2,data.size());
                        ResumeFlag = true;
                        break;
                    }
                    if(data.at(len + index) == FOOTER1 && data.at(len + index + 1) == FOOTER2)
                    {
                       // qDebug() << "ok";
                        ResumeFlag = false;
                       // DoaData(data.mid(index,len - FOOTERSIZE - HEADERSIZE ));
                       // PSDdata(data.mid(index,len - 4 ));

                    }
                    i = len + index;
                    //ResumeFlag = false;

                }
            }
        }

    }
}
