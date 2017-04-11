#include <QCoreapplication>
#include <QDir>
#include <QFileInfoList>
#include <QHostInfo>

#include "fileutils.h"

FileUtils::FileUtils() {

}

bool FileUtils::createUPZ(const QString& inputPath, const QString &outputName, const QString& device) {
    QFile outputFile(outputName);
    if(!outputFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QList<QString> files;
    listFiles(files, inputPath);

    QByteArray header;
    header.append('U');
    header.append('P');
    header.append('Z');
    header.append('\x00');

    header.append(device.toUtf8());
    header.append('\x00');

    outputFile.write(header);

    foreach(QString fileName, files) {
        QFileInfo file(inputPath + "/" + fileName);
        QFile inFile(file.absoluteFilePath());
        if(!inFile.open(QIODevice::ReadOnly)) {
            continue;
        }

        QByteArray fileHeader;
        fileHeader.append(fileName.toUtf8());
        fileHeader.append('\0');

        outputFile.write(fileHeader);

        QByteArray buffer;
        do {
            QByteArray chunkData;

            buffer = inFile.read(50 * 1024 * 1024);
            if(buffer.size() == 0) {
                break;
            }

            QByteArray compressedBuffer = ::qCompress(buffer, 1);

            uint32_t chunkSize = compressedBuffer.size();
            chunkData.append(static_cast<uint8_t>((chunkSize) & 0xFF));
            chunkData.append(static_cast<uint8_t>((chunkSize >> 8) & 0xFF));
            chunkData.append(static_cast<uint8_t>((chunkSize >> 16) & 0xFF));
            chunkData.append(static_cast<uint8_t>((chunkSize >> 24) & 0xFF));
            chunkData.append(compressedBuffer);

            outputFile.write(chunkData);
        } while(buffer.size() > 0);

        QByteArray endData;
        endData.append('\0');
        endData.append('\0');
        endData.append('\0');
        endData.append('\0');
        outputFile.write(endData);

        inFile.close();
    }

    outputFile.flush();
    outputFile.close();

    return true;
}

bool FileUtils::extractUPZ(QFile& inputFile, const QString& outputPath) {
    UPZHeader upzHeader;
    if(!getUPZHeader(inputFile, upzHeader)) {
        return false;
    }

    while(inputFile.bytesAvailable() > 0) {
        QByteArray fileNameData;
        QByteArray c;
        do {
            c = inputFile.read(1);
            fileNameData += c;
        } while(c.size() == 1 && c.at(0) != '\0');

        QString fileName = QString::fromUtf8(fileNameData);

        //qDebug()<<"Extracting"<<outputPath<<fileName<<fileNameData.size()<<c.size();

        QDir().mkpath(QFileInfo(outputPath + "/" + fileName).dir().absolutePath());

        QFile outputFile(outputPath + "/" + fileName);
        if(!outputFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            return false;
        }

        //qDebug()<<"Reading chunks";

        uint32_t chunkSize = 0;
        do {
            QByteArray chunkHeader = inputFile.read(4);
            chunkSize = static_cast<uint32_t>(chunkHeader.at(0) & 0xFF)
                | static_cast<uint32_t>(chunkHeader.at(1) & 0xFF) << 8
                | static_cast<uint32_t>(chunkHeader.at(2) & 0xFF) << 16
                | static_cast<uint32_t>(chunkHeader.at(3) & 0xFF) << 24;

            //qDebug()<<"Chunk"<<chunkSize;

            if(chunkSize == 0) {
                break;
            }

            QByteArray chunkData = inputFile.read(chunkSize);
            outputFile.write(qUncompress(chunkData));
        } while(chunkSize != 0);

        outputFile.flush();
        outputFile.close();
    }

    return true;
}


bool FileUtils::getUPZHeader(QFile& inputFile, UPZHeader& upzHeader) {
    QByteArray header = inputFile.read(4);
    if(header.size() < 4) {
        return false;
    }

    memcpy(upzHeader.magic, header.data(), 3);
    upzHeader.version = header.at(3);

    QByteArray deviceData;
    while(inputFile.bytesAvailable() > 0) {
        QByteArray c = inputFile.read(1);
        if(c.size() == 0 || c.at(0) == '\0') {
            break;
        }

        deviceData += c;
    }

    upzHeader.device = QString::fromUtf8(deviceData);

    return true;
}

void FileUtils::listFiles(QList<QString> &files, const QString& source, const QString& sub) {
    QFileInfoList list = QDir(source + "/" + sub).entryInfoList();
    foreach(QFileInfo info, list) {
        QString fileName = info.fileName();
        if(fileName == ".." || fileName == ".") {
            continue;
        }

        if(info.isDir()) {
            if(sub.size() > 0) {
                listFiles(files, source, sub + "/" + fileName);
            } else {
                listFiles(files, source, fileName);
            }

            continue;
        }

        if(sub.size() > 0) {
            files.append(sub + "/" + fileName);
        } else {
            files.append(fileName);
        }
    }
}

void FileUtils::listDirs(QList<QString> &dirs, const QString& source, const QString& sub) {
    QFileInfoList list = QDir(source + "/" + sub).entryInfoList();
    foreach(QFileInfo info, list) {
        QString fileName = info.fileName();
        if(fileName == ".." || fileName == ".") {
            continue;
        }

        if(info.isDir()) {
            if(sub.size() > 0) {
                listDirs(dirs, source, sub + "/" + fileName);
                dirs.append(sub + "/" + fileName);
            } else {
                listDirs(dirs, source, fileName);
                dirs.append(fileName);
            }

            continue;
        }
    }
}
