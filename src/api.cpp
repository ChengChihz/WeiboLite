#include "api.h"
#include "authv2.h"
#include "diagnostic.h"
#include "profiledata.h"

#include <QDebug>
#include <QUrl>
#include <QDateTime>
#include <QCryptographicHash>
#include "qjson/parser.h"

#include "api_defines.h"
#include "networkconnector.h"
#include "qmlapplicationviewer.h"
#include "utility.h"
#include "id_std.h"

extern QmlApplicationViewer *qml_viewer;


ID_API_DECL(SessionConfig) {
 idNetworkConnector connector;connector.SetEngine(qml_viewer->engine());QByteArray bytes;
 QVariantMap headers;headers.insert("Accept","application/json");headers.insert("Referer",M_HOST "/");
 QList<QPair<QByteArray,QByteArray> > query;query<<qMakePair(QByteArray("_"),QByteArray::number(QDateTime::currentMSecsSinceEpoch()));
 int rc=connector.SyncRequest(M_HOST "/api/config",query,QByteArray(),idNetworkConnector::Connect_Get,headers,&bytes);
 QJson::Parser parser;bool ok=false;QVariantMap root=parser.parse(bytes,&ok).toMap();bytes.fill(0);
 if(r)*r=rc;QVariantMap data=root.value("data").toMap();
 diagnostic(QString("write_session transport=%1 json=%2 login=%3 token=%4 cookie_token=%5").arg(rc).arg(ok?1:0).arg(data.value("login").toBool()?1:0).arg(!data.value("st").toString().isEmpty()?1:0).arg(!idUtility::Instance()->GetCookie(M_HOST,"XSRF-TOKEN").toByteArray().isEmpty()?1:0));
 return root;
}
static QVariantMap writeMobileAction(const QString &path,const QString &referer,QVariantMap form,int *r) {
 int rc=-1;QVariantMap config=idAPI::SessionConfig(QVariantMap(),&rc),session=config.value("data").toMap(),result;
 if(rc||!config.value("ok").toInt()||!session.value("login").toBool()||session.value("st").toString().isEmpty()){
  result.insert("ok",0);result.insert("msg",rc?QString::fromUtf8("无法取得发布校验信息，请检查网络后重试"):QString::fromUtf8("当前登录会话无法发布，请重新同步登录"));if(r)*r=rc;return result;
 }
 QByteArray token=session.value("st").toByteArray();form.insert("st",token);
 QVariantMap headers;headers.insert("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");headers.insert("Accept","application/json");headers.insert("x-xsrf-token",token);headers.insert("x-requested-with","XMLHttpRequest");headers.insert("Referer",referer);headers.insert("Origin",M_HOST);
 idNetworkConnector connector;connector.SetEngine(qml_viewer->engine());QByteArray bytes;
 rc=connector.SyncRequest(M_HOST+path,QList<QPair<QByteArray,QByteArray> >(),idNetworkConnector::MakePostData(form),idNetworkConnector::Connect_Post,headers,&bytes,false);
 QJson::Parser parser;bool ok=false;result=parser.parse(bytes,&ok).toMap();bytes.fill(0);token.fill(0);
 diagnostic(QString("write_action route=%1 transport=%2 json=%3 ok=%4 code=%5").arg(path).arg(rc).arg(ok?1:0).arg(result.value("ok").toInt()).arg(result.value("errno",result.value("error_code")).toInt()));
 if(!ok){result.insert("ok",0);result.insert("msg",QString::fromUtf8("服务器未返回有效结果；请先检查微博是否已发送，避免重复提交"));}
 if(r)*r=rc;return result;
}

ID_API_DECL(UserProfile)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	header.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");
 header.insert("Accept","application/json");header.insert("Referer","https://m.weibo.cn/");
 connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("type", "uid"));
 querys.push_back(qMakePair<QByteArray, QByteArray>("value", params["uid"].toByteArray()));

	ret = connector.SyncRequest(M_INDEX_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("profile")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("profile")

		result = normalizeUserProfile(json.toMap(), params["uid"].toString());
 diagnostic(QString("user_profile normalized=%1").arg(result.value("ok").toInt()));

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(MyComment)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("page", params["page"].toByteArray()));

	ret = connector.SyncRequest(M_COMMENTME_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("commentme")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("commentme")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(MyAt)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("page", params["page"].toByteArray()));

	ret = connector.SyncRequest(M_ATME_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("atme")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("atme")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(MyMessage)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("page", params["page"].toByteArray()));

	ret = connector.SyncRequest(M_MESSAGE_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("message")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("message")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(MessageList)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("uid", params["uid"].toByteArray()));
	querys.push_back(qMakePair<QByteArray, QByteArray>("count", params["count"].toByteArray()));
	querys.push_back(qMakePair<QByteArray, QByteArray>("unfollowing", params["unfollowing"].toByteArray()));

	ret = connector.SyncRequest(M_MSGLIST_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("msglist")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("msglist")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Index)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;
	const char *Params[] = {
		"containerid",
		"openApp",
		"since_id", "max_id", "page",
		"page_type",
		"type",
		"uid", "value"
	};

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	for(int i = 0; i < int(sizeof(Params) / sizeof(const char *)); i++)
	{
		if(params.contains(Params[i]))
			querys.push_back(qMakePair<QByteArray, QByteArray>(Params[i], params[Params[i]].toByteArray()));
	}

	header.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");header.insert("Referer",params.value("feed_kind").toString()=="hotsearch"?"https://weibo.com/":"https://m.weibo.cn/");
    ret = connector.SyncRequest(params.value("feed_kind").toString()=="hotsearch"?"https://weibo.com/ajax/side/hotSearch":params.value("feed_kind").toString()=="following"?"https://m.weibo.cn/feed/friends":M_INDEX_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("index")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("index")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(UpdateProfile)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	QVariantMap formData;
	const char *FormName[] = {
		"gender",
		"province",
		"city",
		"year",
		"month",
		"day",
		"email",
		"url",
		"qq",
		"msn",
		"description"
	};

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	for(int i = 0; i < int(sizeof(FormName) / sizeof(formData[0])); i++)
	{
		if(params.contains(FormName[i]))
			formData.insert(FormName[i], params[FormName[i]]);
	}
	header.insert("referer", M_HOST "/users/" + params["uid"].toByteArray() + "?set=1");
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_UPDATE_PROFILE_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("update_profile")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("update_profile")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(UpdateStatus)
{
 QVariantMap form;form.insert("content",params.value("content"));if(params.contains("picId"))form.insert("picId",params.value("picId"));return writeMobileAction("/api/statuses/update",M_HOST "/compose",form,r);
}

ID_API_DECL(SendMsg)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	QVariantMap formData;
	QByteArray uid(params["uid"].toByteArray());
	QVariant xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN"));

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	formData.insert("content", params["content"]);
	formData.insert("uid", uid);
	formData.insert("st", xsrf_token); 
	formData.insert("fileId", QVariant()); 
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/message/chat?uid=" + uid + "&name=msgbox");
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_SENDMSG_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("send_message")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("send_message")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Follow)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	QVariantMap formData;
	QVariant xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN"));
	QByteArray uid(params["uid"].toByteArray());

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	formData.insert("uid", uid);
	formData.insert("st", xsrf_token);
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/profile/" + uid);
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_FOLLOW_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("follow")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("follow")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Unfollow)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	QVariantMap formData;
	QVariant xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN"));
	QByteArray uid(params["uid"].toByteArray());

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	formData.insert("uid", uid);
	formData.insert("st", xsrf_token);
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/profile/" + uid);
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_UNFOLLOW_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("unfollow")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("unfollow")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}


ID_API_DECL(Attitude)
{
 QVariantMap form;form.insert("id",params.value("id"));form.insert("attitude","heart");return writeMobileAction("/api/attitudes/create",M_HOST "/",form,r);
}

ID_API_DECL(Unattitude)
{
 QVariantMap form;form.insert("id",params.value("id"));form.insert("attitude","heart");return writeMobileAction("/api/attitudes/destroy",M_HOST "/",form,r);
}

ID_API_DECL(SendCmt)
{
 QVariantMap form;form.insert("mid",params.value("mid"));form.insert("content",params.value("content"));if(params.contains("dualPost"))form.insert("dualPost",params.value("dualPost"));return writeMobileAction("/api/comments/create",QString(M_HOST "/detail/")+params.value("mid").toString(),form,r);
}

ID_API_DECL(Reply)
{
 QVariantMap form=params;form.insert("id",params.value("mid"));form.insert("reply",params.value("cid"));return writeMobileAction("/api/comments/reply",QString(M_HOST "/detail/")+params.value("mid").toString(),form,r);
}

ID_API_DECL(Repost)
{
 QVariantMap form=params;form.insert("id",params.value("mid"));return writeMobileAction("/api/statuses/repost",QString(M_HOST "/compose/repost?id=")+params.value("mid").toString(),form,r);
}

ID_API_DECL(StatusDetail)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;
	int index;
	int end;
	const char Start[] = "var $render_data = [";
	const char End[] = "][0]";

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	ret = connector.SyncRequest(QString(M_STATUSDETAIL_URL).arg(params["mid"].toString()), querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("status_detail")

		index = data.indexOf(Start);
	ok = false;
	if(index != -1)
	{
		index += strlen(Start);
		end = data.indexOf(End, index);
		if(end != -1)
		{
			data = data.mid(index, end - index);
			json = parser.parse(data, &ok);
		}
	}
	ID_JSON_ERR("status_detail")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(LongText)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap header;
	QVariantMap result;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("id", params["mid"].toByteArray()));
	ret = connector.SyncRequest(M_LONGTEXT_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("long_text")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("long_text")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Comment)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	QByteArray mid(params["mid"].toByteArray());

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("id", mid));
	querys.push_back(qMakePair<QByteArray, QByteArray>("mid", mid));
    if(params.contains("count"))querys.push_back(qMakePair(QByteArray("count"),params.value("count").toByteArray()));
	if(params.contains("max_id"))
		querys.push_back(qMakePair<QByteArray, QByteArray>("max_id", params["max_id"].toByteArray()));
	querys.push_back(qMakePair<QByteArray, QByteArray>("max_id_type", params["max_id_type"].toByteArray()));
 if(params.value("latest").toBool())querys.push_back(qMakePair(QByteArray("page"),params.value("page").toByteArray()));
 querys.push_back(qMakePair(QByteArray("_"),QByteArray::number(QDateTime::currentMSecsSinceEpoch())));
 header.insert("Cache-Control","no-cache");header.insert("Referer",M_HOST "/");
	ret = connector.SyncRequest(params.value("latest").toBool()?QString(M_HOST "/api/comments/show"):QString(M_COMMENT_URL), querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("comment")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("comment")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Unread)
{
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	querys.push_back(qMakePair<QByteArray, QByteArray>("t", params.contains("t") ? params["t"].toByteArray() : QByteArray::number(QDateTime::currentMSecsSinceEpoch())));
	ret = idNetworkConnector::SyncRequest_thread(M_UNREAD_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("unread")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("unread")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Contact)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("page", params["page"].toByteArray()));

	ret = connector.SyncRequest(M_CONTACT_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("contact")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("contact")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(SearchContact)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	querys.push_back(qMakePair<QByteArray, QByteArray>("keyword", params["keyword"].toByteArray()));
	querys.push_back(qMakePair<QByteArray, QByteArray>("page", params["page"].toByteArray()));

	ret = connector.SyncRequest(M_SEARCHCONTACT_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("search_contact")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("search_contact")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(DelStatus)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QVariantMap formData;
	QList<QPair<QByteArray, QByteArray> > querys;
	QByteArray mid(params["mid"].toByteArray());
	QVariant xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN"));

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	formData.insert("mid", mid);
	formData.insert("st", xsrf_token);
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/detail/" + mid);
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_DELSTATUS_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("del_status")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("del_status")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(DelComment)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QVariantMap formData;
	QList<QPair<QByteArray, QByteArray> > querys;
	QByteArray mid(params["mid"].toByteArray());
	QByteArray cid(params["cid"].toByteArray());
	QVariant xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN"));

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	formData.insert("cid", cid);
	formData.insert("st", xsrf_token);
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/detail/" + mid + "?cid=" + cid);
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness

	ret = connector.SyncRequest(M_DELCOMMENT_URL, querys, idNetworkConnector::MakePostData(formData), idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("del_comment")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("del_comment")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(UploadPic)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QByteArray formData;
	QList<QPair<QByteArray, QByteArray> > querys;
	QByteArray xsrf_token(idUtility::Instance()->GetCookie(M_HOST, "XSRF-TOKEN").toByteArray());
	const QByteArray boundaryValue(QCryptographicHash::hash(QByteArray().append(QByteArray::number(qulonglong((QDateTime::currentMSecsSinceEpoch() ^ qrand()) ^ 2014)).toBase64()), QCryptographicHash::Md5).toHex().mid(8, 16));
	const QByteArray boundary("----WebKitFormBoundary" + boundaryValue);

	ret = -1;

	connector.SetEngine(qml_viewer->engine());
	header.insert("x-xsrf-token", xsrf_token);
	header.insert("referer", M_HOST "/compose/");
	header.insert("x-requested-with", "XMLHttpRequest"); // not ness
	header.insert("origin", M_HOST); // not ness
	header.insert("Content-Type", "multipart/form-data; boundary=" + boundary);

	formData
		.append("--")
		.append(boundary)
		.append("\r\n")
		.append("Content-Disposition: form-data; name=\"type\"")
		.append("\r\n")
		.append("\r\n")
		.append(params["type"].toByteArray())
		.append("\r\n")

		.append("--")
		.append(boundary)
		.append("\r\n")
		.append("Content-Disposition: form-data; name=\"pic\"; filename=\"" + params["filename"].toByteArray() + "\"")
		.append("\r\n")
		.append("Content-Type: " + params["filetype"].toByteArray())
		.append("\r\n")
		.append("\r\n")
		.append(params["filedata"].toByteArray())
		.append("\r\n")

		.append("--")
		.append(boundary)
		.append("\r\n")
		.append("Content-Disposition: form-data; name=\"st\"")
		.append("\r\n")
		.append("\r\n")
		.append(xsrf_token)
		.append("\r\n")

		.append("--")
		.append(boundary)
		.append("--")
		;

	//qDebug() << formData;
	ret = connector.SyncRequest(M_UPLOADPIC_URL, querys, formData, idNetworkConnector::Connect_Post, header, &data);
	ID_REQ_ERR("upload_pic")
		json = parser.parse(data, &ok);
	ID_JSON_ERR("upload_pic")

		result = json.toMap();

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Logout)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;
	Q_UNUSED(params)

	ret = -1;

	connector.SetEngine(qml_viewer->engine());

	ret = connector.SyncRequest(M_LOGOUT_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data);
	ID_REQ_ERR("logout")

__Error:
	if(r)
		*r = ret;
	return result;
}

ID_API_DECL(Login)
{
 int ret=0; QVariantMap result=loginV2(qml_viewer->engine(),params,&ret); if(r)*r=ret; return result;
}

ID_API_DECL(UserId)
{
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QVariant json;
	QVariantMap result;
	QVariantMap header;
	QList<QPair<QByteArray, QByteArray> > querys;

	ret = -1;

	connector.SetEngine(qml_viewer->engine());

	ret = connector.SyncRequest(M_USER_URL + params["name"].toString(), querys, QByteArray(), idNetworkConnector::Connect_Get, header, &data, false);
	ID_REQ_ERR("user_id")
		result.insert("url", data);

__Error:
	if(r)
		*r = ret;
	return result;
}
