#ifndef WEIBOMM_TLSREPLY_H
#define WEIBOMM_TLSREPLY_H
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QProcess>
#include <QTimer>
#include <QTime>

class TlsReply : public QNetworkReply {
 Q_OBJECT
public:
 TlsReply(const QNetworkRequest &, QNetworkAccessManager::Operation,
          const QByteArray &, QNetworkCookieJar *, QObject *);
public slots:
 virtual void abort();
public:
 virtual qint64 bytesAvailable() const;
protected:
 virtual qint64 readData(char *, qint64);
private slots:
 void start();
 void processFinished(int, QProcess::ExitStatus);
 void processError(QProcess::ProcessError);
 void collect();
private:
 void fail(NetworkError, const QString &);
 void complete();
 bool readStaticCache();
 void writeStaticCache();
 QString staticCachePath() const;
 QProcess process;
 QTimer timer;
 QByteArray outgoing, wire, body;
 QNetworkCookieJar *jar;
 qint64 offset;
 bool done;
 QTime elapsed;
};
#endif
