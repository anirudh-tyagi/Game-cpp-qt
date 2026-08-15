#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "core/Config.h"
#include "core/Controller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Skyward_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    //declared before the engine on purpose, locals die in reverse order so the
    //engine tears down its QML (and every binding onto "control" or "config")
    //first
    Config config;
    Controller control;

    QQmlApplicationEngine engine;

    //the two bridges between the simulation and the view layer: live game state,
    //and the entity sizes the view has to draw the same way collision sees them
    engine.rootContext()->setContextProperty("control", &control);
    engine.rootContext()->setContextProperty("config", &config);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Skyward", "Main");

    return app.exec();
}
