#ifndef WEIBOMM_PENDINGLOGIN_H
#define WEIBOMM_PENDINGLOGIN_H
#include "networkmanager.h"
#include "qjson/parser.h"
#include "qjson/serializer.h"
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileInfo>
namespace PendingLogin {
inline QString path(){return QDir::homePath()+"/.cache/weibomm-pending-login.json";}
inline bool trusted(const QUrl &u){QString h=u.host().toLower();return (u.scheme()=="https"||u.scheme()=="http")&&u.userInfo().isEmpty()&&u.port(u.scheme()=="https"?443:80)==(u.scheme()=="https"?443:80)&&(h=="passport.sina.cn"||h=="weibo.com"||h.endsWith(".weibo.com")||h=="weibo.cn"||h.endsWith(".weibo.cn")||h.endsWith(".sina.com.cn"));}
inline void clear(const QString &file=path()){QFile::remove(file);QFile::remove(file+".new");}
inline QVariantMap load(const QString &file=path()){
 QFile f(file);if(!f.open(QIODevice::ReadOnly))return QVariantMap();if(f.size()>131072){f.close();clear(file);return QVariantMap();}
 bool ok=false;QJson::Parser parser;QVariantMap state=parser.parse(f.readAll(),&ok).toMap();f.close();qint64 age=QDateTime::currentMSecsSinceEpoch()-state.value("created").toLongLong();QString url=state.value("url").toString();QVariantList cookies=state.value("cookies").toList();
 if(!ok||state.value("version").toInt()!=1||age<0||age>=900000||url.size()>8192||!trusted(QUrl::fromEncoded(url.toUtf8()))||cookies.size()>200){clear(file);return QVariantMap();}
 for(int i=0;i<cookies.size();i++)if(cookies[i].toString().size()>4096){clear(file);return QVariantMap();}
 return state;
}
inline bool save(const QUrl &url,QNetworkCookieJar *jar,qint64 created=0,const QString &file=path()){
 if(!jar||!trusted(url)||url.toEncoded().size()>8192)return false;
 if(!created)created=QDateTime::currentMSecsSinceEpoch();
 idNetworkCookieJar *native=qobject_cast<idNetworkCookieJar*>(jar);if(!native)return false;
 QVariantList values;foreach(const QNetworkCookie &cookie,native->GetAllCookies()){
 QString h=cookie.domain().toLower();if(h.startsWith('.'))h.remove(0,1);
 if(!(h=="passport.sina.cn"||h=="weibo.com"||h.endsWith(".weibo.com")||h=="weibo.cn"||h.endsWith(".weibo.cn")||h=="sina.cn"||h=="sina.com.cn"||h.endsWith(".sina.com.cn")))continue;
 QByteArray raw=cookie.toRawForm();if(raw.size()>4096||values.size()>=200)return false;values<<QString::fromLatin1(raw);
 }
 QVariantMap state;state.insert("version",1);state.insert("created",created);state.insert("url",QString::fromLatin1(url.toEncoded()));state.insert("cookies",values);QJson::Serializer serializer;QByteArray bytes=serializer.serialize(state);if(bytes.size()>131072)return false;
 QDir().mkpath(QFileInfo(file).absolutePath());QFile f(file+".new");if(!f.open(QIODevice::WriteOnly|QIODevice::Truncate))return false;
 if(!f.setPermissions(QFile::ReadOwner|QFile::WriteOwner)){f.close();QFile::remove(file+".new");return false;}
 bool ok=f.write(bytes)==bytes.size()&&f.flush();f.close();if(!ok){QFile::remove(file+".new");return false;}QFile::remove(file);return QFile::rename(file+".new",file);
}
inline bool restore(const QVariantMap &state){
 if(state.isEmpty())return false;QList<QNetworkCookie> cookies;
 foreach(const QVariant &value,state.value("cookies").toList()){
 QList<QNetworkCookie> parsed=QNetworkCookie::parseCookies(value.toString().toLatin1());
 foreach(const QNetworkCookie &c,parsed){QString h=c.domain().toLower();if(h.startsWith('.'))h.remove(0,1);if(h=="passport.sina.cn"||h=="weibo.com"||h.endsWith(".weibo.com")||h=="weibo.cn"||h.endsWith(".weibo.cn")||h=="sina.cn"||h=="sina.com.cn"||h.endsWith(".sina.com.cn"))cookies<<c;}
 }
 idNetworkCookieJar::Instance()->SetAllCookies(cookies);return true;
}
}
#endif
