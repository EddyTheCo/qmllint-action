#pragma once

#include <QJsonArray>
#include <QJsonObject>

class ModuleReviewer
{
public:
    ModuleReviewer(const QString filepath, QString modType = "qmllint");
    static void setRootPath(const QString rp);
    QString module() const { return m_module; }
    QJsonObject getReview(const QHash<QString, std::pair<quint32, quint32>> &changedFiles) const;

private:
    void checkQmllint(QString filepath);
    QJsonObject warningToComment(const QJsonObject &warning, const QString filePath) const;
    static QString rootPath;
    QString m_module;
    QJsonArray m_comments;
};
