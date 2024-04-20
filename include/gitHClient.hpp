#pragma once


#include<QNetworkAccessManager>

class Client: public QObject
{

    Q_OBJECT
public:
    Client(const QString Grepo,const quint16 pNumber,const QString Gtoken,QObject *parent = nullptr);
    auto getChangedFiles()const{return m_changedFiles;}
    void sendReview(const QJsonObject& payload);
signals:
    void ready();
private:
    void getDiff();

    QHash<QString, std::pair<quint32,quint32>> m_changedFiles;
    QUrl m_apiAddress;
    QNetworkAccessManager* nam;
    QString m_Grepo;
    quint16 m_pNumber;
    QString m_Gtoken;
};
