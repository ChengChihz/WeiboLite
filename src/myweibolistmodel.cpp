#include "myweibolistmodel.h"

#include <QDebug>

#include "id_std.h"
#include "api.h"
#include "accountmodel.h"

	id_MyWeiboListModel::id_MyWeiboListModel(QObject *parent)
	: id_accListModel_base(parent)
{
	setObjectName("id_MyWeiboListModel");
}

id_MyWeiboListModel::~id_MyWeiboListModel()
{
	ID_QOBJECT_DESTROY_DBG
}

void id_MyWeiboListModel::deleteItemByStatusId(const QString &statusID)
{
	int i;
	int len;
	int ret;
	QVariantMap params;
	QVariantMap result;

	params.insert("mid", statusID);
	idAPI::DelStatus(params, &ret);
	if(ret != 0 && result["ok"].toInt() == 0)
	{
		qDebug() << result["msg"].toString();
	}

	len = m_list.size();
	for(i = 0; i < len; i++)
	{
		if(GetStatusId(i) == statusID)
		{
			Remove(i);
			return;
		}
	}
}

void id_MyWeiboListModel::setUser(const QString &userId, const QString &nickName)
{
 if(m_userId!=userId){removeAllItemList();m_sinceId.clear();SetPn(0);SetHasNext(true);}
 m_userId=userId;m_nickName=nickName;m_containerId="107603"+userId;
 if(userId.isEmpty()){m_containerId.clear();return;}
 QVariantMap params;params.insert("type","uid");params.insert("value",userId);int ret=0;QVariantMap result=idAPI::Index(params,&ret);
 if(ret==0 && result.value("ok").toInt()){foreach(const QVariant &v,result.value("data").toMap().value("tabsInfo").toMap().value("tabs").toList()){QVariantMap tab=v.toMap();if(tab.value("tab_type").toString()=="weibo"||tab.value("tabKey").toString()=="weibo"||tab.value("title").toString()==QString::fromUtf8("微博")){QString value=tab.value("containerid").toString();if(!value.isEmpty())m_containerId=value;}}}
}

void id_MyWeiboListModel::setPageNum(int pn)
{
	SetPn(pn);
}

void id_MyWeiboListModel::createDBTable()
{
}

void id_MyWeiboListModel::getMyWeiBoFromModel(int type)
{
	if(m_containerId.isEmpty())
		return;
	MyWeibo(type);
	return;

	if(id_accountModel::Instance()->CurrentUserId() == m_userId)
		MyWeibo(type);
	else
		UserWeibo(type);
}

QString id_MyWeiboListModel::getMyWeiBoStudusId(int index) const
{
	return GetStatusId(index);
}

void id_MyWeiboListModel::clearMyWeiBoModel()
{
	removeAllItemList();
}

void id_MyWeiboListModel::MyWeibo(int type)
{
	int ret;
	QVariantMap result;
	QVariantMap params;
	QVariantList list;
	int pn;
	QVariantMap data;
	QVariantMap page;
	int c;

	c = 0;
	if(type == idAPI::Type_Next)
	{
		if(!m_hasMore)
			return;
		pn = m_pn + 1;
	}
	else
	{
		
		pn = 1;

		SetHasNext(true);
	}
	if(m_connected)
		emit getUserTimelineItemResult(idAPI::ErrCode_Loading);
	params.insert("page", pn);
    if(type==idAPI::Type_Next&&!m_sinceId.isEmpty())params.insert("since_id",m_sinceId);
	params.insert("containerid", m_containerId);
	params.insert("openApp", 0);
	result = idAPI::Index(params, &ret);
	if(ret != 0)
	{
		goto __Error;
	}
	if(result["ok"].toInt())
	{
		if(type!=idAPI::Type_Next)removeAllItemList();
		data = result["data"].toMap();
		list = data["cards"].toList();
		page = data["cardlistInfo"].toMap();
        SetPn(pn);
        m_sinceId=page.value("since_id").toString();
        QVariantList expanded;foreach(const QVariant &v,list){QVariantMap card=v.toMap();if(card.value("card_type").toInt()==9)expanded<<card;else foreach(const QVariant &nested,card.value("card_group").toList())if(nested.toMap().value("card_type").toInt()==9)expanded<<nested;}
        list=expanded;
        SetHasNext(!list.isEmpty());
		if(list.isEmpty())
		{
			qDebug() << result["msg"].toString();
			goto __Success;
		}

		ID_CONST_FOREACH(QVariantList, list)
		{
			QVariantMap map;
			QVariantMap item = itor->toMap();
			if(id_accListModel_base::MakeModelData(map, item))
			{
                bool duplicate=false;for(int n=0;n<m_list.size();n++)if(GetStatusId(n)==map.value("statusId").toString())duplicate=true;if(duplicate)continue;
				Push_back(map);
				c++;
			}
		}
        SetHasNext(c>0);
		SetReflashCount(c);
		SetReflashTime();
	}
	else
	{
		qDebug() << result["msg"].toString();
		goto __Error;
	}

__Success:
	emit getUserTimelineItemResult(idAPI::ErrCode_Success);
	return;

__Error:
	emit getUserTimelineItemResult(idAPI::ErrCode_Error);
	return;
}

void id_MyWeiboListModel::UserWeibo(int type)
{
	int ret;
	QVariantMap result;
	QVariantMap params;
	QVariantList list;
	QString pn;
	QVariantMap data;
	QVariantMap page;
	int c;

	c = 0;
	if(type == idAPI::Type_Next)
	{
		if(!m_hasMore)
			return;
		pn = m_sinceId;
	}
	else
	{
		removeAllItemList();
		pn.clear();
		SetHasNext(true);
	}
	emit getUserTimelineItemResult(idAPI::ErrCode_Loading);
	params.insert("type", "uid");
	params.insert("uid", m_userId);
	params.insert("value", m_userId);
	params.insert("containerid", m_containerId);
	params.insert("openApp", 0);
	if(!pn.isEmpty())
		params.insert("since_id", pn);
	result = idAPI::Index(params, &ret);
	if(ret != 0)
	{
		goto __Error;
	}
	if(result["ok"].toInt())
	{
		data = result["data"].toMap();
		list = data["cards"].toList();
		page = data["cardlistInfo"].toMap();
		if(page["since_id"].isNull())
			SetHasNext(false);
		else
			SetSinceId(page["since_id"].toString());
		if(list.isEmpty())
		{
			qDebug() << result["msg"].toString();
			goto __Success;
		}

		ID_CONST_FOREACH(QVariantList, list)
		{
			QVariantMap map;
			QVariantMap item = itor->toMap();
			if(id_accListModel_base::MakeModelData(map, item))
			{
				Push_back(map);
				c++;
			}
		}
		SetReflashCount(c);
		SetReflashTime();
	}
	else
	{
		qDebug() << result["msg"].toString();
		goto __Error;
	}

__Success:
	emit getUserTimelineItemResult(idAPI::ErrCode_Success);
	return;

__Error:
	emit getUserTimelineItemResult(idAPI::ErrCode_Error);
	return;
}

void id_MyWeiboListModel::SetSinceId(const QString &id)
{
	if(m_sinceId != id)
		m_sinceId = id;
}
