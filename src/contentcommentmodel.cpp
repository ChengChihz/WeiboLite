#include "contentcommentmodel.h"

#include <QDebug>

#include "api.h"
#include "helper.h"
#include "diagnostic.h"

	id_contentCommentModel::id_contentCommentModel(QObject *parent)
	: id_accListModel_base(parent)
{
	m_previewLimit=0;m_latest=true;setObjectName("id_contentCommentModel");
}

id_contentCommentModel::~id_contentCommentModel()
{
	ID_QOBJECT_DESTROY_DBG
}

void id_contentCommentModel::getContentCommentFromModel(const QString &statusId,int type){
 bool next=type==idAPI::Type_Next;if(next&&!m_hasMore)return;emit getContentCommentResult(idAPI::ErrCode_Loading);QVariantMap params;params.insert("mid",statusId);params.insert("latest",m_latest);if(m_previewLimit>0)params.insert("count",m_previewLimit);params.insert("page",next?m_pn+1:1);params.insert("max_id_type",0);if(next&&!m_maxId.isEmpty())params.insert("max_id",m_maxId);int rc=0;QVariantMap root=idAPI::Comment(params,&rc),data=root.value("data").toMap();
 if(rc||!root.value("ok").toInt()){QString message=root.value("msg").toString();if(!rc&&(message.contains(QString::fromUtf8("暂无数据"))||message.contains(QString::fromUtf8("还没有人评论")))){if(!next)removeAllItemList();SetHasNext(false);emit getContentCommentResult(idAPI::ErrCode_Success);return;}emit getContentCommentResult(idAPI::ErrCode_Error);return;}
 QVariantList list=data.value("data").toList();if(!next){removeAllItemList();SetMaxId();SetPn(1);}else SetPn(m_pn+1);int count=0;foreach(const QVariant &v,list){if(m_previewLimit>0&&count>=m_previewLimit)break;QVariantMap map;if(!MakeModelData(map,v.toMap()))continue;bool duplicate=false;for(int row=0;row<m_list.size();row++)if(GetStatusId(row)==map.value("statusId").toString())duplicate=true;if(!duplicate){Push_back(map);count++;}}
 SetMaxId(data.value("max_id").toString());int total=data.value("total_number").toInt();SetHasNext(m_latest?(!list.isEmpty()&&(total>0?m_list.size()<total:list.size()>=10)):!m_maxId.isEmpty());SetReflashCount(count);SetReflashTime();diagnostic(QString("comments_read latest=%1 rows=%2 total=%3").arg(m_latest?1:0).arg(m_list.size()).arg(total));emit getContentCommentResult(idAPI::ErrCode_Success);
}

QVariantList id_contentCommentModel::previewItems(int limit)const{QVariantList result;limit=qBound(0,limit,15);for(int i=0;i<qMin(limit,m_list.size());i++)result.append(m_list.at(i));return result;}

void id_contentCommentModel::SetMaxId(const QString &id)
{
	if(m_maxId != id)
		m_maxId = id;
	if(m_maxId == "0")
		m_maxId.clear();
}

bool id_contentCommentModel::MakeModelData(QVariantMap &map, const QVariantMap &item)
{
	QString pic;
	QVariantMap user = item["user"].toMap();

	map.clear();
	map.insert("isVip", user["verified"].toBool());
	map.insert("userNickName", user["screen_name"].toString());
	map.insert("untilTime", item["created_at"].toString());
	map.insert("hasPic", item["pic_num"].toInt());
	map.insert("hasMap", item["weibo_position"].toInt());
	map.insert("weibocontent", idHelper::FixedContentEmotion(item["text"].toString()));
	map.insert("image", user["profile_image_url"].toString());
	map.insert("fromPlat", item["source"].toString());
	map.insert("commentCount", item["commentsCount"].toInt());
	map.insert("forwardCount", item["repostsCount"].toInt());

	map.insert("forwardcontent", "");
	map.insert("forwardThumbnailPic", "");
	map.insert("imageAddress_forward", "");

	pic = item["thumbnail_pic"].toString();
	map.insert("statusThumbnailPic", pic);
	pic = item["original_pic"].toString();
	map.insert("imageAddress_status", pic);

	map.insert("userId", user["id"].toString());
	map.insert("statusId", item["id"].toString());
	return true;
}

ID_SINGLE_INSTANCE_DECL(id_contentCommentModel)
