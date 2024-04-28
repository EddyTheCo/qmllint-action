#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <stdint.h>

int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;

	engine.addImportPath("qrc:/amarulasolutions.com/imports");

	QUrl url = QUrl("qrc:/amarulasolutions.com/imports/Eexample1/qml/example1.qml");

	engine.load(url);

	return app.exec();
}
