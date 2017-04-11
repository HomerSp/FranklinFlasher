#ifndef FLASHWORKER_H
#define FLASHWORKER_H

#include <QObject>
#include <QProcess>
#include <QTemporaryDir>
#include <QVariant>
#include <QAtomicInteger>
#include <QMutex>

class FlashWorker : public QObject
{
    Q_OBJECT
public:
    FlashWorker(const QString& device, QObject* parent = 0);
    ~FlashWorker();

public slots:
    void process();

    void processInit();

signals:
    void initDone();
    void initError();

    void doneFlashing();
    void finished();

    void setStatus(QVariant status);

private:
    bool canFlash();
    void waitForDevice();
    void beginFlashing();
    bool waitForDeviceReset();

    QString mDevice;

    QTemporaryDir mDir;

    QMutex mRunningMutex;
    QAtomicInteger<int> mStopped;

};

#endif // FLASHWORKER_H
