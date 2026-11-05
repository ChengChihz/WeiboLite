#ifndef WEIBOMM_DIAGNOSTIC_H
#define WEIBOMM_DIAGNOSTIC_H
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QUrl>
#include <QRegExp>
inline void diagnostic(const QString &s) {
 QDir().mkpath(QDir::homePath()+"/.cache");
 QFile f(QDir::homePath()+"/.cache/weibomm-diagnostic.log");
 if(f.size()>65536) f.remove();
 if(f.open(QIODevice::WriteOnly|QIODevice::Append)) {
 f.setPermissions(QFile::ReadOwner|QFile::WriteOwner);
 f.write((QDateTime::currentDateTime().toString(Qt::ISODate)+" "+s+"\n").toUtf8());
 }
}
inline void diagnosticSsoUrl(const QString &stage,const QUrl &url){
 QString h=url.host().toLower(),p=url.path();QRegExp host("^[a-z0-9.-]{1,80}$"),path("^/[A-Za-z0-9_./-]{1,80}$"),digits("[0-9]{4}");
 if(!host.exactMatch(h))h="redacted";if(!path.exactMatch(p)||digits.indexIn(p)>=0)p="redacted";
 int scheme=url.scheme()=="https"?1:url.scheme()=="http"?2:0;
 diagnostic(QString("%1 scheme=%2 host=%3 path=%4").arg(stage).arg(scheme).arg(h).arg(p));
}
#endif
