#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>

#include "gitHClient.hpp"
#include "reviewer.hpp"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("lintBot");

  QCommandLineParser parser;
  parser.setApplicationDescription(
      "Create reviews for a PR using json lint files");
  parser.addHelpOption();

  parser.addPositionalArgument(
      "jPath", QCoreApplication::translate(
                   "main", "Absolute path to the lint json files"));
  parser.addPositionalArgument(
      "rPath", QCoreApplication::translate(
                   "main", "Absolute path of the local github repo"));
  parser.addPositionalArgument(
      "GRepo", QCoreApplication::translate("main", "OWNER/REPO"));
  parser.addPositionalArgument(
      "PRNumber", QCoreApplication::translate("main", "Pull Request Number"));
  parser.addPositionalArgument(
      "GToken", QCoreApplication::translate("main", "Github token"));

  parser.process(app);
  const QStringList args = parser.positionalArguments();

  if (args.size() != 5)
    parser.showHelp();

  QTimer::singleShot(10000, &app, QCoreApplication::quit);

  ModuleReviewer::setRootPath(args.at(1));

  auto gitHclient =
      new Client(args.at(2), args.at(3).toUInt(), args.at(4), &app);

  QObject::connect(gitHclient, &Client::ready, &app, [=, &app]() {
    auto jsonPath = QDir(args.at(0));
    const QStringList files =
        jsonPath.entryList(QStringList() << ("*.json"), QDir::Files);
    const auto changedFiles = gitHclient->getChangedFiles();
    for (const auto &jsonFilePath : files) {
      const auto reviewer = ModuleReviewer(args.at(0) + "/" + jsonFilePath);
      const auto review = reviewer.getReview(changedFiles);
      if (!review.isEmpty()) {
        gitHclient->sendReview(review);
      }
    }
  });

  return app.exec();
}
