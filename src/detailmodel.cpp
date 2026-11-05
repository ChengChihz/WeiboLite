#include "detailmodel.h"

#include <QDebug>

#include "api.h"
#include "id_std.h"
#include "helper.h"
#include "qmlapplicationviewer.h"
#include "qjson/parser.h"
#include "diagnostic.h"
#include <QDeclarativeEngine>
#include <QNetworkAccessManager>
#include <QTimer>
extern QmlApplicationViewer *qml_viewer;

	id_DetailModel::id_DetailModel(QObject *parent)
: QObject(parent),
	m_hasMapFlag(false),
	m_retweetedRepostCount(0),
	m_retweetedCommentCount(0),
	m_statusCommentCount(0),
	m_statusRepostCount(0),
	m_userVerified(false),
	m_latitude(0.0),
	m_longitude(0.0),
	m_connected(false)
{
	m_likeCount=0;m_liked=false;m_generation=0;
 setObjectName("id_DetailModel");
}

id_DetailModel::~id_DetailModel()
{
 CancelRequests();
	ID_QOBJECT_DESTROY_DBG
}

bool id_DetailModel::HasMapFlag() const
{
	return m_hasMapFlag;
}

QString id_DetailModel::StatusText() const
{
	return m_statusText;
}

QString id_DetailModel::StatusImage() const
{
	return m_statusImage;
}

QString id_DetailModel::RetweetedID() const
{
	return m_retweetedID;
}

QString id_DetailModel::RetweetedUserName() const
{
	return m_retweetedUserName;
}

QString id_DetailModel::RetweetedText() const
{
	return m_retweetedText;
}

QString id_DetailModel::RetweetedImage() const
{
	return m_retweetedImage;
}

int id_DetailModel::RetweetedRepostCount() const
{
	return m_retweetedRepostCount;
}

int id_DetailModel::RetweetedCommentCount() const
{
	return m_retweetedCommentCount;
}

QString id_DetailModel::MapUrl() const
{
	return m_mapUrl;
}

QString id_DetailModel::StatusID() const
{
	return m_statusID;
}

int id_DetailModel::StatusCommentCount() const
{
	return m_statusCommentCount;
}

int id_DetailModel::StatusRepostCount() const
{
	return m_statusRepostCount;
}

QString id_DetailModel::StatusSource() const
{
	return m_statusSource;
}

QString id_DetailModel::StatusCreateTime() const
{
	return m_statusCreateTime;
}

bool id_DetailModel::UserVerified() const
{
	return m_userVerified;
}

QString id_DetailModel::UserProfileImg() const
{
	return m_userProfileImg;
}

QString id_DetailModel::UserName() const
{
	return m_userName;
}

QString id_DetailModel::UserID() const
{
	return m_userID;
}

qreal id_DetailModel::Latitude() const
{
	return m_latitude;
}

qreal id_DetailModel::Longitude() const
{
	return m_longitude;
}

void id_DetailModel::SetHasMapFlag(bool value)
{
	if(m_hasMapFlag != value)
	{
		m_hasMapFlag = value;
		emit hasMapFlagChanged(m_hasMapFlag);
	}
}

void id_DetailModel::SetStatusText(const QString &value)
{
	if(m_statusText != value)
	{
		m_statusText = value;
		emit statusTextChanged(m_statusText);
	}
}

void id_DetailModel::SetStatusImage(const QString &value)
{
	if(m_statusImage != value)
	{
		m_statusImage = value;
		emit statusImageChanged(m_statusImage);
	}
}

void id_DetailModel::SetRetweetedID(const QString &value)
{
	if(m_retweetedID != value)
	{
		m_retweetedID = value;
		emit retweetedIDChanged(m_retweetedID);
	}
}

void id_DetailModel::SetRetweetedUserName(const QString &value)
{
	if(m_retweetedUserName != value)
	{
		m_retweetedUserName = value;
		emit retweetedUserNameChanged(m_retweetedUserName);
	}
}

void id_DetailModel::SetRetweetedText(const QString &value)
{
	if(m_retweetedText != value)
	{
		m_retweetedText = value;
		emit retweetedTextChanged(m_retweetedText);
	}
}

void id_DetailModel::SetRetweetedImage(const QString &value)
{
	if(m_retweetedImage != value)
	{
		m_retweetedImage = value;
		emit retweetedImageChanged(m_retweetedImage);
	}
}

void id_DetailModel::SetRetweetedRepostCount(int value)
{
	if(m_retweetedRepostCount != value)
	{
		m_retweetedRepostCount = value;
		emit retweetedRepostCountChanged(m_retweetedRepostCount);
	}
}

void id_DetailModel::SetRetweetedCommentCount(int value)
{
	if(m_retweetedCommentCount != value)
	{
		m_retweetedCommentCount = value;
		emit retweetedCommentCountChanged(m_retweetedCommentCount);
	}
}

void id_DetailModel::SetMapUrl(const QString &value)
{
	if(m_mapUrl != value)
	{
		m_mapUrl = value;
		emit mapUrlChanged(m_mapUrl);
	}
}

void id_DetailModel::SetStatusID(const QString &value)
{
	if(m_statusID != value)
	{
		m_statusID = value;
		emit statusIDChanged(m_statusID);
	}
}

void id_DetailModel::SetStatusCommentCount(int value)
{
	if(m_statusCommentCount != value)
	{
		m_statusCommentCount = value;
		emit statusCommentCountChanged(m_statusCommentCount);
	}
}

void id_DetailModel::SetStatusRepostCount(int value)
{
	if(m_statusRepostCount != value)
	{
		m_statusRepostCount = value;
		emit statusRepostCountChanged(m_statusRepostCount);
	}
}

void id_DetailModel::SetStatusSource(const QString &value)
{
	if(m_statusSource != value)
	{
		m_statusSource = value;
		emit statusSourceChanged(m_statusSource);
	}
}

void id_DetailModel::SetStatusCreateTime(const QString &value)
{
	if(m_statusCreateTime != value)
	{
		m_statusCreateTime = value;
		emit statusCreateTimeChanged(m_statusCreateTime);
	}
}

void id_DetailModel::SetUserVerified(bool value)
{
	if(m_userVerified != value)
	{
		m_userVerified = value;
		emit userVerifiedChanged(m_userVerified);
	}
}

void id_DetailModel::SetUserProfileImg(const QString &value)
{
	if(m_userProfileImg != value)
	{
		m_userProfileImg = value;
		emit userProfileImgChanged(m_userProfileImg);
	}
}

void id_DetailModel::SetUserName(const QString &value)
{
	if(m_userName != value)
	{
		m_userName = value;
		emit userNameChanged(m_userName);
	}
}

void id_DetailModel::SetUserID(const QString &value)
{
	if(m_userID != value)
	{
		m_userID = value;
		emit userIDChanged(m_userID);
	}
}

void id_DetailModel::SetLatitude(qreal value)
{
	if(m_latitude != value)
	{
		m_latitude = value;
		emit latitudeChanged(m_latitude);
	}
}

void id_DetailModel::SetLongitude(qreal value)
{
	if(m_longitude != value)
	{
		m_longitude = value;
		emit longitudeChanged(m_longitude);
	}
}

void id_DetailModel::connectModel()
{
	m_connected = true;
}

void id_DetailModel::disConnectModel()
{
	m_connected = false;CancelRequests();
}

void id_DetailModel::CancelRequests(){foreach(QNetworkReply *reply,m_replies){reply->disconnect(this);reply->abort();reply->deleteLater();}m_replies.clear();}
void id_DetailModel::StartRequest(const QUrl &url,int stage,int redirects){
 if(!qml_viewer)return;QNetworkRequest request(url);request.setRawHeader("Referer","https://m.weibo.cn/");QNetworkReply *reply=qml_viewer->engine()->networkAccessManager()->get(request);reply->setProperty("detailStage",stage);reply->setProperty("detailGeneration",m_generation);reply->setProperty("detailRedirects",redirects);m_replies<<reply;connect(reply,SIGNAL(finished()),this,SLOT(DetailFinished()));
}
void id_DetailModel::getSinaWeiboDetail(const QString &statusID){
 CancelRequests();m_generation++;if(statusID.isEmpty())return;if(m_connected)emit getRequestResult(idAPI::ErrCode_Loading);
 StartRequest(QUrl("https://m.weibo.cn/detail/"+statusID),0);diagnostic("detail_async requested=1");
}
void id_DetailModel::DetailFinished(){
 QNetworkReply *reply=qobject_cast<QNetworkReply*>(sender());if(!reply||!m_replies.removeOne(reply))return;
 int stage=reply->property("detailStage").toInt(),generation=reply->property("detailGeneration").toInt(),redirects=reply->property("detailRedirects").toInt();QUrl next=reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();int http=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();bool transport=reply->error()==QNetworkReply::NoError;QByteArray bytes=reply->readAll();reply->deleteLater();if(generation!=m_generation)return;
 if((http==301||http==302)&&redirects<3&&next.scheme()=="https"&&next.host()=="m.weibo.cn"){StartRequest(next,stage,redirects+1);return;}
 QJson::Parser parser;bool ok=false;QVariantMap root;
 if(stage==0){QByteArray marker="var $render_data = [";int begin=bytes.indexOf(marker),end=begin>=0?bytes.indexOf("][0]",begin+marker.size()):-1;if(begin>=0&&end>begin)root=parser.parse(bytes.mid(begin+marker.size(),end-begin-marker.size()),&ok).toMap();}
 else root=parser.parse(bytes,&ok).toMap();bytes.fill(0);
 if(stage==0){QVariantMap status=root.value("status").toMap();bool loaded=transport&&ok&&!status.value("id").toString().isEmpty();diagnostic(QString("detail_async loaded=%1").arg(loaded?1:0));if(!loaded){if(m_connected)emit getRequestResult(idAPI::ErrCode_Error);return;}ApplyStatus(status);if(m_connected)emit getRequestResult(idAPI::ErrCode_Success);
  if(status.value("isLongText").toBool()){QUrl url("https://m.weibo.cn/statuses/extend");url.addQueryItem("id",StatusID());StartRequest(url,1);}QVariantMap forwarded=status.value("retweeted_status").toMap();if(forwarded.value("isLongText").toBool()){QUrl url("https://m.weibo.cn/statuses/extend");url.addQueryItem("id",RetweetedID());StartRequest(url,2);}
 }else if(transport&&ok&&root.value("ok").toInt()){QString text=root.value("data").toMap().value("longTextContent").toString();if(!text.isEmpty()){if(stage==1)SetStatusText(idHelper::FixedContentEmotion(text));else SetRetweetedText(idHelper::FixedContentEmotion(text));}}
}
void id_DetailModel::ApplyStatus(const QVariantMap &incoming){
 m_pageInfo.clear();QVariantMap status=incoming,user;
	user = status["user"].toMap();

	SetHasMapFlag(false);
	SetStatusText(idHelper::FixedContentEmotion(status["text"].toString()));
	//SetStatusImage(status["thumbnail_pic"].toString());
	SetStatusImage(status["original_pic"].toString());
	SetMapUrl("");
	SetStatusID(status["id"].toString());
	SetStatusCommentCount(status["comments_count"].toInt());
	SetStatusRepostCount(status["reposts_count"].toInt());
 m_likeCount=status.value("attitudes_count").toInt();m_liked=status.value("attitudes_status").toInt()==1;emit likesChanged();
	SetStatusSource(status["source"].toString());
	SetStatusCreateTime(status["created_at"].toString());
	SetUserVerified(user["verified"].toBool());
	SetUserProfileImg(user["profile_image_url"].toString());
	SetUserName(user["screen_name"].toString());
	SetUserID(user["id"].toString());
	SetLatitude(0.0);
	SetLongitude(0.0);

	AddPageInfo(status["page_info"]);

	status = status["retweeted_status"].toMap();

	if(status.isEmpty())
	{
		SetRetweetedID("");
		SetRetweetedUserName("");
		SetRetweetedText("");
		SetRetweetedImage("");
		SetRetweetedRepostCount(0);
		SetRetweetedCommentCount(0);
		SetRetweetedUserName("");
	}
	else
	{
		user = status["user"].toMap();
		SetRetweetedID(status["id"].toString());
		SetRetweetedText(idHelper::FixedContentEmotion(status["text"].toString()));
		//SetRetweetedImage(status["thumbnail_pic"].toString());
		SetRetweetedImage(status["original_pic"].toString());
		SetRetweetedRepostCount(status["reposts_count"].toInt());
		SetRetweetedCommentCount(status["comments_count"].toInt());
		SetRetweetedUserName(user["screen_name"].toString());

	}


 AddPageInfo(status["page_info"]);
}

void id_DetailModel::clearData()
{
	SetHasMapFlag(false);
	SetStatusText("");
	SetStatusImage("");
	SetMapUrl("");
	SetStatusID("");
	SetStatusCommentCount(0);
	SetStatusRepostCount(0);
	SetStatusSource("");
	SetStatusCreateTime("");
	SetUserVerified("");
	SetUserProfileImg("");
	SetUserName("");
	SetUserID("");
	SetLatitude(0.0);
	SetLongitude(0.0);
	SetRetweetedID("");
	SetRetweetedUserName("");
	SetRetweetedText("");
	SetRetweetedImage("");
	SetRetweetedRepostCount(0);
	SetRetweetedCommentCount(0);
	SetRetweetedUserName("");

	m_pageInfo.clear();
}

void id_DetailModel::AddPageInfo(const QVariant &value)
{
	if(value.isNull())
		return;
	m_pageInfo.push_back(value);
}

QVariant id_DetailModel::GetPageInfo() const
{
	return QVariant(m_pageInfo);
}
