#include "searchblogmodel.h"

#include <QDebug>
#include <QUrl>

#include "api.h"

	id_searchBlogModel::id_searchBlogModel(QObject *parent)
	: id_accListModel_base(parent)
{
	setObjectName("id_searchBlogModel");
}

id_searchBlogModel::~id_searchBlogModel()
{
	ID_QOBJECT_DESTROY_DBG
}

void id_searchBlogModel::searchBlogFromModel(int type, const QString &keyword)
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
		removeAllItemList();
		pn = 1;
		SetHasNext(true);
	}
	emit searchBlogResult(idAPI::ErrCode_Loading);
	params.insert("page", pn);
	params.insert("containerid", QUrl::toPercentEncoding("100103type=1&q=" + keyword));
	params.insert("openApp", 0);
	params.insert("page_type", "searchall");
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
        SetPn(pn);
        SetHasNext(!list.isEmpty());
		if(list.isEmpty())
		{
			qDebug() << result["msg"].toString();
			goto __Success;
		}

        QVariantList pending=list;while(!pending.isEmpty()){QVariantMap item=pending.takeFirst().toMap();pending+=item.value("card_group").toList();QVariantMap map;if(!id_accListModel_base::MakeModelData(map,item))continue;bool duplicate=false;for(int n=0;n<m_list.size();n++)if(GetStatusId(n)==map.value("statusId").toString())duplicate=true;if(!duplicate){Push_back(map);c++;}}
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
	emit searchBlogResult(idAPI::ErrCode_Success);
	return;

__Error:
	emit searchBlogResult(idAPI::ErrCode_Error);
	return;
}

QString id_searchBlogModel::getStudusId(int index) const
{
	return GetStatusId(index);
}

ID_SINGLE_INSTANCE_DECL(id_searchBlogModel)
