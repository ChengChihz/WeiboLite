#include "hottrendmodel.h"

#include <QDebug>
#include <QStringList>

#include "api.h"

	id_hotTrendModel::id_hotTrendModel(QObject *parent)
	: idPagedQmlModel_base(parent)
{
	setObjectName("id_hotTrendModel");
	InitRoleMap();
}

id_hotTrendModel::~id_hotTrendModel()
{
	ID_QOBJECT_DESTROY_DBG
}

QString id_hotTrendModel::getTrendName(int index) const
{
	QString name = GetValue(index, "trendName").toString();

	if(1)
	{
		if(name.startsWith('#'))
			name.remove(0, 1);
		if(name.endsWith('#'))
			name.remove(name.length() - 1, 1);
	}

	return name;
}

void id_hotTrendModel::selectTopicItemFinished()
{
	emit selectTopicItem();
}

void id_hotTrendModel::getSinaHotTrendFromModel(int type)
{
 if(type==idAPI::Type_Next)return;removeAllItemList();SetHasNext(false);emit getHotTrendResult(idAPI::ErrCode_Loading);
 QVariantMap params;params.insert("feed_kind","hotsearch");int ret=0;QVariantMap root=idAPI::Index(params,&ret);int count=0;QStringList seen;
 if(ret==0 && root.value("ok").toInt()==1){foreach(const QVariant &v,root.value("data").toMap().value("realtime").toList()){QVariantMap word=v.toMap();QString name=word.value("word",word.value("note")).toString();if(name.isEmpty()||word.value("is_ad").toBool()||seen.contains(name))continue;seen<<name;QVariantMap row;row.insert("trendName",name);Push_back(row);count++;}}
 SetReflashCount(count);if(count>0)SetReflashTime();emit getHotTrendResult(count>0?idAPI::ErrCode_Success:idAPI::ErrCode_Error);
}

void id_hotTrendModel::InitRoleMap()
{
	m_roles.clear();
	ID_QMLMODEL_INSERT_ROLE(id_hotTrendModel, trendName);
	setRoleNames(m_roles);
}

ID_SINGLE_INSTANCE_DECL(id_hotTrendModel)
