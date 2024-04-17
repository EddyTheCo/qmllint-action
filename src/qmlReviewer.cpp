#include "qmlReviewer.hpp"
#include<QFileInfo>
#include<QDir>
#include<QJsonDocument>


QString moduleReviewer::rootPath="";

void moduleReviewer::setRootPath(const QString rp)
{
    rootPath=rp;
}

moduleReviewer::moduleReviewer(const QString jsonFilepath)
{
    const auto fileInfo=QFileInfo(jsonFilepath);
    auto jsonFile=QFile(fileInfo.absoluteFilePath());
    if(jsonFile.exists()&&jsonFile.open(QIODevice::ReadOnly))
    {
        m_module=fileInfo.baseName().replace("_qmllint","");
        QByteArray json=jsonFile.readAll();
        jsonFile.close();

        const auto jsonDocument=QJsonDocument::fromJson(json);
        if(jsonDocument.isObject())
        {
            const auto data=jsonDocument.object();
            const auto files=data["files"].toArray();
            for(const auto& fileobj:files)
            {
                const auto file=fileobj.toObject();
                auto filename=file["filename"].toString();

                QDir rootDir(rootPath);
                QString filepath=rootDir.relativeFilePath(filename);

                const auto warnings=file["warnings"].toArray();
                for (const auto & v: warnings)
                {
                    const auto warning=v.toObject();
                    const auto comment=warningToComment(warning,filepath);
                    m_comments.push_back(comment);
                }
            }
        }
    }
}

QJsonObject moduleReviewer::warningToComment(const QJsonObject &warning, const QString filePath)const
{
    QJsonObject comment;

    comment.insert("path",filePath);
    comment.insert("body",warning["message"].toString());

    if(!warning["line"].isNull())
    {
        comment.insert("side","RIGHT");
        comment.insert("line",warning["line"].toInt());
    }
    return comment;
}
QJsonObject moduleReviewer::getReview(const QHash<QString, std::pair<quint32, quint32> > &changedFiles)const
{
    QJsonObject review;
    QJsonArray commentsInChange;

    QString errorString="";
    for(const auto & v: std::as_const(m_comments))
    {
        const auto comment=v.toObject();
        const auto path=comment["path"].toString();

        if(changedFiles.contains(path))
        {
            const auto pair = changedFiles.value(path);
            const quint32 cline=comment["line"].toInteger();

            if(cline>=pair.first&&cline<=pair.second)
            {
                commentsInChange.push_back(comment);
            }
        }

        QString line="";
        if(!comment["line"].isNull())
        {
            line=QString::number(comment["line"].toInteger());
        }
        errorString+="- "+ comment["path"].toString() + (line!=""?(":"+line):"") + " (" +  comment["body"].toString()+")\n";

    }
    if(commentsInChange.size()|| errorString!="")
    {
        review.insert("body","**qmllint report for " + m_module + " module**\n"+errorString);
        review.insert("event","COMMENT");
        review.insert("comments",commentsInChange);
    }

    return review;
}
