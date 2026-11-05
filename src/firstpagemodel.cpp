#include "firstpagemodel.h"

#include <QDebug>
#include <QSet>

#include "api.h"

	id_firstPageModel::id_firstPageModel(QObject *parent)
	: id_accListModel_base(parent), m_recommended(false)
{
	setObjectName("id_firstPageModel");
}

id_firstPageModel::~id_firstPageModel()
{
	ID_QOBJECT_DESTROY_DBG
}

void id_firstPageModel::setRecommended(bool value)
{
 if(m_recommended==value)return; m_recommended=value;removeAllItemList();m_feedCursor.clear();SetHasNext(true);emit feedModeChanged();getSinaContentFromModel(0);
}

void id_firstPageModel::getSinaContentFromModel(int type)
{
	int ret;
	QVariantMap result;
	QVariantMap params;
	QVariantList list;

	QVariantMap data;
	QVariantMap page;
	int c;
    bool next=type==idAPI::Type_Next;
    bool hadRows=!m_list.isEmpty();
    QVariantList additions;

	c = 0;
	if(type == idAPI::Type_Next)
	{
		if(!m_hasMore)
			return;

	}
	emit getFriendsTimeLineResult(idAPI::ErrCode_Loading);
	if(next&&!m_feedCursor.isEmpty())params.insert(m_recommended?"since_id":"max_id",m_feedCursor);
    params.insert("feed_kind", m_recommended?"recommended":"following");
	params.insert("containerid", "102803");
	params.insert("openApp", 0);
	result = idAPI::Index(params, &ret);
	if(ret != 0)
	{
		goto __Error;
	}
	if(result["ok"].toInt())
	{
		data = result["data"].toMap();
		list = data["cards"].toList();
        if(!m_recommended){list.clear();foreach(const QVariant &status,data.value("statuses").toList()){QVariantMap card;card.insert("card_type",9);card.insert("mblog",status);list<<card;}}
		page = data["cardlistInfo"].toMap();
        QString nextCursor=(m_recommended?page.value("since_id"):data.value("max_id")).toString();
        if(next||!hadRows){SetHasNext(!nextCursor.isEmpty()&&nextCursor!="0"&&nextCursor!=m_feedCursor);m_feedCursor=nextCursor;}
		if(list.isEmpty())
		{
			qDebug() << result["msg"].toString();
			goto __Success;
		}

        QSet<QString> seen;for(int i=0;i<m_list.size();i++)seen.insert(GetStatusId(i));
		ID_CONST_FOREACH(QVariantList, list)
		{
			QVariantMap map;
			QVariantMap item = itor->toMap();
			if(id_accListModel_base::MakeModelData(map, item))
			{
                QString id=map.value("statusId").toString();if(id.isEmpty()||seen.contains(id))continue;seen.insert(id);
                if(next)Push_back(map);else additions<<map;
				c++;
			}
		}
        if(!next&&!additions.isEmpty()){beginInsertRows(QModelIndex(),0,additions.size()-1);for(int i=additions.size()-1;i>=0;i--)m_list.prepend(additions[i].toMap());endInsertRows();}
		SetReflashCount(c);
		SetReflashTime();
	}
	else
	{
		qDebug() << result["msg"].toString();
		goto __Error;
	}

__Success:
	emit getFriendsTimeLineResult(idAPI::ErrCode_Success);
	return;

__Error:
	emit getFriendsTimeLineResult(idAPI::ErrCode_Error);
	return;
}

QString id_firstPageModel::getSinaStudusId(int index) const
{
	return GetStatusId(index);
}

void id_firstPageModel::deleteItemByStatusId(const QString &statusId)
{
	int i;
	int len;

	len = m_list.size();
	for(i = 0; i < len; i++)
	{
		if(GetStatusId(i) == statusId)
		{
			Remove(i);
			return;
		}
	}
}

ID_SINGLE_INSTANCE_DECL(id_firstPageModel)
