#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

#include "fileutils.h"
#include "runguard.h"
#include "flashworker.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qml);

    QGuiApplication app(argc, argv);

    RunGuard runGuard("FranklinFlasher_758d66e16ca27e3468e20cd675e8a27dc97a4db8");
    if(!runGuard.tryToRun()) {
        return 0;
    }

    QGuiApplication::setApplicationName("FranklinFlasher");
    QGuiApplication::setOrganizationDomain("inficell.net");
    QGuiApplication::setOrganizationName("Inficell");

    qsrand(QDateTime::currentDateTime().toTime_t());

    int ret = 0;

    QStringList args = QCoreApplication::arguments();
    if(args.size() == 5 && args.at(1) == "create") {
        FileUtils::createUPZ(args.at(2), args.at(3), args.at(4));
    } else {
        QFile upzFile("data/data.upz");
        if(!upzFile.exists()) {
            return -1;
        }

        upzFile.open(QIODevice::ReadOnly);

        FileUtils::UPZHeader upzHeader;
        FileUtils::getUPZHeader(upzFile, upzHeader);

        upzFile.close();

        qInfo()<<"Device:"<<upzHeader.device;

        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("flashDevice", upzHeader.device);
        engine.load(QUrl(QStringLiteral("qrc:/res/qml/main.qml")));

        QThread* thread = new QThread;

        FlashWorker* worker = new FlashWorker(upzHeader.device);
        worker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, worker, &FlashWorker::processInit);
        QObject::connect(worker, &FlashWorker::finished, thread, &QThread::quit);

        QObject* flashButton = engine.rootObjects().first()->findChild<QObject*>("flashButton");
        QObject::connect(worker, SIGNAL(initDone()), flashButton, SLOT(enable()));
        QObject::connect(flashButton, SIGNAL(clicked()), worker, SLOT(process()));
        QObject::connect(worker, SIGNAL(doneFlashing()), flashButton, SLOT(enable()));

        QObject* statusText = engine.rootObjects().first()->findChild<QObject*>("statusText");
        QObject::connect(worker, SIGNAL(setStatus(QVariant)), statusText, SLOT(setStatus(QVariant)));

        thread->start();

        ret = app.exec();

        qInfo()<<"Exiting app";

        if(thread->isRunning()) {
            thread->quit();
        }

        delete worker;
        delete thread;
    }

    return ret;
}
