#include "authv2.h"
#include "networkconnector.h"
#include "diagnostic.h"
#include "pendinglogin.h"
#include "qjson/parser.h"
#include <QUrl>
#include <QDateTime>
#include <openssl/pem.h>
#include <openssl/rsa.h>

namespace {
const QString Host="https://passport.weibo.com";
QVariantMap headers() {
 QVariantMap h;
 h.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");
 h.insert("Content-Type","application/x-www-form-urlencoded");
 h.insert("Accept","application/json, text/plain, */*");
 h.insert("Origin",Host);
 h.insert("Referer",Host+"/sso/signin?entry=wapsso&source=wapssowb&url=https%3A%2F%2Fm.weibo.cn%2F");
 h.insert("X-Requested-With","XMLHttpRequest");
 return h;
}
int request(QDeclarativeEngine *engine,const QString &url,const QVariantMap &form,
 QByteArray *body,int method=1,bool redirect=false) {
 if(engine) {
  idNetworkConnector c;c.SetEngine(engine);
  return c.SyncRequest(url,QList<QPair<QByteArray,QByteArray> >(),idNetworkConnector::MakePostData(form),method,headers(),body,redirect);
 }
 return idNetworkConnector::SyncRequest_thread(url,QList<QPair<QByteArray,QByteArray> >(),idNetworkConnector::MakePostData(form),method,headers(),body,redirect);
}
QVariantMap parse(const QByteArray &body,bool *ok) { QJson::Parser p;return p.parse(body,ok).toMap(); }
QVariantMap entry() {QVariantMap v;v.insert("entry","wapsso");v.insert("source","wapssowb");return v;}
QByteArray encrypt(const QString &pem,const QByteArray &secret) {
 QByteArray bytes=pem.toAscii();BIO *bio=BIO_new_mem_buf(bytes.data(),bytes.size());
 if(!bio)return QByteArray();
 RSA *rsa=PEM_read_bio_RSA_PUBKEY(bio,0,0,0);BIO_free(bio);
 if(!rsa)return QByteArray();
 QByteArray cipher(RSA_size(rsa),0);
 int n=secret.size()<=RSA_size(rsa)-11 ? RSA_public_encrypt(secret.size(),
  reinterpret_cast<const unsigned char *>(secret.constData()),
  reinterpret_cast<unsigned char *>(cipher.data()),rsa,RSA_PKCS1_PADDING) : -1;
 RSA_free(rsa);return n>0?cipher.left(n).toHex():QByteArray();
}
// The current official form's el=2 encoding; this is serialization, not encryption.
QString encodeName(const QString &name) {
 QByteArray b=name.toUtf8(),key="x9Lp2WnQ";
 for(int i=0;i<b.size();++i)b[i]=b[i]^key[i%key.size()];
 b=b.toBase64().replace('+','-').replace('/','_');while(b.endsWith('='))b.chop(1);
 const QByteArray from="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
 const QByteArray to="iaAehJvQ7RIDBX5L31T0kcf9ElVbpMdPZSmzoN_uxGO-2rFUtyY6sHCwKj4Wqgn8";
 for(int i=0;i<b.size();++i){int j=from.indexOf(b[i]);if(j<0)return QString();b[i]=to[j];}
 return QString::fromLatin1(b);
}
bool trustedSso(const QUrl &url) {
 QString h=url.host().toLower();
 return url.scheme()=="https" && url.userInfo().isEmpty() && url.port(443)==443 &&
 (h=="passport.sina.cn"||h=="weibo.com" || h.endsWith(".weibo.com") || h=="weibo.cn" || h.endsWith(".weibo.cn") || h.endsWith(".sina.com.cn"));
}
QVariantMap failure(const QString &text,int code=9999) {
 if(code==20000000 || code==200000)code=9999;
 QVariantMap v;v.insert("retcode",code);v.insert("_diagnostic",text);return v;
}
}

int probeLoginV2() {
 bool encodingOk=encodeName("n9-test-user")=="JZaSaJpovutXTmoA" && encodeName(QString::fromUtf8("测试账号"))=="ux6Q_RwASF9dwBLQ";
 diagnostic(QString("v2_encoding_test ok=%1").arg(encodingOk?1:0));
 if(!encodingOk)return 2;
 QByteArray body;int rc=request(0,Host+"/sso/v2/web/config",entry(),&body);
 bool ok=false;QVariantMap config=parse(body,&ok),d=config.value("data").toMap();
 QByteArray encrypted=encrypt(d.value("pubkey").toString(),QByteArray("diagnostic-only-not-a-user-password"));
 diagnostic(QString("v2_config_probe network=%1 json=%2 retcode=%3 rsa_bytes=%4 password_enabled=%5 sms_enabled=%6")
  .arg(rc).arg(ok?1:0).arg(config.value("retcode").toInt()).arg(encrypted.size()/2).arg(d.value("show_pw").toInt()).arg(d.value("show_sms").toInt()));
 return rc==0 && ok && config.value("retcode").toInt()==20000000 && !encrypted.isEmpty()?0:1;
}

QVariantMap loginV2(QDeclarativeEngine *engine,const QVariantMap &params,int *r) {
 PendingLogin::clear();
 *r=0;QByteArray body;int rc=0;bool ok=false;QVariantMap config,d;
 QString verifiedId=params.value("verification_id").toString();
 if(!verifiedId.isEmpty()) { d=params.value("verification_config").toMap(); }
 else {
  rc=request(engine,Host+"/sso/signin?entry=wapsso&source=wapssowb&url=https%3A%2F%2Fm.weibo.cn%2F",QVariantMap(),&body,0,false);
  if(rc){*r=rc;return failure(QString::fromUtf8("登录页面初始化失败，Qt %1").arg(rc));}
  rc=request(engine,Host+"/sso/v2/web/config",entry(),&body);
  if(rc){*r=rc;return failure(QString::fromUtf8("登录配置连接失败，Qt %1").arg(rc));}
  config=parse(body,&ok);d=config.value("data").toMap();
  diagnostic(QString("v2_config json=%1 retcode=%2").arg(ok?1:0).arg(config.value("retcode").toInt()));
  if(!ok || config.value("retcode").toInt()!=20000000)return failure(QString::fromUtf8("登录配置无效"));
 }
 const QString action=params.value("action","password").toString();
 const bool smsSend=action=="sms_send",smsLogin=action=="sms_login";
 if((smsSend||smsLogin) && !d.value("show_sms").toBool())return failure(QString::fromUtf8("服务器暂不提供短信登录"));
 QVariantMap form=entry();QByteArray pass;
 if(smsSend){
  form.insert("mobile",encodeName(params.value("username").toString()));form.insert("el",2);
  if(!verifiedId.isEmpty())form.insert("mfa_id",verifiedId);
 }else{
  form.insert("type",smsLogin?2:1);form.insert("url","https://m.weibo.cn/");
  form.insert("username",encodeName(params.value("username").toString()));form.insert("el",2);
  if(!verifiedId.isEmpty())form.insert("cid",verifiedId);
  if(smsLogin){form.insert("scode",encodeName(params.value("password").toString()));}
  else{
   if(!d.value("show_pw").toBool())return failure(QString::fromUtf8("服务器暂不提供密码登录"));
   QByteArray secret=(d.value("servertime").toString()+"\t"+d.value("nonce").toString()+"\n"+params.value("password").toString()).toUtf8();
   pass=encrypt(d.value("pubkey").toString(),secret);secret.fill(0);
   if(pass.isEmpty())return failure(QString::fromUtf8("密码加密失败或密码过长"));
   form.insert("pass",pass);form.insert("pwencode","rsa");form.insert("rsakv",d.value("rsakv"));
  }
  form.insert("rid","norid");form.insert("ver","20250520");
 }
 rc=request(engine,Host+(smsSend?"/sso/v2/sms/send":"/sso/v2/login"),form,&body);pass.fill(0);form.clear();
 if(rc){*r=rc;return failure(QString::fromUtf8("登录连接失败，Qt %1").arg(rc));}
 QVariantMap result=parse(body,&ok),data=result.value("data").toMap();
 if(!ok)return failure(QString::fromUtf8("登录接口未返回有效 JSON"));
 int code=result.value("retcode").toInt();QString act=data.value("act").toString();
 diagnostic(QString("v2_login retcode=%1 verification=%2").arg(code).arg(act=="mfa_1"||act=="mfa_2"?1:0));
 if(act=="mfa_1") {
  QVariantMap f=failure(QString::fromUtf8("请完成微博官方安全验证"),code);
  f.insert("_action",action);f.insert("_mfa_id",data.value("mfa_id"));f.insert("_mfa_config",d);return f;
 }
 if(act=="mfa_2")return failure(QString::fromUtf8("微博要求图形验证码，当前验证页尚未支持"),code);
 if(code!=20000000 && code!=200000) {
  // Server message is shown only on device; it is never logged.
  return failure(QString::fromUtf8("微博返回 %1：%2").arg(code).arg(result.value("msg").toString().left(160)),code);
 }
 if(smsSend){QVariantMap sent;sent.insert("retcode",201);sent.insert("_sms_sent",true);return sent;}
 if(act!="goto")return failure(QString::fromUtf8("登录返回成功，但缺少会话跳转"));
 QUrl next=QUrl::fromEncoded(data.value("location").toString().toUtf8());
 if(!trustedSso(next))return failure(QString::fromUtf8("登录跳转需要另行验证"));
 PendingLogin::save(next,idNetworkCookieJar::Instance());
 QVariantMap pending;pending.insert("retcode",202);pending.insert("_session_url",next.toEncoded());
 pending.insert("_diagnostic",QString::fromUtf8("正在通过官方页面同步登录状态"));return pending;
}

QVariantMap checkLoginSession(QDeclarativeEngine *engine) {
 QByteArray body;bool ok=false;int rc=0;QVariantMap result;
 rc=request(engine,"https://m.weibo.cn/api/config",QVariantMap(),&body,0,false);
 if(rc)return failure(QString::fromUtf8("登录状态确认失败"));
 QVariantMap state=parse(body,&ok).value("data").toMap();
 QString uid=state.value("uid").toString();QVariantMap user=state.value("user").toMap();
 if(uid.isEmpty())uid=user.value("id").toString();
 if(!ok || !state.value("login").toBool() || uid.isEmpty() || uid=="0"){
 QVariantMap f=failure(QString::fromUtf8("会话尚未登录，可能需要网页确认"));f.insert("_session_state_checked",ok && state.contains("login"));return f;
 }
 QVariantMap logged;logged.insert("uid",uid);logged.insert("nick",user.value("screen_name"));logged.insert("avatar",user.value("profile_image_url"));
 result.clear();result.insert("retcode",20000000);result.insert("data",logged);return result;
}
