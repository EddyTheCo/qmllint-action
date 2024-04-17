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

    for(const auto & v: std::as_const(m_comments))
    {
        const auto comment=v.toObject();
        const auto path=comment["path"].toString();
            qDebug()<<"path:"<<path;
        if(changedFiles.contains(path))
        {
            qDebug()<<"changed files contains:"<<path;
            const auto pair = changedFiles.value(path);
            const auto cline=comment["line"].toInteger();
            qDebug()<<"cline:"<<cline<<" "<<pair.first<<" "<<pair.second;
            if(cline>=pair.first&&cline<=pair.first+pair.second)
            {
                commentsInChange.push_back(comment);
            }
        }

    }
    if(commentsInChange.size())
    {
        review.insert("body","qmllint report for " + m_module + " module");
        review.insert("event","COMMENT");
        review.insert("comments",m_comments);
    }
    qDebug()<<"review:"<<review;
    return review;
}