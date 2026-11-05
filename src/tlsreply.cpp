#include "tlsreply.h"
#include "diagnostic.h"
#include "pendinglogin.h"
#include <QCoreApplication>
#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QCryptographicHash>
#include <QDateTime>
#include <QRegExp>
#include <cstring>
#include <zlib.h>

static const int MaxResponse = 32 * 1024 * 1024;

static bool inflateGzip(const QByteArray &input,QByteArray *output) {
 z_stream stream;std::memset(&stream,0,sizeof(stream));
 if(inflateInit2(&stream,15+16)!=Z_OK)return false;
 stream.next_in=reinterpret_cast<Bytef*>(const_cast<char*>(input.constData()));stream.avail_in=input.size();
 char chunk[16384];int rc=Z_OK;output->clear();
 while(rc==Z_OK){stream.next_out=reinterpret_cast<Bytef*>(chunk);stream.avail_out=sizeof(chunk);
  rc=inflate(&stream,Z_NO_FLUSH);int n=sizeof(chunk)-stream.avail_out;
  if(output->size()+n>MaxResponse){inflateEnd(&stream);output->clear();return false;}
  output->append(chunk,n);
 }
 bool ok=rc==Z_STREAM_END && stream.avail_in==0;inflateEnd(&stream);if(!ok)output->clear();return ok;
}

TlsReply::TlsReply(const QNetworkRequest &req, QNetworkAccessManager::Operation op,
 const QByteArray &data, QNetworkCookieJar *cookies, QObject *parent)
 : QNetworkReply(parent), jar(cookies), offset(0), done(false) {
 setRequest(req); setUrl(req.url()); setOperation(op);
 open(QIODevice::ReadOnly | QIODevice::Unbuffered);
 QByteArray method = op == QNetworkAccessManager::PostOperation ? "POST" :
  op == QNetworkAccessManager::PutOperation ? "PUT" :
  op == QNetworkAccessManager::DeleteOperation ? "DELETE" :
  op == QNetworkAccessManager::HeadOperation ? "HEAD" : "GET";
 QByteArray target = req.url().encodedPath(); if(target.isEmpty()) target = "/";
 if(req.url().hasQuery()) target += "?" + req.url().encodedQuery();
 outgoing = method + " " + target + " HTTP/1.1\r\n";
 QByteArray host = req.url().host().toAscii();
 if(req.url().port(443) != 443) host += ":" + QByteArray::number(req.url().port());
 outgoing += "Host: " + host + "\r\nConnection: close\r\nAccept-Encoding: gzip\r\n";
 foreach(const QByteArray &key, req.rawHeaderList()) {
  const QByteArray lower = key.toLower();
  if(lower == "host" || lower == "connection" || lower == "content-length" ||
     lower == "transfer-encoding" || lower == "accept-encoding" || lower == "cookie") continue;
  const QByteArray value = req.rawHeader(key);
  if(key.contains('\r') || key.contains('\n') || value.contains('\r') || value.contains('\n')) continue;
  outgoing += key + ": " + value + "\r\n";
 }
 QByteArray cookie;
 foreach(const QNetworkCookie &c, jar->cookiesForUrl(req.url())) {
  if(!cookie.isEmpty()) cookie += "; ";
  cookie += c.toRawForm(QNetworkCookie::NameAndValueOnly);
 }
 if(!cookie.isEmpty()) outgoing += "Cookie: " + cookie + "\r\n";
 if(!data.isEmpty() || op == QNetworkAccessManager::PostOperation || op == QNetworkAccessManager::PutOperation)
  outgoing += "Content-Length: " + QByteArray::number(data.size()) + "\r\n";
 outgoing += "\r\n" + data;
 timer.setSingleShot(true);
 connect(&timer, SIGNAL(timeout()), this, SLOT(abort()));
 connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(collect()));
 connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
 connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
 QTimer::singleShot(0, this, SLOT(start()));
}

void TlsReply::start() {
 if(done) return;
 elapsed.start();
 if(readStaticCache()) { emit metaDataChanged();complete();return; }
 if(outgoing.size() >= 8*1024*1024) { fail(ContentOperationNotPermittedError,"Request too large");return; }
 const QString dir = QCoreApplication::applicationDirPath();
 process.start(dir+"/weibomm-tls", QStringList() << url().host() << QString::number(url().port(443)) << dir+"/cacert.pem");
 process.write(outgoing); process.closeWriteChannel();
 outgoing.fill(0); outgoing.clear(); timer.start(30000);
}
void TlsReply::abort() { fail(OperationCanceledError,"Request cancelled or timed out"); }
void TlsReply::collect() {
 if(done) return;
 wire += process.readAllStandardOutput();
 if(wire.size() > MaxResponse) fail(UnknownContentError,"Response too large");
}
void TlsReply::processError(QProcess::ProcessError) {
 if(!done) fail(UnknownNetworkError,"TLS helper could not run");
}
void TlsReply::fail(NetworkError errorCode, const QString &message) {
 if(done) return;
 diagnostic(QString("tls_failure code=%1 http=%2").arg(int(errorCode)).arg(attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
 setError(errorCode,message); process.kill(); complete();
}
void TlsReply::complete() {
 if(done) return;
 done=true; timer.stop(); setProperty("_weibommFinished",true);
 if(error()!=NoError) emit error(error());
 if(!body.isEmpty()) emit readyRead();
 emit finished();
}
void TlsReply::processFinished(int exitCode, QProcess::ExitStatus status) {
 if(done) return;
 collect(); if(done) return;
 const QByteArray info=process.readAllStandardError();
 const bool verified=info.contains("tls=1.2 verified=1");
 diagnostic(QString("tls_transport exit=%1 verified=%2 bytes=%3").arg(exitCode).arg(verified?1:0).arg(wire.size()));
 if(status!=QProcess::NormalExit || exitCode!=0 || !verified) {
  fail(SslHandshakeFailedError,"TLS helper handshake or transfer failed");return;
 }
 // Skip informational responses before the final HTTP/1.x response.
 int sep;
 for(;;) {
  sep=wire.indexOf("\r\n\r\n");
  if(sep<0 || sep>65536) {fail(ProtocolFailure,"Invalid HTTP headers");return;}
  int firstEnd=wire.indexOf("\r\n");
  QList<QByteArray> first=wire.left(firstEnd).split(' ');
  bool ok=false; int code=first.size()>1 ? first[1].toInt(&ok) : 0;
  if(!ok || !first[0].startsWith("HTTP/1.") || code<100 || code>599) {fail(ProtocolFailure,"Invalid HTTP status");return;}
  if(code>=200) {setAttribute(QNetworkRequest::HttpStatusCodeAttribute,code);break;}
  wire.remove(0,sep+4);
 }
 diagnostic(QString("tls_http status=%1 millis=%2").arg(attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()).arg(elapsed.elapsed()));
 const QList<QByteArray> lines=wire.left(sep).split('\n');
 bool chunked=false; qint64 length=-1;int cookieHeaders=0,parsedCookies=0,acceptedCookies=0;
 foreach(QByteArray line, lines) {
  if(line.endsWith('\r')) line.chop(1);
  int colon=line.indexOf(':'); if(colon<=0) continue;
  QByteArray name=line.left(colon), value=line.mid(colon+1).trimmed();
  setRawHeader(name,value);
  if(name.toLower()=="set-cookie"){cookieHeaders++;QList<QNetworkCookie> parsed=QNetworkCookie::parseCookies(value);parsedCookies+=parsed.size();if(jar->setCookiesFromUrl(parsed,url()))acceptedCookies+=parsed.size();}
  if(name.toLower()=="location") setAttribute(QNetworkRequest::RedirectionTargetAttribute,url().resolved(QUrl::fromEncoded(value)));
  if(name.toLower()=="transfer-encoding") chunked=value.toLower().contains("chunked");
  if(name.toLower()=="content-length") {bool ok;length=value.toLongLong(&ok);if(!ok || length<0 || length>MaxResponse){fail(ProtocolFailure,"Invalid content length");return;}}
 }
 const QString host=url().host();int tag=host=="passport.weibo.com"?1:host=="passport.weibo.cn"?2:host=="login.sina.com.cn"?3:host=="m.weibo.cn"?4:host=="passport.sina.cn"?5:0;
 if(tag)diagnostic(QString("sso_response host_tag=%1 http=%2 cookies=%3 self_redirect=%4").arg(tag).arg(attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()).arg(jar->cookiesForUrl(url()).size()).arg(attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()==url()?1:0));
 if(tag){
  QString p=url().path();int pathTag=p=="/sso/login.php"?1:p=="/sso/crossdomain.php"?2:p=="/api/config"?3:p=="/sso/v2/login"?4:p=="/sso/v2/sms/send"?5:0;
  if(tag==3){
   QString route=url().path();QRegExp safeRoute("^/[A-Za-z0-9_./-]{1,80}$");QRegExp digits("[0-9]{4}");
   if(!safeRoute.exactMatch(route)||digits.indexIn(route)>=0)route="redacted";
   diagnostic(QString("sso_route path=%1 https=%2 get=%3 raw_location_exact=%4").arg(route).arg(url().scheme()=="https"?1:0).arg(operation()==QNetworkAccessManager::GetOperation?1:0).arg(rawHeader("Location")==url().toEncoded()?1:0));
  }
  QUrl next=attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if(!next.isEmpty())diagnosticSsoUrl("sso_next",next);
  QVariantMap saved=PendingLogin::load();if(!saved.isEmpty()){
   QUrl checkpoint=QUrl::fromEncoded(saved.value("url").toString().toUtf8());if(PendingLogin::trusted(next))checkpoint=next;
   PendingLogin::save(checkpoint,jar,saved.value("created").toLongLong());
  }
  QStringList keys=QStringList()<<"ticket"<<"entry"<<"url"<<"callback"<<"client"<<"ssosavestate"<<"retcode";int queryMask=0,nextMask=0;
  for(int i=0;i<keys.size();i++){if(url().hasQueryItem(keys[i]))queryMask|=1<<i;if(next.hasQueryItem(keys[i]))nextMask|=1<<i;}
  diagnostic(QString("sso_detail host=%1 path=%2 query_mask=%3 next_query_mask=%4 same_host=%5 same_path=%6 ua_match=%7 cookie_headers=%8 parsed=%9 accepted=%10").arg(tag).arg(pathTag).arg(queryMask).arg(nextMask).arg(next.host()==url().host()?1:0).arg(next.path()==url().path()?1:0).arg(request().rawHeader("User-Agent")=="WeiboMM-N9/2026 Qt/4.7.4"?1:0).arg(cookieHeaders).arg(parsedCookies).arg(acceptedCookies));
 }
 QByteArray payload=wire.mid(sep+4);
 if(operation()==QNetworkAccessManager::HeadOperation || attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()==204 || attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()==304) payload.clear();
 else if(chunked) {
  int pos=0;
  for(;;) {
   int end=payload.indexOf("\r\n",pos); if(end<0){fail(ProtocolFailure,"Truncated chunk");return;}
   bool ok; qlonglong n=payload.mid(pos,end-pos).split(';').first().toLongLong(&ok,16);pos=end+2;
   if(!ok || n<0 || n>MaxResponse || body.size()+n>MaxResponse){fail(ProtocolFailure,"Invalid chunk size");return;}
   if(n==0) break;
   if(pos+n+2>payload.size() || payload.mid(pos+n,2)!="\r\n"){fail(ProtocolFailure,"Truncated chunk");return;}
   body+=payload.mid(pos,n);pos+=n+2;
  }
 } else {
  if(length>=0 && payload.size()!=length){fail(ProtocolFailure,"Truncated HTTP body");return;}
  body=payload;
 }
 const QByteArray encoding=rawHeader("Content-Encoding").trimmed().toLower();
 if(encoding=="gzip"){
  QByteArray decoded;if(!inflateGzip(body,&decoded)){fail(UnknownContentError,"Invalid compressed response");return;}
  diagnostic(QString("tls_gzip compressed=%1 decoded=%2").arg(body.size()).arg(decoded.size()));body=decoded;
  setRawHeader("Content-Encoding","identity");setRawHeader("Content-Length",QByteArray::number(body.size()));
 }else if(!encoding.isEmpty() && encoding!="identity") {fail(UnknownContentError,"Unsupported content encoding");return;}
 wire.clear();
 writeStaticCache();
 emit metaDataChanged();
 complete();
}
qint64 TlsReply::bytesAvailable() const { return body.size()-offset+QNetworkReply::bytesAvailable(); }
qint64 TlsReply::readData(char *buffer,qint64 maxSize) {
 qint64 n=qMin(maxSize,qint64(body.size())-offset);if(n<=0)return done?-1:0;
 std::memcpy(buffer,body.constData()+offset,n);offset+=n;return n;
}

QString TlsReply::staticCachePath() const {
 if(operation()!=QNetworkAccessManager::GetOperation || url().scheme()!="https")return QString();
 const QString host=url().host();
 const QStringList hosts=QStringList()<<"h5.sinaimg.cn"<<"wx1.sinaimg.cn"<<"wx2.sinaimg.cn"<<"wx3.sinaimg.cn"<<"wx4.sinaimg.cn"<<"tvax1.sinaimg.cn"<<"tvax2.sinaimg.cn"<<"tvax3.sinaimg.cn"<<"tvax4.sinaimg.cn"<<"tva1.sinaimg.cn"<<"tva2.sinaimg.cn"<<"tva3.sinaimg.cn"<<"tva4.sinaimg.cn"<<"passport.sinaimg.cn"<<"h5.sinaimg.cn"<<"cstaticdun.126.net"<<"cstatic.dun.163yun.com";
 const QString path=url().path().toLower();
 if(!hosts.contains(host) || (!path.endsWith(".js") && !path.endsWith(".css") && !path.endsWith(".png") && !path.endsWith(".jpg") && !path.endsWith(".jpeg") && !path.endsWith(".gif")))return QString();
 return QDir::homePath()+"/.cache/weibomm-static/"+QCryptographicHash::hash(url().toEncoded(),QCryptographicHash::Sha1).toHex()+".bin";
}
bool TlsReply::readStaticCache() {
 QString path=staticCachePath();if(path.isEmpty())return false;
 QFile file(path);if(!file.open(QIODevice::ReadOnly) || file.size()>1500000)return false;
 QDataStream stream(&file);stream.setVersion(QDataStream::Qt_4_7);
 quint32 magic=0;qint64 created=0,expires=0;QByteArray type,bytes;
 stream>>magic>>created>>expires;
 if(stream.status()!=QDataStream::Ok || magic!=0x4e395301)return false;
 quint32 typeSize=0,bodySize=0;stream>>typeSize;if(typeSize>256 || typeSize>quint64(file.bytesAvailable()))return false;
 type.resize(typeSize);if(stream.readRawData(type.data(),typeSize)!=int(typeSize))return false;
 stream>>bodySize;if(bodySize>1024*1024 || bodySize>quint64(file.bytesAvailable()))return false;
 bytes.resize(bodySize);if(stream.readRawData(bytes.data(),bodySize)!=int(bodySize))return false;
 qint64 now=QDateTime::currentMSecsSinceEpoch();
 if(stream.status()!=QDataStream::Ok || magic!=0x4e395301 || now<created || now>=expires || bytes.isEmpty() || bytes.size()>1024*1024)return false;
 body=bytes;setAttribute(QNetworkRequest::HttpStatusCodeAttribute,200);
 setRawHeader("Content-Type",type);setRawHeader("Content-Length",QByteArray::number(body.size()));
 setProperty("staticCacheHit",true);diagnostic("tls_static_cache hit=1");return true;
}
void TlsReply::writeStaticCache() {
 const QString path=staticCachePath();if(path.isEmpty() || attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200 || body.isEmpty() || body.size()>1024*1024 || hasRawHeader("Set-Cookie"))return;
 const QByteArray policy=rawHeader("Cache-Control").toLower();
 const QByteArray vary=rawHeader("Vary").toLower();if(!vary.isEmpty() && vary.trimmed()!="accept-encoding")return;
 if(policy.contains("no-store") || policy.contains("private") || policy.contains("no-cache"))return;
 QRegExp age("max-age\\s*=\\s*([0-9]+)");
 if(age.indexIn(QString::fromLatin1(policy))<0)return;
 bool ok=false;qint64 seconds=age.cap(1).toLongLong(&ok);if(!ok || seconds<=0)return;
 seconds=qMin(seconds,qint64(86400));
 QDir().mkpath(QFileInfo(path).absolutePath());QFile file(path);
 if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))return;
 file.setPermissions(QFile::ReadOwner|QFile::WriteOwner);
 QDataStream stream(&file);stream.setVersion(QDataStream::Qt_4_7);
 qint64 now=QDateTime::currentMSecsSinceEpoch();
 stream<<quint32(0x4e395301)<<now<<(now+seconds*1000)<<rawHeader("Content-Type")<<body;
 diagnostic("tls_static_cache stored=1");
}
