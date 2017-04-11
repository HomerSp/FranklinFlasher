#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <QEventLoop>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QFile>

class FileUtils
{
public:
    struct UPZHeader {
        char magic[3];
        uint8_t version;
        QString device;
    };

    static bool createUPZ(const QString& inputPath, const QString& output, const QString& device);
    static bool extractUPZ(QFile& inputFile, const QString& outputPath);

    static bool getUPZHeader(QFile& inputFile, UPZHeader& upzHeader);

    static void listFiles(QList<QString> &files, const QString& source, const QString& sub = QString());
    static void listDirs(QList<QString> &dirs, const QString& source, const QString& sub = QString());

private:
    FileUtils();
};

#endif // COMMON_UTILS_H
