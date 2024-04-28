#include "reviewer.hpp"
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>

QString ModuleReviewer::rootPath = "";

void ModuleReviewer::setRootPath(const QString rp)
{
    rootPath = rp;
}

ModuleReviewer::ModuleReviewer(const QString filepath, QString modType)
{
    const auto fileInfo = QFileInfo(filepath);
    auto file = QFile(fileInfo.absoluteFilePath());
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        m_module = fileInfo.baseName().replace(("+" + modType), "");
        QByteArray json = file.readAll();
        file.close();

        const auto jsonDocument = QJsonDocument::fromJson(json);
        if (jsonDocument.isObject()) {
            const auto data = jsonDocument.object();
            const auto files = data["files"].toArray();
            for (const auto &fileobj : files) {
                const auto file = fileobj.toObject();
                auto filename = file["filename"].toString();

                QDir rootDir(rootPath);
                QString filepath = rootDir.relativeFilePath(filename);

                const auto warnings = file["warnings"].toArray();
                for (const auto &v : warnings) {
                    const auto warning = v.toObject();
                    const auto comment = warningToComment(warning, filepath);
                    m_comments.push_back(comment);
                }
            }
        }
    }
}
void ModuleReviewer::checkQmllint(const QString filepath) {}
QJsonObject ModuleReviewer::warningToComment(const QJsonObject &warning,
                                             const QString filePath) const
{
    QJsonObject comment;

    comment.insert("path", filePath);
    comment.insert("body", warning["message"].toString());

    if (!warning["line"].isNull()) {
        comment.insert("side", "RIGHT");
        comment.insert("line", warning["line"].toInt());
    }
    return comment;
}
QJsonObject ModuleReviewer::getReview(
    const QHash<QString, std::pair<quint32, quint32> > &changedFiles) const
{
    QJsonObject review;
    QJsonArray commentsInChange;

    QString errorString = "<details><summary> :warning: " + QString::number(m_comments.size())
                          + " warnings </summary>";
    for (const auto &v : std::as_const(m_comments)) {
        const auto comment = v.toObject();
        const auto path = comment["path"].toString();

        if (changedFiles.contains(path)) {
            const auto pair = changedFiles.value(path);
            const quint32 cline = comment["line"].toInteger();

            if (cline >= pair.first && cline <= pair.first + pair.second) {
                commentsInChange.push_back(comment);
            }
        }

        QString line = "";
        if (!comment["line"].isNull()) {
            line = QString::number(comment["line"].toInteger());
        }
        errorString += "- " + comment["path"].toString() + (line != "" ? (":" + line) : "") + " ("
                       + comment["body"].toString() + ")\n";
    }
    errorString += "</details>";
    if (m_comments.size()) {
        review.insert("body", "**Lint report for " + m_module + " module**\n" + errorString);
        review.insert("event", "COMMENT");
        review.insert("comments", commentsInChange);
    }

    return review;
}
