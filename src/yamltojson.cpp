#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "yaml-cpp/yaml.h"

#include <iostream>

std::vector<size_t> getLineWidths(QString filepath)
{
    std::vector<size_t> linewidths;
    QFile file(filepath);
    if (file.open(QFile::ReadOnly)) {
        size_t lineoffset = 0;
        while (file.bytesAvailable()) {
            const auto line = file.readLine();
            lineoffset += line.size();
            linewidths.push_back(lineoffset);
        }
    }
    return linewidths;
}
size_t offsetToLine(const size_t &offset, const std::vector<size_t> &linewidths)
{
    const auto d = std::distance(linewidths.begin(),
                                 std::lower_bound(linewidths.begin(), linewidths.end(), offset));
    return d + 1;
}
QJsonObject addLines(QHash<QString, QJsonArray> &files)
{
    QJsonArray farray;
    QHashIterator<QString, QJsonArray> i(files);
    while (i.hasNext()) {
        i.next();
        const auto linewidths = getLineWidths(i.key());
        QJsonArray var;
        for (auto &warning : i.value()) {
            auto wobject = warning.toObject();
            const auto charOffset = wobject["charOffset"].toInteger();
            const auto line = offsetToLine(charOffset, linewidths);
            wobject.insert("line", int(line));
            var.push_back(wobject);
        }
        farray.push_back(QJsonObject({{"filename", i.key()}, {"warnings", var}}));
    }

    return QJsonObject({{"files", farray}});
}
void yamlToJson(QString filepath)
{
    const auto fileInfo = QFileInfo(filepath);
    auto file = QFile(fileInfo.absoluteFilePath());
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        file.close();
        YAML::Node root = YAML::LoadFile(filepath.toStdString());
        const auto diagnostics = root["Diagnostics"];
        if (diagnostics && diagnostics.IsSequence()) {
            QHash<QString, QJsonArray> files;
            for (const auto &diag : diagnostics) {
                const auto diagMess = diag["DiagnosticMessage"];
                const auto filePath = QString::fromStdString(diagMess["FilePath"].as<std::string>());
                const auto message = QString::fromStdString(diagMess["Message"].as<std::string>());
                const auto fileOffset = diagMess["FileOffset"].as<size_t>();
                files[filePath].push_back(
                    QJsonObject({{"message", message}, {"charOffset", int(fileOffset)}}));
            }
            const auto json = QJsonDocument(addLines(files));
            QFile jsonFile(fileInfo.absoluteFilePath().replace(".cpp.o.yaml", "_clang-tidy.json"));
            jsonFile.open(QFile::WriteOnly);
            jsonFile.write(json.toJson());
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("yalmToJson");

    QCommandLineParser parser;
    parser.setApplicationDescription("Create json lint files from clang-tidy yaml files");
    parser.addHelpOption();

    parser.addPositionalArgument("rPath",
                                 QCoreApplication::translate("main",
                                                             "Absolute path of the yaml files"));

    parser.process(app);
    const QStringList args = parser.positionalArguments();

    if (args.size() != 1)
        parser.showHelp();

    auto filePath = QDir(args.at(0));
    const QStringList files = filePath.entryList(QStringList() << "*.yaml", QDir::Files);

    for (const auto &filePath : files) {
        const auto file = args.at(0) + "/" + filePath;
        yamlToJson(file);
    }

    return 0;
}
