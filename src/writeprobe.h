#ifndef WEIBOMM_WRITEPROBE_H
#define WEIBOMM_WRITEPROBE_H
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDeclarativeNetworkAccessManagerFactory>
#include <cstring>
#include <QTimer>
struct WriteProbeState {int gets,posts;int mask;bool valid,login,writeSuccess;WriteProbeState():gets(0),posts(0),mask(0),valid(true),login(true),writeSuccess(true){}};
class WriteProbeReply:public QNetworkReply {
 QByteArray body;int offset;
 public:WriteProbeReply(const QNetworkRequest &request,const QByteArray &payload,QObject *parent):QNetworkReply(parent),body(payload),offset(0){setRequest(request);setUrl(request.url());setAttribute(QNetworkRequest::HttpStatusCodeAttribute,200);open(QIODevice::ReadOnly);setProperty("_weibommFinished",true);QTimer::singleShot(0,this,SIGNAL(finished()));}
 void abort(){}qint64 bytesAvailable() const{return body.size()-offset+QNetworkReply::bytesAvailable();}
 protected:qint64 readData(char *out,qint64 n){int size=qMin(qint64(body.size()-offset),n);if(size<=0)return -1;std::memcpy(out,body.constData()+offset,size);offset+=size;return size;}
};
class WriteProbeManager:public QNetworkAccessManager {
 WriteProbeState *state;
 public:WriteProbeManager(WriteProbeState *s,QObject *p):QNetworkAccessManager(p),state(s){}
 protected:QNetworkReply *createRequest(Operation op,const QNetworkRequest &r,QIODevice *data){
  QByteArray response;
  if(op==GetOperation){state->gets++;state->valid=state->valid&&r.url().path()=="/api/config";response=state->login?"{\"ok\":1,\"data\":{\"login\":true,\"st\":\"fixture-new-token\"}}":"{\"ok\":1,\"data\":{\"login\":false,\"st\":\"fixture-new-token\"}}";}
  else {state->posts++;QByteArray form=data?data->readAll():QByteArray();if(op!=PostOperation)state->mask|=1;if(!r.rawHeader("Content-Type").startsWith("application/x-www-form-urlencoded"))state->mask|=2;if(r.rawHeader("x-xsrf-token")!="fixture-new-token")state->mask|=4;if(!form.contains("st=fixture-new-token"))state->mask|=8;if(r.rawHeader("Origin")!="https://m.weibo.cn")state->mask|=16;if((r.url().path()=="/api/statuses/update"||r.url().path()=="/api/comments/create")&&!form.contains("content=%E6%B5%8B%E8%AF%95%26%2B"))state->mask|=32;if(r.url().path()=="/api/comments/create"&&(!form.contains("mid=fixture-id")||!form.contains("dualPost=1")))state->mask|=64;if(state->posts==6&&r.url().path()!="/api/attitudes/destroy")state->mask|=128;state->valid=state->valid&&op==PostOperation&&r.rawHeader("Content-Type").startsWith("application/x-www-form-urlencoded")&&r.rawHeader("x-xsrf-token")=="fixture-new-token"&&form.contains("st=fixture-new-token")&&r.rawHeader("Origin")=="https://m.weibo.cn";response=state->writeSuccess?"{\"ok\":1}":"{\"ok\":0,\"errno\":100001,\"msg\":\"Fixture rejection\"}";if(r.url().path()=="/api/statuses/update"||r.url().path()=="/api/comments/create")state->valid=state->valid&&form.contains("content=%E6%B5%8B%E8%AF%95%26%2B");if(r.url().path()=="/api/comments/create")state->valid=state->valid&&form.contains("mid=fixture-id")&&form.contains("dualPost=1");if(state->posts==6)state->valid=state->valid&&r.url().path()=="/api/attitudes/destroy";}
  return new WriteProbeReply(r,response,this);
 }
};
class WriteProbeFactory:public QDeclarativeNetworkAccessManagerFactory {
 public:WriteProbeState state;QNetworkAccessManager *create(QObject *parent){return new WriteProbeManager(&state,parent);}
};
#endif
