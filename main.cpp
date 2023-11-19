#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>

#include "transcription.h"



int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("untitled7");


    QGuiApplication app(argc, argv);

    qmlRegisterType<Transcription>("Transcription", 1, 0, "Transcription");


    QQmlApplicationEngine engine;
    engine.load("file:////home/ruan/Desktop/untitled7/Main.qml");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
