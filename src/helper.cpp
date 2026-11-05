#include "helper.h"

#include <QRegExp>
#include <QGraphicsObject>

#include "api.h"
#include "api_defines.h"
#include "utility.h"
#include "qmlapplicationviewer.h"

extern QmlApplicationViewer *qml_viewer;

QString idHelper::FixedContentEmotion(const QString &src)
{
	// <span class=\"url-icon\"><img alt=[吃惊] src=\"//h5.sinaimg.cn/m/emoticon/icon/default/d_chijing-e806473437.png\" style=\"width:1em; height:1em;\" /></span>
	QRegExp p("<img alt=\\[(\\S*)\\] src=\"//");
 QString result=QString(src).replace(p, "<img alt=[_\\1_] src=\"" M_SCHEME "//");
 QRegExp images("<img\\b[^>]*>",Qt::CaseInsensitive);int pos=0;
 while((pos=images.indexIn(result,pos))>=0){
  QString tag=images.cap(0);int original=tag.size();
  QRegExp inlineSize("(width|height)\\s*:\\s*1(\\.0)?em",Qt::CaseInsensitive);
  QRegExp faceClass("class\\s*=\\s*[\"'][^\"']*(face|url-icon)",Qt::CaseInsensitive);
  bool emotion=tag.contains("/emoticon/",Qt::CaseInsensitive)||tag.contains("/expression/",Qt::CaseInsensitive)||tag.contains("class=\"face",Qt::CaseInsensitive);
  QString preceding=result.mid(qMax(0,pos-180),qMin(pos,180));
  bool badge=tag.contains("timeline_card",Qt::CaseInsensitive)||tag.contains("/icon/",Qt::CaseInsensitive)||tag.contains("url-icon",Qt::CaseInsensitive)||(preceding.lastIndexOf("url-icon")>preceding.lastIndexOf("</span>"));
  if(emotion||badge||inlineSize.indexIn(tag)>=0||faceClass.indexIn(tag)>=0){
   QRegExp protocol("src\\s*=\\s*([\"'])//",Qt::CaseInsensitive);tag.replace(protocol,"src=\\1https://");
   QRegExp dimensions("\\s+(width|height)\\s*=\\s*(\"[^\"]*\"|'[^']*'|[^\\s>]+)",Qt::CaseInsensitive);tag.remove(dimensions);
   QRegExp cssDimensions("(width|height)\\s*:\\s*[^;\"']+;?",Qt::CaseInsensitive);tag.remove(cssDimensions);
   int close=tag.endsWith("/>")?tag.size()-2:tag.size()-1;tag.insert(close,emotion?" width=\"32\" height=\"32\"":" width=\"22\" height=\"22\"");
   result.replace(pos,original,tag);
  }
  pos+=tag.size();
 }
 return result;
}

void idHelper::ShowBanner(const QString &text)
{
	QObject *app;

	if(!qml_viewer)
		return;

	app = qml_viewer->rootObject();
	idUtility::Instance()->EvaluteScriptv("showBanner('" + text + "');", app);
}

