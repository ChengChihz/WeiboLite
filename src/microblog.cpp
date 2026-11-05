#include "microblog.h"

#include <QFileInfo>
#include <QDebug>
#include <QRegExp>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDeclarativeEngine>
#include <QDateTime>
#include "qmlapplicationviewer.h"
#include "networkconnector.h"
#include "qjson/parser.h"
#include "loginmodel.h"
extern QmlApplicationViewer *qml_viewer;
#include <QGeoPlace>
#include <QGeoCoordinate>
#include <QGeoAddress>

#include "api.h"
#include "utility.h"

QTM_USE_NAMESPACE

	id_microBlog::id_microBlog(QObject *parent)
: QObject(parent)
{
	setObjectName("id_microBlog");
}

id_microBlog::~id_microBlog()
{
 foreach(QNetworkReply *reply,m_likeReplies){if(!reply)continue;reply->disconnect(this);reply->abort();reply->deleteLater();}m_likeReplies.clear();
	ID_QOBJECT_DESTROY_DBG
}

void id_microBlog::reqGeoAddress(qreal latitude, qreal longitude)
{
	if(m_reply)
		    return;
	QGeoCoordinate coordinate(latitude, longitude);
	m_reply = m_engine->reverseGeocode(coordinate, 0);
	connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished_Slot()));
	emit readyFinishGeoAddress(idAPI::ErrCode_Loading);
}

void id_microBlog::handleTimeout()
{
	if(!m_reply)
		return;
	if(!m_reply->isFinished())
		m_reply->abort();
}

void id_microBlog::addLocation(qreal latitude, qreal longitude)
{
	// comment in qml, implement by Qml
	Q_UNUSED(latitude);
	Q_UNUSED(longitude);
}

QString id_microBlog::getGeoAddress()
{
	return m_address;
}

void id_microBlog::removeLocation()
{
#warning unimplement id_microBlog::removeLocation()
	qWarning() << QString("[Warning]: remove location is unimplement -> %1::%2()").arg(objectName()).arg(__func__);
	//idUtility::Instance()->ShowBanner("移除位置未实现!");
}

void id_microBlog::replyFinished_Slot()
{
	QObject * obj;
	QGeoSearchReply * reply;
	QList<QGeoPlace> places;
	QGeoAddress address;

	obj = sender();
	reply = qobject_cast < QGeoSearchReply *>(obj);
	if(reply != m_reply)
	{
		reply->deleteLater();
		return;
	}
	if(!m_reply)
		return;
	if(!m_reply->isFinished())
		goto __Exit;

	if(m_reply->error() != QGeoSearchReply::NoError)
		goto __Error;

	places = m_reply->places();
	if(places.isEmpty())
		goto __Error;

	address = places[0].address();
	m_address = address.country() + " " + address.district() + " " + address.county();
	qDebug() << m_address;

__Success:
	emit readyFinishGeoAddress(idAPI::ErrCode_Success);
	goto __Exit;

__Error:
	emit readyFinishGeoAddress(idAPI::ErrCode_Error);
	goto __Exit;

__Exit:
	m_reply->deleteLater();
	m_reply = 0;
}

void id_microBlog::sendNewMicroBlog(const QString &content, const QString &selectImageSrc)
{
	QStringList pics;
	if(!selectImageSrc.isEmpty())
		pics.push_back(selectImageSrc);
	SendNewMicroBlog(content, pics);
}

void id_microBlog::SendNewMicroBlog(const QString &content, const QStringList &pics)
{
	int ret;
	QVariantMap result;
	QVariantMap params;
	QStringList pic_id;

	emit readyFinishNew(idAPI::ErrCode_Loading);
	if(!pics.isEmpty()) // upload pic
	{
		for(int i = 0; i < pics.size(); i++)
		{
			if(pics[i].isEmpty())
				continue;

			QString f(pics[i]);
			if(f.startsWith("file://"))
				f = f.mid(7);
			QFileInfo info(f);
			if(!info.exists())
			{
				qDebug() << QString("[Debug]: Upload picture is not exists -> %1(%2))").arg(i).arg(f);
				goto __Error;
			}
			QByteArray picBytes = id::file_get_contents(info.absoluteFilePath());
			params.insert("type", "json");
			params.insert("filename", info.fileName());
			params.insert("filetype", "image/" + info.suffix()); // not use completeSuffix()
			params.insert("filedata", picBytes);
			result = idAPI::UploadPic(params, &ret);
			if(ret != 0)
			{
				qDebug() << QString("[Debug]: Upload picture error -> %1(%2))").arg(i).arg(f);
				goto __Error;
			}
			pic_id.push_back(result["pic_id"].toString());
			params.clear();
		}
	}

	params.insert("content", content);
	if(!pic_id.isEmpty())
		params.insert("picId", pic_id.join(","));
	result = idAPI::UpdateStatus(params, &ret);
    SetActionError(result,ret);
	if(ret != 0)
	{
		goto __Error;
	}

	if(result["ok"].toInt())
	{
		goto __Success;
	}
	else
	{
		goto __Error;
	}

__Success:
	emit readyFinishNew(idAPI::ErrCode_Success);
	return;

__Error:
	emit readyFinishNew(idAPI::ErrCode_Error);
	return;
}

void id_microBlog::sendCommentMicroBlog(const QString &statusId, const QString &content, bool checked)
{
	int ret;
	QVariantMap result;
	QVariantMap params;

	emit readyFinishComment(idAPI::ErrCode_Loading);
	params.insert("content", content);
	params.insert("mid", statusId);
	if(checked)
		params.insert("dualPost", 1);
	result = idAPI::SendCmt(params, &ret);
    SetActionError(result,ret);
	if(ret != 0)
	{
		goto __Error;
	}

	if(result["ok"].toInt())
	{
		goto __Success;
	}
	else
	{
		goto __Error;
	}

__Success:
	emit readyFinishComment(idAPI::ErrCode_Success);
	return;

__Error:
	emit readyFinishComment(idAPI::ErrCode_Error);
	return;
}

void id_microBlog::sendCommentReply(const QString &commentStatusID, const QString &content, const QString &replyRetweetedId)
{
	int ret;
	QVariantMap result;
	QVariantMap params;

	emit readyFinishCommentReply(idAPI::ErrCode_Loading);
	params.insert("content", content);
	params.insert("mid", replyRetweetedId);
	params.insert("cid", commentStatusID);
	params.insert("withReply", 1);
	result = idAPI::Reply(params, &ret);
    SetActionError(result,ret);
	if(ret != 0)
	{
		goto __Error;
	}

	if(result["ok"].toInt())
	{
		goto __Success;
	}
	else
	{
		goto __Error;
	}

__Success:
	emit readyFinishCommentReply(idAPI::ErrCode_Success);
	return;

__Error:
	emit readyFinishCommentReply(idAPI::ErrCode_Error);
	return;
}

void id_microBlog::sendForwordMicroBlog(const QString &content, const QString &statusId, bool checked)
{
	int ret;
	QVariantMap result;
	QVariantMap params;

	emit readyFinishForword(idAPI::ErrCode_Loading);
	params.insert("content", content);
	params.insert("mid", statusId);
	if(checked)
		params.insert("dualPost", 1);
	result = idAPI::Repost(params, &ret);
    SetActionError(result,ret);
	if(ret != 0)
	{
		goto __Error;
	}

	if(result["ok"].toInt())
	{
		goto __Success;
	}
	else
	{
		goto __Error;
	}

__Success:
	emit readyFinishForword(idAPI::ErrCode_Success);
	return;

__Error:
	emit readyFinishForword(idAPI::ErrCode_Error);
	return;
}

ID_SINGLE_INSTANCE_DECL(id_microBlog)

void id_microBlog::SetActionError(const QVariantMap &result,int transport){
 m_lastError.clear();if(transport||!result.value("ok").toInt()){
  QString message=result.value("msg",result.value("message",result.value("errmsg"))).toString();message.remove(QRegExp("<[^>]*>"));message=message.left(180);
  int code=result.value("errno",result.value("error_code")).toInt();
  m_lastError=message.isEmpty()?QString::fromUtf8("操作失败，请稍后重试"):message;
  if(code)m_lastError+=QString(" (%1)").arg(code);else if(transport)m_lastError+=QString(" [network %1]").arg(transport);
 }emit lastErrorChanged();
}
QVariantMap id_microBlog::toggleLike(const QString &statusId,bool currentlyLiked){
 QVariantMap result;if(statusId.isEmpty()){result.insert("ok",0);return result;}
 QVariantMap params;params.insert("id",statusId);int rc=0;result=currentlyLiked?idAPI::Unattitude(params,&rc):idAPI::Attitude(params,&rc);SetActionError(result,rc);
 QVariantMap safe;safe.insert("ok",!rc&&result.value("ok").toInt());safe.insert("message",m_lastError);return safe;
}

bool id_microBlog::requestLike(const QString &statusId,bool currentlyLiked){
 if(statusId.isEmpty()||m_likeReplies.contains(statusId)||!qml_viewer)return false;
 QUrl url("https://m.weibo.cn/api/config");url.addQueryItem("_",QString::number(QDateTime::currentMSecsSinceEpoch()));QNetworkRequest request(url);request.setRawHeader("Referer","https://m.weibo.cn/");request.setRawHeader("Accept","application/json");
 QNetworkReply *reply=qml_viewer->engine()->networkAccessManager()->get(request);reply->setProperty("likeId",statusId);reply->setProperty("likeWas",currentlyLiked);reply->setProperty("likeStage",0);reply->setProperty("likeAccount",id_loginModel::Instance()->UserID());m_likeReplies.insert(statusId,reply);connect(reply,SIGNAL(finished()),this,SLOT(LikeReplyFinished()));return true;
}
void id_microBlog::LikeReplyFinished(){
 QNetworkReply *reply=qobject_cast<QNetworkReply*>(sender());if(!reply)return;QString id=reply->property("likeId").toString(),account=reply->property("likeAccount").toString();bool was=reply->property("likeWas").toBool();int stage=reply->property("likeStage").toInt(),http=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(),transport=reply->error();if(m_likeReplies.value(id)!=reply){reply->deleteLater();return;}
 QByteArray bytes=reply->readAll();QJson::Parser parser;bool json=false;QVariantMap root=parser.parse(bytes,&json).toMap();bytes.fill(0);reply->deleteLater();bool sameAccount=account==id_loginModel::Instance()->UserID();bool success=!transport&&http==200&&json&&root.value("ok").toInt()==1&&sameAccount;
 if(stage==0&&success){QVariantMap session=root.value("data").toMap();QByteArray token=session.value("st").toByteArray();if(session.value("login").toBool()&&!token.isEmpty()){
  QNetworkRequest request(QUrl(was?"https://m.weibo.cn/api/attitudes/destroy":"https://m.weibo.cn/api/attitudes/create"));request.setRawHeader("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");request.setRawHeader("Accept","application/json");request.setRawHeader("Referer","https://m.weibo.cn/");request.setRawHeader("Origin","https://m.weibo.cn");request.setRawHeader("x-requested-with","XMLHttpRequest");request.setRawHeader("x-xsrf-token",token);QVariantMap form;form.insert("id",id);form.insert("attitude","heart");form.insert("st",token);
  QNetworkReply *next=qml_viewer->engine()->networkAccessManager()->post(request,idNetworkConnector::MakePostData(form));next->setProperty("likeId",id);next->setProperty("likeWas",was);next->setProperty("likeStage",1);next->setProperty("likeAccount",account);m_likeReplies.insert(id,next);connect(next,SIGNAL(finished()),this,SLOT(LikeReplyFinished()));token.fill(0);return;
 }success=false;}
 m_likeReplies.remove(id);QString message;
 if(!success){message=root.value("msg",root.value("message",root.value("errmsg"))).toString();message.remove(QRegExp("<[^>]*>"));message=message.left(180);if(message.isEmpty())message=QString::fromUtf8("点赞未成功，请检查网络或登录状态后重试");int code=root.value("errno",root.value("error_code")).toInt();if(code)message+=QString(" (%1)").arg(code);}
 emit likeFinished(id,stage==1&&success,message);
}
