#include <QCryptographicHash>
#include <QMutexLocker>

#include "runguard.h"

RunGuard::RunGuard(const QString& key)
    :   mSharedMem(generateKeyHash(key, "sharedmemKey")),
        mMemLock(generateKeyHash(key, "memLockKey"), 1)
{
    mMemLock.acquire();
    {
        QSharedMemory fix(mSharedMem.key());    // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    mMemLock.release();
}

RunGuard::~RunGuard()
{
    release();
}

bool RunGuard::isAnotherRunning()
{
    if(mSharedMem.isAttached()) {
        return false;
    }

    mMemLock.acquire();
    const bool isRunning = mSharedMem.attach();
    if(isRunning) {
        mSharedMem.detach();
    }
    mMemLock.release();

    return isRunning;
}

bool RunGuard::tryToRun() {
    if(isAnotherRunning()) {
        return false;
    }

    mMemLock.acquire();
    const bool result = mSharedMem.create(sizeof(quint64));
    mMemLock.release();

    if(!result) {
        release();
        return false;
    }

    return true;
}

void RunGuard::release() {
    mMemLock.acquire();
    if(mSharedMem.isAttached()) {
        mSharedMem.detach();
    }
    mMemLock.release();
}

QString RunGuard::generateKeyHash(const QString& key, const QString& salt) {
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}
