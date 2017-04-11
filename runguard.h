#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QSharedMemory>
#include <QString>
#include <QSystemSemaphore>

class RunGuard
{
public:
    RunGuard(const QString& key);
    ~RunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    QString generateKeyHash(const QString& key, const QString& salt);

    QSharedMemory mSharedMem;
    QSystemSemaphore mMemLock;

    Q_DISABLE_COPY(RunGuard)
};

#endif // RUNGUARD_H
