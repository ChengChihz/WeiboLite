#ifndef WEIBOMM_BROWSERSESSION_H
#define WEIBOMM_BROWSERSESSION_H
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QRegExp>
#include <QSet>
inline bool parseBrowserSession(const QByteArray &raw,QList<QNetworkCookie> *out){
 out->clear();if(raw.isEmpty()||raw.size()>16384)return false;QByteArray input=raw.trimmed();if(input.toLower().startsWith("cookie:"))input=input.mid(7).trimmed();
 if(input.contains('\r')||input.contains('\n'))return false;QSet<QByteArray> names;
 foreach(QByteArray pair,input.split(';')){
  pair=pair.trimmed();if(pair.isEmpty())continue;int at=pair.indexOf('=');if(at<1)return false;
  QByteArray name=pair.left(at),value=pair.mid(at+1);if(!QRegExp("^[A-Za-z0-9_-]{1,64}$").exactMatch(QString::fromLatin1(name))||value.size()>4096||names.contains(name))return false;
  for(int i=0;i<value.size();i++){unsigned char c=value[i];if(c<0x21||c>0x7e)return false;}
  names.insert(name);QNetworkCookie cookie(name,value);cookie.setPath("/");cookie.setSecure(true);cookie.setHttpOnly(true);out->append(cookie);if(out->size()>100)return false;
 }
 return !out->isEmpty();
}
#endif
