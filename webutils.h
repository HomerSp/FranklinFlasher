#ifndef WEBUTILS_H
#define WEBUTILS_H

#include <QEventLoop>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>

namespace Web {
    class WebDownloadStatus : public QObject {
        Q_OBJECT
    public:

    signals:
        void progress(qint64 received, qint64 total);
    };

    class WebUtils
    {
    public:
        static bool download(const QUrl& url, QByteArray& output);
        static bool download(const QUrl& url, QByteArray& output, QString postData, WebDownloadStatus* status = nullptr);
        static bool download(const QUrl& url, QByteArray& output, const QHash<QString, QString> &headers, QString postData = "", WebDownloadStatus* status = nullptr);

        static bool upload(const QUrl& url, QByteArray& output, const QByteArray& input, const QString& name, const QString& filename, WebDownloadStatus* status = nullptr);

    private:
        WebUtils();
    };
}

#endif // WEBUTILS_H
