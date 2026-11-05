#ifndef WEIBOMM_PROFILEDATA_H
#define WEIBOMM_PROFILEDATA_H
#include <QVariantMap>
inline QVariantMap normalizeUserProfile(const QVariantMap &root,const QString &uid){
 QVariantMap fail;fail.insert("ok",0);if(!root.value("ok").toInt())return fail;
 QVariantMap data=root.value("data").toMap(),user=data.value("userInfo").toMap();if(user.isEmpty())user=data.value("user").toMap();
 QString actual=user.value("idstr").toString();if(actual.isEmpty())actual=user.value("id").toString();if(user.isEmpty()||actual.isEmpty()||(!uid.isEmpty()&&actual!=uid))return fail;
 if(!user.contains("follow_count")&&user.contains("friends_count"))user.insert("follow_count",user.value("friends_count"));
 if(user.value("profile_image_url").toString().isEmpty())user.insert("profile_image_url",user.value("avatar_large",user.value("avatar_hd")));
 data.insert("user",user);QVariantMap result;result.insert("ok",1);result.insert("data",data);return result;
}
#endif
