#include<QJsonValue>
#include<QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include<QFile>
#include <QNetworkRequest>
#include<QNetworkAccessManager>
#include<QNetworkReply>

#include<QCoreApplication>
#include<QCommandLineParser>



QJsonObject reviewModule(const QString jsonFilepath,const QString rootPath="")
{
    QJsonObject root;
    const auto fileInfo=QFileInfo(jsonFilepath);
    auto jsonFile=QFile(fileInfo.absoluteFilePath());
    if(jsonFile.exists()&&jsonFile.open(QIODevice::ReadOnly))
    {
        const auto module=fileInfo.baseName().replace("_qmllint","");
        QByteArray json=jsonFile.readAll();
        jsonFile.close();

        const auto jsonDocument=QJsonDocument::fromJson(json);
        if(jsonDocument.isObject())
        {

            root.insert("body","qmllint report for " + module + " module");
            root.insert("event","COMMENT");
            const auto data=jsonDocument.object();
            const auto files=data["files"].toArray();
            QJsonArray comments;
            for(const auto& fileobj:files)
            {
                const auto file=fileobj.toObject();
                auto filename=file["filename"].toString();

                QDir rootDir(rootPath);

                QString m_filepath=rootDir.relativeFilePath(filename);

                const auto warnings=file["warnings"].toArray();
                for (const auto & v: warnings)
                {
                    QJsonObject var;
                    const auto vobject=v.toObject();
                    var.insert("path",m_filepath);
                    var.insert("body",vobject["message"].toString());

                    if(!vobject["line"].isNull())
                        var.insert("position",vobject["line"].toInt());

                    comments.push_back(var);
                }
            }
            root.insert("comments",comments);
        }

    }

    return root;

}
QNetworkReply* sendReview(const QString apiPath,const QString githubToken,const QJsonObject& payload,
                          QNetworkAccessManager * nam)
{

    auto api=QUrl("https://api.github.com");

    
    api.setPath(apiPath);
    qDebug()<<"api:"<<api;
    auto request=QNetworkRequest(api);

    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Bearer ").append(githubToken.toUtf8()));
    request.setRawHeader(QByteArray("Accept"),
                         QByteArray("application/vnd.github+json"));
    request.setRawHeader(QByteArray("X-GitHub-Api-Version"),
                         QByteArray("2022-11-28"));

    return nam->post(request,QJsonDocument(payload).toJson());

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("qmlLGC");

    QCommandLineParser parser;
    parser.setApplicationDescription("Create reviews for a PR using json qmllint files");
    parser.addHelpOption();
    parser.addPositionalArgument("jsonPath", QCoreApplication::translate("main", "Absolute path to the qmllint json files"));
    parser.addPositionalArgument("repoPath", QCoreApplication::translate("main", "Absolute path of the local github repo"));
    parser.addPositionalArgument("apiPath", QCoreApplication::translate("main", "Github API path"));
    parser.addPositionalArgument("token", QCoreApplication::translate("main", "Github token"));

    parser.process(app);
    const QStringList args = parser.positionalArguments();

    if(args.size()!=4)
        parser.showHelp();

    auto nam = new QNetworkAccessManager(&app);

    auto jsonPath=QDir(args.at(0));
    qDebug()<<"jsonPath:"<<jsonPath;
    const QStringList files = jsonPath.entryList(QStringList() << "*_qmllint.json" , QDir::Files);


    qDebug()<<"files:"<<files;
    for(const auto & jsonFilePath:files)
    {
        const auto payload=reviewModule(args.at(0)+"/"+jsonFilePath,args.at(1));
        qDebug()<<"payload:"<<payload;
        qDebug()<<"apiPath:"<<args.at(2);
        const auto reply=sendReview(args.at(2),args.at(3),payload,nam);

        QObject::connect(reply, &QNetworkReply::finished,&app, [=](){

            QByteArray response_data = reply->readAll();
            auto data = (QJsonDocument::fromJson(response_data)).object();
            qDebug()<<"replydata:"<<data;

        });

        QObject::connect(reply, &QNetworkReply::errorOccurred,&app,[=](QNetworkReply::NetworkError code)
                         {
                             auto errorreply=reply->errorString();
                             qDebug()<<"Error:"<<errorreply;
                             qDebug()<<"code:"<<code;
                             qDebug()<<"errorfound"<<reply->readAll();
                         });
    }


    return app.exec();
}
