#include "emotionlist.h"
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QSet>
#include "qjson/parser.h"
#define ID_EMOTION_DIR "images/emotion_v2"
id_emotionlist::id_emotionlist(QObject *parent):idQmlModel_base(parent),m_path(QDir::cleanPath(id::adjust_path("qml/" ID_QML_DIR "/" ID_EMOTION_DIR))){setObjectName("id_emotionlist");Init();}
id_emotionlist::~id_emotionlist(){}
QString id_emotionlist::getPath() const{return m_path;}
int id_emotionlist::getPageCount(int ps) const{return ps>0?(rowCount()+ps-1)/ps:0;}
QString id_emotionlist::getIconPath(int pn,int index,int ps) const{if(pn<0||ps<=0||index<0||index>=ps||pn>rowCount()/ps)return QString();int i=pn*ps+index;if(i>=rowCount())return QString();return QUrl::fromLocalFile(m_path+"/"+GetValue(i,"file_name").toString()).toString();}
QString id_emotionlist::getEmotionCode(int pn,int index,int ps) const{if(pn<0||ps<=0||index<0||index>=ps||pn>rowCount()/ps)return QString();int i=pn*ps+index;if(i>=rowCount())return QString();return GetValue(i,"code").toString();}
void id_emotionlist::Init(){removeAllItemList();QFile file(m_path+"/manifest.json");if(!file.open(QIODevice::ReadOnly))return;QJson::Parser parser;bool ok=false;QVariantList entries=parser.parse(file.readAll(),&ok).toList();if(!ok)return;QSet<QString> seen;foreach(const QVariant &v,entries){QVariantMap e=v.toMap();QString name=e.value("file").toString(),code=e.value("code").toString();if(name.contains('/')||name.contains('\\')||!name.endsWith(".png")||!code.startsWith('[')||!code.endsWith(']')||seen.contains(code)||!QFile::exists(m_path+"/"+name))continue;seen.insert(code);QVariantMap row;row.insert("file_name",name);row.insert("code",code);Push_back(row);}SetReflashCount(rowCount());SetReflashTime();}
ID_SINGLE_INSTANCE_DECL(id_emotionlist)
