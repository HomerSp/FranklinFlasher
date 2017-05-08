#include <QFile>
#include <QProcess>
#include <QTemporaryDir>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QRegularExpression>

#include "fileutils.h"
#include "flashworker.h"
#include "webutils.h"

FlashWorker::FlashWorker(const QString& device, QObject* parent)
    : QObject(parent),
      mDevice(device),
      mStopped(0)
{

}

FlashWorker::~FlashWorker() {
    mStopped = 1;

    // Wait for the processing to stop
    qInfo()<<"Waiting for process to stop";
    QMutexLocker locker(&mRunningMutex);
    qInfo()<<"Process stopped";

    emit finished();
}

void FlashWorker::process() {
    QMutexLocker locker(&mRunningMutex);

    QString dir = mDir.path();

    emit setStatus("Please connect to the device's wifi now.");
    waitForDevice();
    if(mStopped == 1) {
        //qDebug()<<"Done waiting";
        emit doneFlashing();
        return;
    }

    if(!checkIMEI()) {
        emit setStatus("Could not flash device.");
        emit doneFlashing();
        return;
    }

    emit setStatus("Flashing the device.");
    if(mStopped == 1) {
        emit doneFlashing();
        return;
    }

    beginFlashing();
}

void FlashWorker::processInit() {
    QMutexLocker locker(&mRunningMutex);

    if(canFlash()) {
        QFile upzFile("data/data.upz");
        if(upzFile.exists()) {
            upzFile.open(QIODevice::ReadOnly);

            if(mDir.isValid()) {
                QString dir = mDir.path();

                if(FileUtils::extractUPZ(upzFile, dir)) {
                    upzFile.close();

                    QFile imeiFile(dir + "/imei.list");
                    if(imeiFile.open(QIODevice::ReadOnly)) {
                        QTextStream stream(&imeiFile);
                        while(!stream.atEnd()) {
                            QString line = stream.readLine();
                            line.replace(QRegularExpression("[^0-9A-Fa-f]"), "");
                            qDebug()<<"Addinng imei"<<line<<"to whilelist";
                            mIMEIWhitelist.append(line);
                        }
                    }

                    emit setStatus("Ready to flash");
                    emit initDone();
                    return;
                }
            }

            upzFile.close();
        }
    } else {
        emit setStatus("Could not communicate with the server, please contact Inficell.");
        return;
    }

    emit setStatus("Could not initialize the program, please try again.");
}

bool FlashWorker::canFlash() {
#ifdef NO_INTERNET
    return true;
#endif

    QJsonObject obj;
    obj.insert("t", "checkin");
    obj.insert("device", mDevice);

    QString jsonData = QString(QJsonDocument(obj).toJson());

    QByteArray output;
    if(!Web::WebUtils::download(QUrl("http://inficell.net/endpoint/flasher/franklin.php"), output, jsonData)) {
        return false;
    }

    QJsonObject outputObj = QJsonDocument::fromJson(output).object();
    return outputObj.contains("enable") && outputObj.value("enable").toBool() == true;
}

void FlashWorker::waitForDevice() {
    bool found = false;
    do {
        qInfo()<<"waitForDevice";
        QByteArray output;
        if(Web::WebUtils::download(QUrl("http://my.jetpack/webpst/labpst"), output)) {
            found = true;
        }

        if(found || mStopped == 1) {
            break;
        }

        QThread::sleep(1);
    } while(!found);
}

bool FlashWorker::checkIMEI() {
    if(mIMEIWhitelist.empty()) {
        return true;
    }

    QJsonObject obj;
    obj.insert("Command", "GetDiagnostics");

    QString jsonData = QString(QJsonDocument(obj).toJson());

    QByteArray output;
    if(!Web::WebUtils::download(QUrl("http://my.jetpack/cgi-bin/about_diagnostics.cgi"), output, jsonData)) {
        return false;
    }

    QJsonObject outputObj = QJsonDocument::fromJson(output).object();
    if(outputObj.contains("IMEI")) {
        QString imei = outputObj.value("IMEI").toString();
        qDebug()<<"Got imei"<<imei<<"from device, checking if it can be flashed.";
        return mIMEIWhitelist.contains(imei);
    }

    return false;
}

void FlashWorker::beginFlashing() {
    qDebug()<<"beginFlashing";

    QString dir = mDir.path();
    if(!QFile(dir + "/firmware.sfp").exists()) {
        emit setStatus("Could not flash the device, please contact Inficell.");
        emit doneFlashing();
        return;
    }

    QByteArray preOutput;
    if(Web::WebUtils::download(QUrl("http://my.jetpack/cgi-bin/webpst.cgi?Command=PreFileUpload"), preOutput)) {
        qInfo()<<"Pre output:";
        qInfo()<<QString(preOutput);

        QFile firmwareFile(dir + "/firmware.sfp");
        firmwareFile.open(QFile::ReadOnly);

        QByteArray uploadOutput;
        if(Web::WebUtils::upload(QUrl("http://my.jetpack/cgi-bin/webpst.cgi"), uploadOutput, firmwareFile.readAll(), "filename1", "firmware.sfp")) {
            qInfo()<<"Upload output:";
            qInfo()<<QString(uploadOutput);

            QByteArray flashOutput;
            if(Web::WebUtils::download(QUrl("http://my.jetpack/cgi-bin/webpst.cgi?Command=FirmwareUpdateStart&EraseConfig=1"), flashOutput)) {
                qInfo()<<"Flash output:";
                qInfo()<<QString(flashOutput);

                emit setStatus("Waiting for device to reset");

                if(waitForDeviceReset()) {
                    emit setStatus("Done flashing");
                    emit doneFlashing();
                    return;
                }
            }
        }
    }

    emit setStatus("Could not flash the device, please contact Inficell.");
    emit doneFlashing();
}

bool FlashWorker::waitForDeviceReset() {
    int counter = 0;
    bool notFound = false;
    do {
        qInfo()<<"waitForDeviceReset";
        QByteArray output;
        if(!Web::WebUtils::download(QUrl("http://my.jetpack/webpst/labpst"), output)) {
            notFound = true;
        }

        counter++;
        if(notFound || counter >= 10 || mStopped == 1) {
            break;
        }

        QThread::sleep(1);
    } while(!notFound);

    if(!notFound && counter >= 10) {
        return false;
    }

    return true;
}
