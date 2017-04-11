#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPair>

#include "webutils.h"

using namespace Web;

WebUtils::WebUtils()
{

}

bool WebUtils::download(const QUrl& url, QByteArray& output) {
    QHash<QString, QString> headers;
    return download(url, output, headers);
}

bool WebUtils::download(const QUrl& url, QByteArray& output, QString postData, WebDownloadStatus* status) {
    QHash<QString, QString> headers;
    return download(url, output, headers, postData, status);
}

bool WebUtils::download(const QUrl& url, QByteArray& output, const QHash<QString, QString> &headers, QString postData, WebDownloadStatus* status) {
    QNetworkRequest req(url);
    for(QHash<QString, QString>::const_iterator i = headers.begin(); i != headers.end(); ++i) {
        req.setRawHeader(i.key().toLatin1(), i.value().toLatin1());
    }

    QNetworkAccessManager manager;

    QNetworkReply* reply = nullptr;
    if(postData.size() > 0) {
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        reply = manager.post(req, postData.toLatin1());
    } else {
        reply = manager.get(req);
    }

    if(reply == nullptr) {
        return false;
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    if(status != nullptr) {
        QObject::connect(reply, &QNetworkReply::downloadProgress, status, &WebDownloadStatus::progress);
    }

    loop.exec();

    bool success = reply->error() == QNetworkReply::NoError;
    if(success) {
        output.clear();
        output = reply->readAll();
    }

    return success;
}

bool WebUtils::upload(const QUrl& url, QByteArray& output, const QByteArray& input, const QString& name, const QString& filename, WebDownloadStatus* status) {
    QString bound = "---------------------------7d935033608e2";

    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n\r");
    data.append("\r\n");
    data.append(QString("--" + bound + "\r\n").toLatin1());
    data.append(QString("Content-Disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"\r\n").toLatin1());
    data.append("Content-Type: application/octet-stream\r\n\r\n");
    data.append(input);
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + bound);
    req.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(data.size()));

    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.post(req, data);
    if(reply == nullptr) {
        return false;
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    if(status != nullptr) {
        QObject::connect(reply, &QNetworkReply::downloadProgress, status, &WebDownloadStatus::progress);
    }

    loop.exec();

    bool success = reply->error() == QNetworkReply::NoError;
    if(success) {
        output.clear();
        output = reply->readAll();
    }

    return success;
}
