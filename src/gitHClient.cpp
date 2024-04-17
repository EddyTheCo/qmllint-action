#include <QNetworkRequest>
#include<QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "gitHClient.hpp"


Client::Client(const QString Grepo,const quint16 pNumber,const QString Gtoken,QObject * parent):QObject(parent),
    m_Grepo(Grepo),m_pNumber(pNumber),m_Gtoken(Gtoken),m_apiAddress("https://api.github.com"),
    nam(new QNetworkAccessManager(this))
{
    getDiff();
}
void Client::getDiff()
{
    auto api=m_apiAddress;
    api.setPath("/repos/"+m_Grepo+"/pulls/"+ QString::number(m_pNumber));
    auto request=QNetworkRequest(api);
    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Bearer ").append(m_Gtoken.toUtf8()));
    request.setRawHeader(QByteArray("Accept"),
                         QByteArray("application/vnd.github.diff"));
    request.setRawHeader(QByteArray("X-GitHub-Api-Version"),
                         QByteArray("2022-11-28"));
    const auto reply=nam->get(request);
    QObject::connect(reply, &QNetworkReply::finished,this, [=](){

        while(reply->bytesAvailable() )
        {
            auto line=reply->readLine();

            if (line.startsWith("+++ b/")&&line.size()>7)
            {
                line.chop(1);
                const auto filename=line.sliced(6);
                if(filename.size()>4)
                {
                    const auto extension=filename.last(3);
                    if(extension=="qml"&&reply->bytesAvailable())
                    {
                        auto diff=reply->readLine();
                        const auto psign=diff.lastIndexOf("+");
                        if(psign!=-1&&diff.size()>5)
                        {
                            diff.chop(4);
                            auto lNumbers=diff.sliced(psign+1).split(',');
                            if(lNumbers.size()==2)
                            {
                                const quint32 first=lNumbers.at(0).toUInt();
                                const quint32 second=lNumbers.at(1).toUInt();
                                if(second)
                                {
                                    m_changedFiles.insert(filename,std::make_pair(first,second));
                                }
                            }

                        }
                    }
                }

            }
        }

        emit ready();
        reply->deleteLater();
    });
    QObject::connect(reply, &QNetworkReply::errorOccurred,this,[=](QNetworkReply::NetworkError code)
                     {
                         auto errorreply=reply->errorString();
                         qDebug()<<"Error on diff:"<<errorreply;
                         qDebug()<<"code:"<<code;
                         qDebug()<<"errorfound:"<<reply->readAll();
                         reply->deleteLater();
                     });
}
void Client::sendReview(const QJsonObject& payload)
{

    auto api=m_apiAddress;
    api.setPath("/repos/"+m_Grepo+"/pulls/"+ QString::number(m_pNumber)+"/reviews");

    auto request=QNetworkRequest(api);

    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Bearer ").append(m_Gtoken.toUtf8()));
    request.setRawHeader(QByteArray("Accept"),
                         QByteArray("application/vnd.github+json"));
    request.setRawHeader(QByteArray("X-GitHub-Api-Version"),
                         QByteArray("2022-11-28"));

    const auto reply = nam->post(request,QJsonDocument(payload).toJson());


    QObject::connect(reply, &QNetworkReply::finished,this, [=](){

        QByteArray response_data = reply->readAll();
        auto data = (QJsonDocument::fromJson(response_data)).object();
        qDebug()<<"data from review:" <<data;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred,this,[=](QNetworkReply::NetworkError code)
                     {
                         auto errorreply=reply->errorString();
                         qDebug()<<"Error on review:"<<errorreply;
                         qDebug()<<"code:"<<code;
                         qDebug()<<"errorfound:"<<reply->readAll();
                         reply->deleteLater();
                     });

}
