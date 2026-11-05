#include <QtGui/QApplication>
#include <QtDeclarative>
#include <QLocale>
#include <QTranslator>
#include <QTextCodec>
#include <QDebug>
#include <QCoreApplication>
#include <cstdio>
#include "diagnostic.h"
#include "authv2.h"
#include "pendinglogin.h"
#include "browsersession.h"
#include "profiledata.h"
#include "helper.h"
#include "api.h"
#include "writeprobe.h"
#include <QPainter>
#include <QImage>
#include <QX11Info>
#include <QLibrary>
#include <QWebPage>
#include <QWebFrame>
#include <QFile>
#include <QDataStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "qjson/serializer.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "qtm/qdeclarativewebview.h"
class VerificationProbePage : public QWebPage {
 protected: void javaScriptConsoleMessage(const QString &text,int line,const QString &) {
 diagnostic(QString("verification_js line=%1 message=%2").arg(line).arg(text.left(160)));
 }
};
static bool probeOfflineTouch(QDeclarativeItem *root,QDeclarativeWebView *view){
 if(!root || !view)return false;
 QGraphicsScene scene;scene.setSceneRect(0,0,480,720);scene.addItem(root);
 GraphicsWebView *graphics=0;foreach(QGraphicsItem *child,view->childItems()){graphics=dynamic_cast<GraphicsWebView*>(child);if(graphics)break;}
 if(!graphics){scene.removeItem(root);return false;}
 view->page()->mainFrame()->setHtml("<html><body style='margin:0;width:100%;height:100%'><script>window.lastX=-1;window.lastY=-1;window.moves=0;window.clicks=0;window.doubles=0;document.addEventListener('dblclick',function(e){window.doubles++;},false);document.addEventListener('mousemove',function(e){window.moves++;},false);document.addEventListener('click',function(e){window.lastX=e.clientX;window.lastY=e.clientY;window.clicks++;},false);</script></body></html>");
 QCoreApplication::processEvents();view->ConfigureVerificationViewport();
 const QPointF points[]={QPointF(60,60),QPointF(240,160),QPointF(400,300)};int error=0;
 for(int i=0;i<3;++i){
 QPointF p=graphics->mapFromScene(view->mapToScene(points[i]));QPointF initial=p;
 QGraphicsSceneMouseEvent down(QEvent::GraphicsSceneMousePress);down.setPos(p);down.setScenePos(graphics->mapToScene(p));down.setScreenPos(graphics->mapToScene(p).toPoint());down.setButton(Qt::LeftButton);down.setButtons(Qt::LeftButton);scene.sendEvent(graphics,&down);
 p=graphics->mapFromScene(view->mapToScene(points[i]+QPointF(2,3)));QGraphicsSceneMouseEvent move(QEvent::GraphicsSceneMouseMove);move.setPos(p);move.setScenePos(graphics->mapToScene(p));move.setScreenPos(graphics->mapToScene(p).toPoint());move.setLastPos(initial);move.setLastScenePos(graphics->mapToScene(initial));move.setButtons(Qt::LeftButton);scene.sendEvent(graphics,&move);
 QGraphicsSceneMouseEvent up(QEvent::GraphicsSceneMouseRelease);up.setPos(p);up.setScenePos(graphics->mapToScene(p));up.setScreenPos(graphics->mapToScene(p).toPoint());up.setButton(Qt::LeftButton);up.setButtons(Qt::NoButton);scene.sendEvent(graphics,&up);QCoreApplication::processEvents();
 int x=view->page()->mainFrame()->evaluateJavaScript("window.lastX").toInt(),y=view->page()->mainFrame()->evaluateJavaScript("window.lastY").toInt();error=qMax(error,qAbs(x-qRound(p.x()))+qAbs(y-qRound(p.y())));
 }
 QPointF last=graphics->mapFromScene(view->mapToScene(points[2]+QPointF(2,3)));
 QGraphicsSceneMouseEvent dbl(QEvent::GraphicsSceneMouseDoubleClick);dbl.setPos(last);dbl.setScenePos(graphics->mapToScene(last));dbl.setScreenPos(graphics->mapToScene(last).toPoint());dbl.setButton(Qt::LeftButton);dbl.setButtons(Qt::LeftButton);scene.sendEvent(graphics,&dbl);
 QGraphicsSceneMouseEvent finalUp(QEvent::GraphicsSceneMouseRelease);finalUp.setPos(last);finalUp.setScenePos(graphics->mapToScene(last));finalUp.setScreenPos(graphics->mapToScene(last).toPoint());finalUp.setButton(Qt::LeftButton);finalUp.setButtons(Qt::NoButton);scene.sendEvent(graphics,&finalUp);QCoreApplication::processEvents();
 int doubles=view->page()->mainFrame()->evaluateJavaScript("window.doubles").toInt();
 diagnostic(QString("offline_double_click_probe count=%1 zoom=%2").arg(doubles).arg(view->contentsScale()));
 int moves=view->page()->mainFrame()->evaluateJavaScript("window.moves").toInt(),clicks=view->page()->mainFrame()->evaluateJavaScript("window.clicks").toInt();
 diagnostic(QString("offline_touch_probe error=%1 moves=%2 clicks=%3 zoom=%4").arg(error).arg(moves).arg(clicks).arg(view->contentsScale()));scene.removeItem(root);return error<=2 && moves>=3 && clicks>=3 && doubles==1;
}
static void discardLegacyLog(QtMsgType, const char *) {}
static void offlineFixtureLog(QtMsgType,const char *message){fprintf(stdout,"offline_fixture: %s\n",message);fflush(stdout);}
#include "qmlapplicationviewer.h"

#include "id_std.h"
#include "utility.h"
#include "pipeline.h"
#include "networkconnector.h"
#include "networkmanager.h"
#include "qt_funcview.h"
#include "settingmodel.h"
#include "newmsgnotification.h"
#include "accountmodel.h"
#include "loginmodel.h"
#include "commenttomemodel.h"
#include "messagecentermodel.h"
#include "atmemodel.h"
#include "profile.h"
#include "microblog.h"
#include "inserttopicmodel.h"
#include "hottrendmodel.h"
#include "trendblogmodel.h"
#include "contentcommentmodel.h"
#include "weiboprofilemodel.h"
#include "database.h"
#include "firstpagemodel.h"
#include "searchblogmodel.h"
#include "searchusermodel.h"
#include "searchcontactmodel.h"
#include "searchtopicmodel.h"
#include "suibiankanmodel.h"
#include "favoritesmodel.h"
#include "fanlistmodel.h"
#include "friendlistmodel.h"
#include "trendtopicmodel.h"
#include "myweibolistmodel.h"
#include "updateprofilemodel.h"
#include "emotionlist.h"
#include "detailmodel.h"
#include "deletemodel.h"
#include "frequentcontactmodel.h"
#include "qtm/qdeclarativewebview.h"
#ifdef _KARIN_MM_EXTENSIONS
#include "qtm/qdeclarativevideo_p.h"
#endif


#define QML_MODULE_NAME "SinaWeiboTypeLib"
#define QML_MODULE_MAJOR 1
#define QML_MODULE_MINOR 0

idPipeline *pipeline = 0;
static idDatabase *activeDatabase = 0;

static idUtility *ut = 0;
static idNetworkConnector *connector = 0;
static id_qt_funcView *qt_funcView = 0;
static id_settingModel *settingModel = 0;
static id_newMsgNotification *newMsgNotification = 0;
static id_accountModel *accountModel = 0;
static id_loginModel *loginModel = 0;
static id_profile *profile = 0;
static id_commentToMeModel *commentToMeModel = 0;
static id_messageCenterModel *messageCenterModel = 0;
static id_atMeModel *atMeModel = 0;
static id_updateProfileModel *updateProfileModel = 0;
static id_emotionlist *emotionlist = 0;
static id_firstPageModel *firstPageModel = 0;
static id_searchBlogModel *searchBlogModel = 0;
static id_searchUserModel *searchUserModel = 0;
static id_searchContactModel *searchContactModel = 0;
static id_searchTopicModel *searchTopicModel = 0;
static id_suiBianKanModel *suiBianKanModel = 0;
static id_microBlog *microBlog = 0;
static id_insertTopicModel *insertTopicModel = 0;
static id_hotTrendModel *hotTrendModel = 0;
static id_contentCommentModel *contentCommentModel = 0;
static id_frequentContactModel *frequentContactModel = 0;
idDeclarativeNetworkAccessManagerFactory factory;

namespace id
{
	class QmlApplicationViewer;
}

QmlApplicationViewer *qml_viewer;
bool lock = false;
bool inited = false;
static QTime launchStageClock;
static bool startupProfileMode=false;
static bool firstWindowPaint=false;
static void startupStage(const QString &stage){QString line=QString("startup stage=%1 elapsed_ms=%2").arg(stage).arg(launchStageClock.elapsed());diagnostic(line);if(startupProfileMode){fprintf(stdout,"%s\n",line.toAscii().constData());fflush(stdout);}}

class NightIconProvider:public QDeclarativeImageProvider {
 QString root;
 public:explicit NightIconProvider(const QString &path):QDeclarativeImageProvider(QDeclarativeImageProvider::Image),root(path){}
 QImage requestImage(const QString &id,QSize *size,const QSize &requested){
  if(id.contains('/')||id.contains('\\')||id.contains("..")||!id.endsWith(".png"))return QImage();
  QImage image(root+"/"+id);if(image.isNull())return image;if(size)*size=image.size();image=image.convertToFormat(QImage::Format_ARGB32);
  for(int y=0;y<image.height();y++){QRgb *row=reinterpret_cast<QRgb*>(image.scanLine(y));for(int x=0;x<image.width();x++){QRgb pixel=row[x];int shade=qMin(240,255-qGray(pixel));row[x]=qRgba(shade,qMin(245,shade+5),qMin(250,shade+10),qAlpha(pixel));}}
  if(requested.width()>0&&requested.height()>0)image=image.scaled(requested,Qt::KeepAspectRatio,Qt::SmoothTransformation);return image;
 }
};

static bool publishNativePortrait(QWidget *window){
 if(!window||!QX11Info::display())return false;
 static QLibrary x11("X11",6);
 typedef unsigned long (*Intern)(void*,const char*,int);
 typedef int (*Change)(void*,unsigned long,unsigned long,unsigned long,int,int,const unsigned char*,int);
 typedef int (*Flush)(void*);
 Intern intern=reinterpret_cast<Intern>(x11.resolve("XInternAtom"));Change change=reinterpret_cast<Change>(x11.resolve("XChangeProperty"));Flush flush=reinterpret_cast<Flush>(x11.resolve("XFlush"));
 if(!intern||!change||!flush)return false;
 unsigned long angle=270;void *display=QX11Info::display();change(display,window->winId(),intern(display,"_MEEGOTOUCH_ORIENTATION_ANGLE",0),6,32,0,reinterpret_cast<const unsigned char*>(&angle),1);flush(display);return true;
}

namespace id
{
	class QmlApplicationViewer : public ::QmlApplicationViewer
	{
    public:
        bool openingFrame;
        QImage openingPortrait,openingLandscape,openingIcon;
        QmlApplicationViewer():openingFrame(true){}
    protected:
        void drawBackground(QPainter *painter,const QRectF &rect){
            if(!openingFrame){::QmlApplicationViewer::drawBackground(painter,rect);return;}
            bool dark=settingModel&&settingModel->darkMode();QRectF area=sceneRect();painter->fillRect(rect,dark?QColor("#101419"):QColor(Qt::white));
            const QImage &image=openingPortrait;
            if(image.isNull())return;
            painter->save();painter->translate(area.center());
            bool rotated=area.width()>area.height();if(rotated)painter->rotate(270);
            QSizeF available=rotated?QSizeF(area.height(),area.width()):area.size();
            QSizeF size=image.size();size.scale(available,Qt::KeepAspectRatio);
            QRectF target(QPointF(-size.width()/2,-size.height()/2),size);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);
            if(dark){painter->drawImage(QRectF(-48,-84,96,96),openingIcon);painter->setPen(QColor("#edf2f7"));QFont font=painter->font();font.setPixelSize(32);painter->setFont(font);painter->drawText(QRectF(-200,36,400,64),Qt::AlignHCenter|Qt::AlignTop,QString::fromUtf8("微博Lite"));}
            else painter->drawImage(target,image);painter->restore();
        }
        void showEvent(QShowEvent *event){::QmlApplicationViewer::showEvent(event);publishNativePortrait(this);}
        void paintEvent(QPaintEvent *event){::QmlApplicationViewer::paintEvent(event);if(!firstWindowPaint){firstWindowPaint=true;startupStage("first_paint");}}
		virtual void closeEvent(QCloseEvent *event)
		{
			if(false)
			{
				hide();
				event->ignore();
			}
			else
				::QmlApplicationViewer::closeEvent(event);
		}
	};

	void create_qml_viewer()
	{
		QDeclarativeEngine *engine;
		QDeclarativeContext *context;

		if(!inited)
			return;
		if(lock)
			return;
		if(qml_viewer)
			return;

		lock = true;
		qml_viewer = new id::QmlApplicationViewer;
		qml_viewer->setAttribute(Qt::WA_DeleteOnClose, true);
		engine = qml_viewer->engine();
		context = engine->rootContext();
        engine->addImageProvider("nighticon",new NightIconProvider(QCoreApplication::applicationDirPath()+"/../qml/" ID_QML_DIR "/images"));
		engine->setNetworkAccessManagerFactory(&factory);
		connector->SetEngine(engine);
		ut->SetEngine(engine);

		context->setContextProperty("_UT", ut);
		context->setContextProperty("_CONNECTOR", connector);
		context->setContextProperty("_PIPELINE", pipeline);
		context->setContextProperty("qt_funcView", qt_funcView);
		context->setContextProperty("settingModel", settingModel);
		context->setContextProperty("newMsgNotification", newMsgNotification);
		context->setContextProperty("accountModel", accountModel);
		context->setContextProperty("loginModel", loginModel);
		context->setContextProperty("profile", profile);
		context->setContextProperty("commentToMeModel", commentToMeModel);
		context->setContextProperty("messageCenterModel", messageCenterModel);
		context->setContextProperty("atMeModel", atMeModel);
		context->setContextProperty("updateProfileModel", updateProfileModel);
		context->setContextProperty("emotionlist", emotionlist);
		context->setContextProperty("firstPageModel", firstPageModel);
		context->setContextProperty("searchBlogModel", searchBlogModel);
		context->setContextProperty("searchUserModel", searchUserModel);
		context->setContextProperty("searchContactModel", searchContactModel);
		context->setContextProperty("searchTopicModel", searchTopicModel);
		context->setContextProperty("suiBianKanModel", suiBianKanModel);
		context->setContextProperty("microBlog", microBlog);
		context->setContextProperty("insertTopicModel", insertTopicModel);
		context->setContextProperty("hotTrendModel", hotTrendModel);
		context->setContextProperty("contentCommentModel", contentCommentModel);
		context->setContextProperty("frequentContactModel", frequentContactModel);

		qml_viewer->setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
        id::QmlApplicationViewer *window=static_cast<id::QmlApplicationViewer*>(qml_viewer);
        if(!startupProfileMode||qgetenv("N9_STARTUP_BASELINE")!="1"){
        QString images=QCoreApplication::applicationDirPath()+"/../qml/" ID_QML_DIR "/images/";
        window->openingIcon.load(images+"app_icon.png");window->openingPortrait.load(images+"blog_loading.png");window->openingLandscape.load(images+"blog_loading_landscape.png");
        window->setSceneRect(0,0,480,854);
        window->showExpanded();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        window->viewport()->repaint();
        if(startupProfileMode)QPixmap::grabWidget(window->viewport()).save("/home/developer/startup-profile39/opening-frame.png");
        startupStage("opening_frame_ready");
        }
		startupStage("before_qml");
        qml_viewer->setMainQmlFile(QLatin1String("qml/" ID_QML_DIR "/SinaWeiboFuncContainer.qml"));
        startupStage("qml_ready");
        publishNativePortrait(window);
        window->openingFrame=false;window->openingPortrait=QImage();window->openingLandscape=QImage();window->openingIcon=QImage();
        window->viewport()->update();
		qml_viewer->showExpanded();
		lock = false;
	}

	idDatabase * database_instance()
	{
		static idDatabase _db;
        activeDatabase=&_db;
		if(!_db.IsConnected())
			_db.Connect(ID_PKG, ID_DEV, ID_CODE);
		return &_db;
	}

}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
	qInstallMsgHandler(discardLegacyLog);
 if(argc == 2 && QByteArray(argv[1]) == "--network-probe") {
 QCoreApplication probe(argc, argv);
 QStringList urls; urls << "https://passport.weibo.cn/signin/login?entry=mweibo" << "https://m.weibo.cn/api/config";
 for(int i=0;i<urls.size();++i) { QByteArray body; QVariantMap headers; headers.insert("Accept-Encoding", "identity");
 int rc=idNetworkConnector::SyncRequest_thread(urls[i], QList<QPair<QByteArray,QByteArray> >(), QByteArray(), 0, headers, &body, false);
 diagnostic(QString("probe endpoint=%1 qt_error=%2 bytes=%3").arg(i).arg(rc).arg(body.size()));
 fprintf(stdout,"probe endpoint=%d qt_error=%d bytes=%d\n", i,rc,body.size()); }
 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--profile-format-probe") {
 QCoreApplication app(argc,argv);QVariantMap user;user.insert("id","fixture");user.insert("screen_name","Offline test");user.insert("friends_count",12);user.insert("avatar_large","https://example.invalid/avatar.png");QVariantMap data;data.insert("userInfo",user);QVariantMap root;root.insert("ok",1);root.insert("data",data);
 QVariantMap normalized=normalizeUserProfile(root,"fixture"),out=normalized.value("data").toMap().value("user").toMap();bool profile=normalized.value("ok").toInt()==1&&out.value("follow_count").toInt()==12&&!out.value("profile_image_url").toString().isEmpty();bool mismatch=!normalizeUserProfile(root,"other").value("ok").toInt();root.insert("data",QVariantMap());bool missing=!normalizeUserProfile(root,"fixture").value("ok").toInt();
 QString emote="<img alt='[fixture]' src='//h5.sinaimg.cn/m/emoticon/fixture.png' width='96' height='96' style='width:1em;height:1em' />",photo="<img src='https://example.invalid/photo.png' width='640' height='480' />";QString formatted=idHelper::FixedContentEmotion(emote);bool images=formatted.contains("width=\"32\"")&&formatted.contains("height=\"32\"")&&formatted.contains("https://h5.sinaimg.cn/")&&idHelper::FixedContentEmotion(photo)==photo;
 QString modernFace=idHelper::FixedContentEmotion("<img src='https://face.t.sinajs.cn/t4/appstyle/expression/normal/fixture.png' style='width: 1em; height:1em' />");QString icon=idHelper::FixedContentEmotion("<img class='url-icon' src='https://example.invalid/video.svg' />");images=images&&modernFace.contains("width=\"32\"")&&icon.contains("height=\"22\"");
 fprintf(stdout,"profile_format_probe profile=%d foreign_id_rejected=%d missing_rejected=%d inline_emote=%d\n",profile?1:0,mismatch?1:0,missing?1:0,images?1:0);return profile&&mismatch&&missing&&images?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--emotion-picker-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(offlineFixtureLog);id_emotionlist *model=id_emotionlist::Instance();QDeclarativeEngine engine;{QVariantMap defaults;defaults.insert("darkMode",false);engine.rootContext()->setContextProperty("settingModel",defaults);}engine.setNetworkAccessManagerFactory(&factory);engine.rootContext()->setContextProperty("emotionlist",model);QVariantMap fc;fc.insert("inPortrait",true);engine.rootContext()->setContextProperty("funcContainer",fc);QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/../qml/SinaWeiboClient/SinaWeiboEmotion.qml"));QObject *root=component.create();if(!root){foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"emoji_qml_error=%s\n",e.description().toUtf8().constData());return 2;}QGraphicsScene scene;QDeclarativeItem *item=qobject_cast<QDeclarativeItem*>(root);scene.addItem(item);QSet<QString> codes;bool valid=model->rowCount()==127;int checked=0;for(int layout=0;layout<2;layout++){int ps=layout==0?24:30;fc.insert("inPortrait",layout==0);engine.rootContext()->setContextProperty("funcContainer",fc);item->setWidth(layout==0?480:854);item->setHeight(layout==0?321:194);QCoreApplication::processEvents();for(int i=0;i<model->rowCount();i++){QString expected=model->getEmotionCode(i/ps,i%ps,ps);QVariant result;QMetaObject::invokeMethod(root,"selectEmotion",Q_RETURN_ARG(QVariant,result),Q_ARG(QVariant,QVariant(i/ps)),Q_ARG(QVariant,QVariant(i%ps)));if(expected.isEmpty()||!result.toBool()||root->property("emotionContent").toString()!=expected)valid=false;if(layout==0)codes.insert(expected);checked++;}int blank=model->rowCount();QVariant empty;QString before=root->property("emotionContent").toString();QMetaObject::invokeMethod(root,"selectEmotion",Q_RETURN_ARG(QVariant,empty),Q_ARG(QVariant,QVariant(blank/ps)),Q_ARG(QVariant,QVariant(blank%ps)));if(empty.toBool()||root->property("emotionContent").toString()!=before)valid=false;root->setProperty("currentPage",0);QCoreApplication::processEvents();QEventLoop imageWait;QTimer::singleShot(400,&imageWait,SLOT(quit()));imageWait.exec();int readyImages=0;foreach(QGraphicsItem *graphic,scene.items()){QGraphicsObject *child=dynamic_cast<QGraphicsObject*>(graphic);if(child && child->property("source").isValid() && child->property("source").toUrl().scheme()=="file" && child->property("status").toInt()==1)readyImages++;}if(readyImages<ps)valid=false;fprintf(stdout,"emotion_images layout=%d ready=%d\n",layout,readyImages);QImage image(qRound(item->width()),qRound(item->height()),QImage::Format_ARGB32);image.fill(qRgb(247,248,250));QPainter painter(&image);scene.setSceneRect(0,0,item->width(),item->height());scene.render(&painter);painter.end();image.save(QCoreApplication::applicationDirPath()+QString(layout==0?"/emoji-portrait.png":"/emoji-landscape.png"));}
 valid=valid&&codes.size()==127&&model->getPageCount(24)==6&&model->getPageCount(30)==5&&model->getEmotionCode(-1,0,24).isEmpty()&&model->getIconPath(0,0,0).isEmpty();fprintf(stdout,"emotion_picker_probe count=%d unique=%d checked=%d portrait_pages=%d landscape_pages=%d passed=%d\n",model->rowCount(),codes.size(),checked,model->getPageCount(24),model->getPageCount(30),valid);scene.clear();if(activeDatabase)activeDatabase->Disconnect();return valid?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--like-ui-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);WriteProbeFactory mock;qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&mock);QDeclarativeEngine engine;engine.rootContext()->setContextProperty("microBlog",id_microBlog::Instance());QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/ui-preview/LikeProbe.qml"));QObject *root=component.create();if(!root)return 2;QObject *actions=root->findChild<QObject*>("likeActions");if(!actions)return 3;bool passed=true;for(int step=0;step<3;step++){if(step==2)mock.state.writeSuccess=false;QMetaObject::invokeMethod(actions,"toggle");bool immediate=actions->property("busy").toBool()&&actions->property("effectiveLiked").toBool()==(step!=1);QTime wait;wait.start();while(actions->property("busy").toBool()&&wait.elapsed()<1500){QEventLoop pulse;QTimer::singleShot(10,&pulse,SLOT(quit()));pulse.exec();}bool expected=step==0;bool finalState=!actions->property("busy").toBool()&&actions->property("effectiveLiked").toBool()==expected;bool popup=step==2?!root->property("lastPopup").toString().isEmpty():root->property("lastPopup").toString().isEmpty();passed=passed&&immediate&&finalState&&popup;fprintf(stdout,"like_ui_probe step=%d immediate=%d settled=%d popup=%d\n",step,immediate?1:0,finalState?1:0,popup?1:0);}
 passed=passed&&mock.state.valid&&mock.state.gets==3&&mock.state.posts==3;fprintf(stdout,"like_ui_probe mock_only=1 passed=%d\n",passed?1:0);delete root;if(activeDatabase)activeDatabase->Disconnect();delete qml_viewer;qml_viewer=0;return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--write-request-probe") {
 QApplication app(argc,argv);WriteProbeFactory mock;qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&mock);QVariantMap p;p.insert("content",QString::fromUtf8("测试&+"));p.insert("mid","fixture-id");p.insert("id","fixture-id");p.insert("cid","fixture-comment");p.insert("dualPost",1);int rc=0;bool ok=true;ok=idAPI::UpdateStatus(p,&rc).value("ok").toInt()&&rc==0&&ok;ok=idAPI::SendCmt(p,&rc).value("ok").toInt()&&rc==0&&ok;ok=idAPI::Reply(p,&rc).value("ok").toInt()&&rc==0&&ok;ok=idAPI::Repost(p,&rc).value("ok").toInt()&&rc==0&&ok;ok=idAPI::Attitude(p,&rc).value("ok").toInt()&&rc==0&&ok;ok=idAPI::Unattitude(p,&rc).value("ok").toInt()&&rc==0&&ok;mock.state.login=false;bool blocked=!idAPI::UpdateStatus(p,&rc).value("ok").toInt()&&mock.state.posts==6;ok=ok&&blocked&&mock.state.valid&&mock.state.gets==7;fprintf(stdout,"write_request_probe requests=%d posts=%d session_blocked=%d passed=%d\n",mock.state.gets,mock.state.posts,blocked?1:0,ok?1:0);fprintf(stdout,"write_request_mask=%d\n",mock.state.mask);delete qml_viewer;qml_viewer=0;return ok?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--comments-model-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","detail-render");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);id_firstPageModel *feed=id_firstPageModel::Instance();feed->getSinaContentFromModel(0);QString target=feed->GetStatusId(0);for(int row=0;row<feed->rowCount();row++){if(feed->data(feed->index(row,0),id_accListModel_base::Role_commentCount).toInt()>0){target=feed->GetStatusId(row);break;}}if(target.isEmpty())return 4;

 id_contentCommentModel *model=id_contentCommentModel::Instance();model->setLatest(true);model->getContentCommentFromModel(target,0);int first=model->rowCount();model->getContentCommentFromModel(target,0);int refreshed=model->rowCount();model->setLatest(false);model->getContentCommentFromModel(target,0);int hot=model->rowCount();fprintf(stdout,"comments_model_probe latest=%d refreshed=%d hot=%d passed=%d\n",first,refreshed,hot,first>0&&refreshed>0&&hot>0?1:0);delete qml_viewer;qml_viewer=0;return first>0&&refreshed>0&&hot>0?0:1;}
 if(argc==2 && QByteArray(argv[1])=="--comments-read-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","detail-render");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);id_firstPageModel *feed=id_firstPageModel::Instance();feed->getSinaContentFromModel(0);QString target=feed->GetStatusId(0);for(int row=0;row<feed->rowCount();row++){if(feed->data(feed->index(row,0),id_accListModel_base::Role_commentCount).toInt()>0){target=feed->GetStatusId(row);break;}}if(target.isEmpty())return 4;

 for(int mode=0;mode<3;mode++){QList<QPair<QByteArray,QByteArray> > query;query<<qMakePair(QByteArray("id"),target.toUtf8())<<qMakePair(QByteArray("mid"),target.toUtf8());if(mode==0)query<<qMakePair(QByteArray("page"),QByteArray("1"));else query<<qMakePair(QByteArray("max_id_type"),QByteArray("0"))<<qMakePair(QByteArray("flow"),QByteArray::number(mode-1));QVariantMap headers;headers.insert("Referer","https://m.weibo.cn/");QByteArray bytes;idNetworkConnector connector;connector.SetEngine(qml_viewer->engine());int rc=connector.SyncRequest(mode==0?"https://m.weibo.cn/api/comments/show":"https://m.weibo.cn/comments/hotflow",query,QByteArray(),0,headers,&bytes);QJson::Parser parser;bool ok=false;QVariantMap root=parser.parse(bytes,&ok).toMap(),data=root.value("data").toMap();int rows=data.value("data").toList().size();if(root.value("data").type()==QVariant::List)rows=root.value("data").toList().size();fprintf(stdout,"comments_read_probe mode=%d error=%d json=%d ok=%d rows=%d total=%d nested=%d code=%d\n",mode,rc,ok?1:0,root.value("ok").toInt(),rows,data.value("total_number").toInt(),root.value("data").type()==QVariant::Map?1:0,root.value("errno",root.value("error_code")).toInt());bytes.fill(0);}
 delete qml_viewer;qml_viewer=0;return 0;}
 if(argc==2 && QByteArray(argv[1])=="--detail-render-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","detail-render");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);id_firstPageModel *feed=id_firstPageModel::Instance();feed->getSinaContentFromModel(0);QString target=feed->GetStatusId(0);if(target.isEmpty())return 4;
 qmlRegisterType<id_contentCommentModel>("SinaWeiboTypeLib",1,0,"CommentModel");qmlRegisterType<id_DetailModel>("SinaWeiboTypeLib",1,0,"DetailModel");qmlRegisterType<id_DeleteModel>("SinaWeiboTypeLib",1,0,"DeleteModel");QDeclarativeEngine engine;engine.setNetworkAccessManagerFactory(&factory);QVariantMap fc,login;fc.insert("statusIDInDetailPage",target);login.insert("userID","");engine.rootContext()->setContextProperty("funcContainer",fc);engine.rootContext()->setContextProperty("loginModel",login);QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/ui-preview/SinaWeiboContentDetailPage.qml"));QTime createTime;createTime.start();QObject *root=component.create();fprintf(stdout,"detail_create_millis=%d\n",createTime.elapsed());fflush(stdout);if(!root){foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"detail_error=%s\n",e.description().toUtf8().constData());return 5;}QDeclarativeItem *item=qobject_cast<QDeclarativeItem*>(root);item->setVisible(true);item->setWidth(480);item->setHeight(746);QGraphicsScene scene;scene.addItem(item);id_DetailModel *model=root->findChild<id_DetailModel*>();QTime requestWait;requestWait.start();while(model&&model->StatusID().isEmpty()&&requestWait.elapsed()<15000){QEventLoop pulse;QTimer::singleShot(100,&pulse,SLOT(quit()));pulse.exec();}QEventLoop settle;QTimer::singleShot(600,&settle,SLOT(quit()));settle.exec();bool ok=model&&!model->StatusID().isEmpty()&&!model->StatusText().isEmpty();QImage image(480,746,QImage::Format_ARGB32);image.fill(qRgb(255,255,255));QPainter painter(&image);scene.setSceneRect(0,0,480,746);scene.render(&painter);painter.end();bool saved=image.save(QCoreApplication::applicationDirPath()+"/detail-preview28.png");fprintf(stdout,"detail_render_probe loaded=%d saved=%d\n",ok?1:0,saved?1:0);fflush(stdout);scene.clear();if(activeDatabase)activeDatabase->Disconnect();delete qml_viewer;qml_viewer=0;return ok&&saved?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--write-session-probe") {
 QApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","write-session");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);int rc=0;QVariantMap root=idAPI::SessionConfig(QVariantMap(),&rc),data=root.value("data").toMap();bool token=!data.value("st").toString().isEmpty(),login=data.value("login").toBool();fprintf(stdout,"write_session_probe error=%d ok=%d login=%d token=%d cookie_token=%d\n",rc,root.value("ok").toInt(),login?1:0,token?1:0,!idUtility::Instance()->GetCookie("https://m.weibo.cn/","XSRF-TOKEN").toByteArray().isEmpty()?1:0);delete qml_viewer;qml_viewer=0;return !rc&&token&&login?0:1;
 }
 if(argc==2 && (QByteArray(argv[1])=="--qml-compile-probe"||QByteArray(argv[1])=="--qml-all-probe")) {
 QApplication app(argc,argv);qmlRegisterType<id_FavoritesModel>("SinaWeiboTypeLib",1,0,"FavoritesModel");qmlRegisterType<id_FanListModel>("SinaWeiboTypeLib",1,0,"FanListModel");qmlRegisterType<id_FriendListModel>("SinaWeiboTypeLib",1,0,"FriendListModel");qmlRegisterType<id_TrendTopicModel>("SinaWeiboTypeLib",1,0,"TrendTopicModel");qmlRegisterType<id_TrendBlogModel>("SinaWeiboTypeLib",1,0,"TrendBlogModel");qmlRegisterType<id_contentCommentModel>("SinaWeiboTypeLib",1,0,"CommentModel");qmlRegisterType<id_WeiboProfileModel>("SinaWeiboTypeLib",1,0,"WeiboProfileModel");qmlRegisterType<id_MyWeiboListModel>("SinaWeiboTypeLib",1,0,"MyWeiboListModel");qmlRegisterType<id_DetailModel>("SinaWeiboTypeLib",1,0,"DetailModel");qmlRegisterType<id_DeleteModel>("SinaWeiboTypeLib",1,0,"DeleteModel");qmlRegisterType<QDeclarativeWebView>("karin.weibomm",1,0,"KarinWebView");QDeclarativeEngine engine;QStringList files;files<<"SinaWeiboMessageCenter.qml"<<"SinaWeiboMsgPage.qml"<<"controls/RefreshPulse.qml"<<"SinaWeiboPicViewer.qml"<<"SessionFilePicker.qml"<<"SessionSyncView.qml"<<"SinaWeiboLoginPage.qml"<<"SinaWeiboAddAccount.qml"<<"AuxiliaryLoginView.qml"<<"VerificationView.qml"<<"SinaWeiboAccount.qml"<<"SinaWeiboAboutPage.qml"<<"SinaWeiboMainContainer.qml"<<"SinaWeiboFirstPage.qml"<<"SinaWeiboHotTrend.qml"<<"SinaWeiboContentComment.qml"<<"SinaWeiboContentDetailPage.qml"<<"ProfileTimeline.qml"<<"SinaWeiboMyProfilePage.qml"<<"SinaWeiboUserProfilePage.qml"<<"SinaWeiboSettingPage.qml"<<"SinaWeiboFuncContainer.qml"<<"SinaWeiboSearchPage.qml"<<"SinaWeiboMsgPage.qml"<<"SinaWeiboUpdateProfile.qml"<<"SinaWeiboNewBlog.qml";if(QByteArray(argv[1])=="--qml-all-probe"){QString base=QCoreApplication::applicationDirPath()+"/ui-preview/";files=QDir(base).entryList(QStringList()<<"*.qml",QDir::Files);foreach(const QString &name,QDir(base+"controls").entryList(QStringList()<<"*.qml",QDir::Files))files<<"controls/"+name;files.removeAll("Preview.qml");files.removeAll("RefreshProbe37.qml");files.removeAll("ZoomProbe37.qml");}
 bool ok=true;foreach(const QString &file,files){QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/ui-preview/"+file));bool ready=component.status()==QDeclarativeComponent::Ready;fprintf(stdout,"qml_compile file=%s ready=%d\n",file.toUtf8().constData(),ready);if(!ready){foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"qml_error=%s\n",e.description().toUtf8().constData());ok=false;}}return ok?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--ui-preview-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(offlineFixtureLog);qmlRegisterType<id_contentCommentModel>("SinaWeiboTypeLib",1,0,"CommentModel");qmlRegisterType<id_DetailModel>("SinaWeiboTypeLib",1,0,"DetailModel");qmlRegisterType<id_DeleteModel>("SinaWeiboTypeLib",1,0,"DeleteModel");qmlRegisterType<id_WeiboProfileModel>("SinaWeiboTypeLib",1,0,"WeiboProfileModel");qmlRegisterType<id_MyWeiboListModel>("SinaWeiboTypeLib",1,0,"MyWeiboListModel");qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);QDeclarativeEngine engine;engine.setNetworkAccessManagerFactory(&factory);engine.rootContext()->setContextProperty("hotTrendModel",id_hotTrendModel::Instance());QVariantMap settings;settings.insert("portraitLocked",false);settings.insert("darkMode",false);engine.rootContext()->setContextProperty("settingModel",settings);QString dir=QCoreApplication::applicationDirPath()+"/ui-preview/";engine.addImageProvider("nighticon",new NightIconProvider(dir+"images"));
 QDeclarativeComponent component(&engine,QUrl::fromLocalFile(dir+"Preview.qml"));QObject *object=component.create();if(!object){foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"preview_error=%s\n",e.description().toUtf8().constData());return 2;}
 QGraphicsScene scene;QDeclarativeItem *item=qobject_cast<QDeclarativeItem*>(object);scene.addItem(item);QCoreApplication::processEvents();QEventLoop visualWait;QTimer::singleShot(1100,&visualWait,SLOT(quit()));visualWait.exec();QImage image(qRound(item->width()),qRound(item->height()),QImage::Format_ARGB32);image.fill(qRgb(255,255,255));QPainter painter(&image);scene.setSceneRect(0,0,item->width(),item->height());scene.render(&painter);painter.end();bool saved=image.save(dir+"timeline-preview.png");
 QDeclarativeComponent about(&engine,QUrl::fromLocalFile(dir+"SinaWeiboAboutPage.qml"));QObject *aboutItem=about.create();bool aboutOk=aboutItem!=0;delete aboutItem;
 fprintf(stdout,"ui_preview_probe timeline=%d static_about=%d\n",saved?1:0,aboutOk?1:0);scene.clear();fprintf(stdout,"ui_preview_teardown scene=1\n");if(activeDatabase)activeDatabase->Disconnect();fprintf(stdout,"ui_preview_teardown database=1\n");delete qml_viewer;qml_viewer=0;return saved&&aboutOk?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--feed-layout-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(offlineFixtureLog);QDeclarativeEngine engine;QVariantMap roles;roles.insert("isVip",false);roles.insert("userNickName",QString::fromUtf8("这是一个很长很长很长的离线测试昵称"));roles.insert("untilTime","");roles.insert("hasPic",0);roles.insert("hasMap",0);roles.insert("weibocontent",QString::fromUtf8("离线排版测试：昵称和时间应该各自显示，正文自然换行。"));roles.insert("image","");roles.insert("fromPlat",QString::fromUtf8("一个很长很长的测试客户端来源"));roles.insert("commentCount",123);roles.insert("forwardCount",456);roles.insert("forwardcontent","");roles.insert("forwardThumbnailPic","");roles.insert("statusThumbnailPic","");roles.insert("userId","fixture");roles.insert("index",0);
 for(QVariantMap::const_iterator it=roles.constBegin();it!=roles.constEnd();++it)engine.rootContext()->setContextProperty(it.key(),it.value());QVariantMap fc;fc.insert("inPortrait",true);engine.rootContext()->setContextProperty("funcContainer",fc);
 QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/layout-probe/AccListSina.qml"));QObject *object=component.create();if(!object){fprintf(stdout,"feed_layout_probe component=0\n");foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"qml_error=%s\n",e.description().toUtf8().constData());return 2;}
 QGraphicsScene scene;QDeclarativeItem *container=new QDeclarativeItem;container->setWidth(480);container->setHeight(640);scene.addItem(container);QDeclarativeItem *item=qobject_cast<QDeclarativeItem*>(object);item->setParentItem(container);QCoreApplication::processEvents();engine.rootContext()->setContextProperty("untilTime",QString("Tue Nov 03 11:39:36 +0800 2026"));QCoreApplication::processEvents();
 QDeclarativeItem *author=object->findChild<QDeclarativeItem*>("feedAuthor"),*time=object->findChild<QDeclarativeItem*>("feedTime"),*source=object->findChild<QDeclarativeItem*>("feedSource");bool geometry=author&&time&&source&&author->width()>0&&author->x()+author->width()<=time->x()&&time->x()+time->width()<=400&&source->width()>0;bool shortened=time&&time->width()>0&&!time->property("text").toString().isEmpty()&&time->property("text").toString().size()<=15;
 QImage image(480,640,QImage::Format_ARGB32);image.fill(qRgb(240,241,242));QPainter painter(&image);scene.setSceneRect(0,0,480,640);scene.render(&painter);painter.end();bool saved=image.save(QCoreApplication::applicationDirPath()+"/layout-probe/feed-preview.png");fprintf(stdout,"feed_layout_probe component=1 non_overlap=%d date_shortened=%d preview=%d\n",geometry?1:0,shortened?1:0,saved?1:0);return geometry&&shortened&&saved?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--user-home-probe") {
 QApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","home-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT user_id,cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QString ownId=q.value(0).toString();QByteArray cookie=q.value(1).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);
 id_MyWeiboListModel own;own.connectModel();own.setUser(ownId,"");own.getMyWeiBoFromModel(0);int ownRows=own.rowCount();
 QVariantMap params;params.insert("feed_kind","following");int ret=0;QVariantMap feed=idAPI::Index(params,&ret);QString otherId;foreach(const QVariant &v,feed.value("data").toMap().value("statuses").toList()){QString candidate=v.toMap().value("user").toMap().value("id").toString();if(!candidate.isEmpty()&&candidate!=ownId){otherId=candidate;break;}}
 id_MyWeiboListModel other;other.connectModel();other.setUser(otherId,"");other.getMyWeiBoFromModel(0);int first=other.rowCount();other.getMyWeiBoFromModel(2);int second=other.rowCount();bool match=true;for(int n=0;n<other.rowCount();n++)if(other.data(other.index(n,0),id_accListModel_base::Role_userId).toString()!=otherId)match=false;
 bool passed=!otherId.isEmpty()&&first>0&&second>first&&match&&own.rowCount()==ownRows;fprintf(stdout,"user_home_probe own_rows=%d other_rows=%d other_next=%d author_match=%d independent=%d passed=%d\n",ownRows,first,second,match,own.rowCount()==ownRows,passed);delete qml_viewer;qml_viewer=0;return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--discovery-model-probe") {
 QApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","home-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT user_id,cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QString ownId=q.value(0).toString();QByteArray cookie=q.value(1).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);
 id_hotTrendModel *hot=id_hotTrendModel::Instance();hot->getSinaHotTrendFromModel(0);int words=hot->rowCount();id_suiBianKanModel *browse=id_suiBianKanModel::Instance();browse->getSuiBianKanFromModel(0);int first=browse->rowCount();browse->getSuiBianKanFromModel(2);int second=browse->rowCount();id_searchBlogModel *search=id_searchBlogModel::Instance();search->searchBlogFromModel(0,QString::fromUtf8("天气"));int results=search->rowCount();bool passed=words>0&&first>0&&second>first&&results>0;fprintf(stdout,"discovery_model_probe words=%d browse=%d browse_next=%d search=%d passed=%d\n",words,first,second,results,passed);delete qml_viewer;qml_viewer=0;return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--profile-top-probe") {
  QApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","home-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT user_id,cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QString ownId=q.value(0).toString();QByteArray cookie=q.value(1).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);

 QVariantMap params;params.insert("feed_kind","following");int ret=0;QVariantMap feed=idAPI::Index(params,&ret);QString other;foreach(const QVariant &v,feed.value("data").toMap().value("statuses").toList()){QString uid=v.toMap().value("user").toMap().value("id").toString();if(!uid.isEmpty()&&uid!=ownId){other=uid;break;}}
 qmlRegisterType<id_WeiboProfileModel>("SinaWeiboTypeLib",1,0,"WeiboProfileModel");qmlRegisterType<id_MyWeiboListModel>("SinaWeiboTypeLib",1,0,"MyWeiboListModel");QDeclarativeEngine engine;engine.setNetworkAccessManagerFactory(&factory);QVariantMap login,fc;login.insert("userID",ownId);fc.insert("readMode",0);engine.rootContext()->setContextProperty("loginModel",login);engine.rootContext()->setContextProperty("funcContainer",fc);QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/ui-preview/ProfileTimeline.qml"));QObject *object=component.create();if(!object)return 4;QGraphicsScene scene;QDeclarativeItem *item=qobject_cast<QDeclarativeItem*>(object);item->setWidth(480);item->setHeight(746);scene.addItem(item);object->setProperty("userId",other);QEventLoop wait;QTimer::singleShot(800,&wait,SLOT(quit()));wait.exec();QEventLoop layout;QTimer::singleShot(80,&layout,SLOT(quit()));layout.exec();QDeclarativeItem *list=object->findChild<QDeclarativeItem*>("profilePosts");qreal top=-999;foreach(QGraphicsItem *graphic,scene.items()){QGraphicsObject *o=dynamic_cast<QGraphicsObject*>(graphic);if(o&&o->objectName()=="profileSummary")top=graphic->mapToScene(QPointF(0,0)).y();}int count=list?list->property("count").toInt():0;bool passed=count>0&&top>=65&&top<=100;fprintf(stdout,"profile_top_probe posts=%d summary_y=%d passed=%d\n",count,qRound(top),passed);scene.clear();delete qml_viewer;qml_viewer=0;if(activeDatabase)activeDatabase->Disconnect();return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--discovery-probe") {
 QCoreApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","discovery-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();
 for(int i=0;i<3;i++){QList<QPair<QByteArray,QByteArray> > query;if(i>0)query<<qMakePair(QByteArray("containerid"),i==1?QByteArray("106003type=25&t=3&disable_hot=1&filter_type=realtimehot"):QByteArray("231583"));QVariantMap h;h.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");h.insert("Referer",i==0?"https://weibo.com/":"https://m.weibo.cn/");QByteArray bytes;int rc=idNetworkConnector::SyncRequest_thread(i==0?"https://weibo.com/ajax/side/hotSearch":"https://m.weibo.cn/api/container/getIndex",query,QByteArray(),0,h,&bytes,false);QJson::Parser parser;bool ok=false;QVariantMap root=parser.parse(bytes,&ok).toMap(),data=root.value("data").toMap();QVariantList cards=data.value("cards").toList();int titles=0;QVariantList pending=cards;while(!pending.isEmpty()){QVariantMap c=pending.takeFirst().toMap();if(!c.value("desc").toString().isEmpty()||!c.value("title").toString().isEmpty())titles++;pending+=c.value("card_group").toList();}fprintf(stdout,"discovery_probe case=%d error=%d json=%d ok=%d words=%d cards=%d titles=%d\n",i,rc,ok?1:0,root.value("ok").toInt(),data.value("realtime").toList().size(),cards.size(),titles);bytes.fill(0);}
 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--feed-model-probe") {
 QApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","feed-model-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&factory);
 id_firstPageModel *model=id_firstPageModel::Instance();model->getSinaContentFromModel(0);int following=model->rowCount();model->getSinaContentFromModel(idAPI::Type_Next);int followingNext=model->rowCount();QStringList before;for(int i=0;i<model->rowCount();i++)before<<model->GetStatusId(i);QPersistentModelIndex anchor(model->index(0,0));model->getSinaContentFromModel(0);int added=model->rowCount()-before.size();bool refreshKept=added>=0&&anchor.isValid()&&model->GetStatusId(anchor.row())==before.first();for(int i=0;i<before.size()&&refreshKept;i++)if(model->GetStatusId(added+i)!=before[i])refreshKept=false;fprintf(stdout,"feed_refresh_probe kept=%d old_rows=%d added=%d persistent=%d\n",refreshKept,before.size(),added,anchor.isValid());model->getSinaContentFromModel(idAPI::Type_Next);bool cursorKept=model->rowCount()>followingNext+added;model->setRecommended(true);int recommended=model->rowCount();model->getSinaContentFromModel(idAPI::Type_Next);int recommendedNext=model->rowCount();bool success=refreshKept&&cursorKept&&following>0&&followingNext>following&&recommended>0&&recommendedNext>recommended;fprintf(stdout,"feed_model_probe following=%d following_next=%d recommended=%d recommended_next=%d passed=%d\n",following,followingNext,recommended,recommendedNext,success?1:0);delete qml_viewer;qml_viewer=0;return success?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--feed-endpoint-probe") {
 QCoreApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","feed-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open())return 2;QSqlQuery q(db);if(!q.exec("SELECT cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next())return 3;QByteArray cookie=q.value(0).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();
 for(int i=0;i<2;i++){QString cursor;QSet<QString> seen;for(int page=0;page<2;page++){QList<QPair<QByteArray,QByteArray> > query;if(i==0)query<<qMakePair(QByteArray("containerid"),QByteArray("102803"))<<qMakePair(QByteArray("openApp"),QByteArray("0"));if(!cursor.isEmpty())query<<qMakePair(i==0?QByteArray("since_id"):QByteArray("max_id"),cursor.toUtf8());QVariantMap h;h.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");h.insert("Referer","https://m.weibo.cn/");QByteArray bytes;int rc=idNetworkConnector::SyncRequest_thread(i==0?"https://m.weibo.cn/api/container/getIndex":"https://m.weibo.cn/feed/friends",query,QByteArray(),0,h,&bytes,false);QJson::Parser parser;bool ok=false;QVariantMap root=parser.parse(bytes,&ok).toMap(),data=root.value("data").toMap();QVariantList posts=data.value("statuses").toList();foreach(const QVariant &card,data.value("cards").toList()){QVariantMap m=card.toMap().value("mblog").toMap();if(!m.isEmpty())posts<<m;}int follow=0,other=0,unknown=0,fresh=0;foreach(const QVariant &v,posts){QVariantMap post=v.toMap(),u=post.value("user").toMap();QString id=post.value("id").toString();if(!seen.contains(id)){seen.insert(id);fresh++;}if(!u.contains("following"))unknown++;else if(u.value("following").toBool())follow++;else other++;}QString next=(i==0?data.value("cardlistInfo").toMap().value("since_id"):data.value("max_id")).toString();fprintf(stdout,"feed_endpoint case=%d page=%d error=%d json=%d ok=%d posts=%d following=%d other=%d unknown=%d new=%d cursor=%d advanced=%d\n",i,page,rc,ok?1:0,root.value("ok").toInt(),posts.size(),follow,other,unknown,fresh,!next.isEmpty(),next!=cursor);cursor=next;bytes.fill(0);if(cursor.isEmpty())break;}}

 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--profile-endpoint-probe") {
 QCoreApplication app(argc,argv);QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","profile-probe");db.setConnectOptions("QSQLITE_OPEN_READONLY");db.setDatabaseName("/home/user/.local/share/data/karin/weibomm/weibomm.db");if(!db.open()){fprintf(stdout,"profile_probe database=0\n");return 2;}
 QSqlQuery q(db);if(!q.exec("SELECT user_id,cookie FROM id_account ORDER BY ts DESC LIMIT 1")||!q.next()){fprintf(stdout,"profile_probe account=0\n");return 2;}QByteArray uid=q.value(0).toByteArray(),cookie=q.value(1).toByteArray();idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(cookie));cookie.fill(0);q.finish();db.close();
 QStringList urls;urls<<"https://m.weibo.cn/profile/info"<<"https://m.weibo.cn/api/container/getIndex";
 for(int i=0;i<urls.size();i++){QList<QPair<QByteArray,QByteArray> > query;if(i==0)query<<qMakePair(QByteArray("uid"),uid);else query<<qMakePair(QByteArray("type"),QByteArray("uid"))<<qMakePair(QByteArray("value"),uid);
 QVariantMap h;h.insert("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");h.insert("Accept","application/json");h.insert("Referer","https://m.weibo.cn/");QByteArray bytes;int rc=idNetworkConnector::SyncRequest_thread(urls[i],query,QByteArray(),0,h,&bytes,false);QJson::Parser parser;bool ok=false;QVariantMap root=parser.parse(bytes,&ok).toMap(),data=root.value("data").toMap(),user=data.value("userInfo").toMap();if(user.isEmpty())user=data.value("user").toMap();
 fprintf(stdout,"profile_endpoint case=%d error=%d json=%d ok=%d has_user=%d id_match=%d name=%d avatar=%d description=%d location=%d follow_count=%d friends_count=%d followers_count=%d\n",i,rc,ok?1:0,root.value("ok").toInt(),!user.isEmpty()?1:0,user.value("id").toString().toUtf8()==uid?1:0,!user.value("screen_name").toString().isEmpty()?1:0,!user.value("profile_image_url").toString().isEmpty()?1:0,user.contains("description")?1:0,user.contains("location")?1:0,user.contains("follow_count")?1:0,user.contains("friends_count")?1:0,user.contains("followers_count")?1:0);
 if(i==1){QVariantMap adapted=normalizeUserProfile(root,QString::fromLatin1(uid));fprintf(stdout,"live_profile_adapter normalized=%d\n",adapted.value("ok").toInt());}
 bytes.fill(0);}
 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--import-transaction-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);WriteProbeFactory mock;mock.state.login=false;qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&mock);idNetworkCookieJar *jar=idNetworkCookieJar::Instance();QNetworkCookie original("fixture-original","keep");original.setDomain("m.weibo.cn");original.setPath("/");jar->SetAllCookies(QList<QNetworkCookie>()<<original);id_loginModel *model=id_loginModel::Instance();bool malformed=!model->importBrowserSession("invalid\nCookie");bool oldKept=jar->GetAllCookies().size()==1&&jar->GetAllCookies().first().value()=="keep";bool rejected=!model->importBrowserSession("SUB=fixture-invalid");bool restored=jar->GetAllCookies().size()==1&&jar->GetAllCookies().first().value()=="keep";bool passed=malformed&&oldKept&&rejected&&restored&&mock.state.gets==1&&mock.state.posts==0;fprintf(stdout,"import_transaction_probe malformed=%d original_kept=%d unauthenticated=%d restored=%d mock_only=1 passed=%d\n",malformed?1:0,oldKept?1:0,rejected?1:0,restored?1:0,passed?1:0);jar->SetAllCookies(QList<QNetworkCookie>());delete qml_viewer;qml_viewer=0;return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--browser-cookie-probe") {
 QCoreApplication app(argc,argv);QList<QNetworkCookie> values;
 bool valid=parseBrowserSession("Cookie: SUB=diagnostic-only%2B; sample=a=b",&values)&&values.size()==2&&values.first().value()=="diagnostic-only%2B"&&values.first().domain().isEmpty()&&values.first().isSecure()&&values.first().isHttpOnly();
 bool newline=!parseBrowserSession("a=b\r\nHost: invalid",&values),duplicate=!parseBrowserSession("a=b; a=c",&values),badName=!parseBrowserSession("bad name=x",&values),large=!parseBrowserSession(QByteArray(16385,'a'),&values);
 fprintf(stdout,"browser_cookie_parser valid=%d newline_rejected=%d duplicate_rejected=%d name_rejected=%d large_rejected=%d\n",valid?1:0,newline?1:0,duplicate?1:0,badName?1:0,large?1:0);return valid&&newline&&duplicate&&badName&&large?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--import-browser-session") {
 QCoreApplication app(argc,argv);QByteArray input;char chunk[1024];size_t n;
 while((n=fread(chunk,1,sizeof(chunk),stdin))>0){input.append(chunk,n);if(input.size()>16384){input.fill(0);fprintf(stdout,"{\"result\":\"invalid_input\"}\n");return 2;}}
 QList<QNetworkCookie> cookies;bool parsed=parseBrowserSession(input,&cookies);input.fill(0);input.clear();if(!parsed){fprintf(stdout,"{\"result\":\"invalid_input\"}\n");return 2;}
 idNetworkCookieJar *jar=idNetworkCookieJar::Instance();jar->SetAllCookies(QList<QNetworkCookie>());bool accepted=jar->setCookiesFromUrl(cookies,QUrl("https://m.weibo.cn/"));cookies.clear();
 QVariantMap state=accepted?checkLoginSession(0):QVariantMap();bool authenticated=state.value("retcode").toInt()==20000000;state.clear();
 bool saved=authenticated&&PendingLogin::save(QUrl("https://m.weibo.cn/"),jar,0,"/home/user/.cache/weibomm-pending-login.json");jar->SetAllCookies(QList<QNetworkCookie>());
 fprintf(stdout,"{\"result\":\"%s\"}\n",saved?"authenticated":authenticated?"save_failed":"not_authenticated");return saved?0:3;
 }
 if(argc==2 && QByteArray(argv[1])=="--native-portrait-probe") {
 QApplication app(argc,argv);QWidget window;bool published=publishNativePortrait(&window);QLibrary x11("X11",6);typedef unsigned long (*Intern)(void*,const char*,int);typedef int (*Get)(void*,unsigned long,unsigned long,long,long,int,unsigned long,unsigned long*,int*,unsigned long*,unsigned long*,unsigned char**);typedef int (*Free)(void*);Intern intern=reinterpret_cast<Intern>(x11.resolve("XInternAtom"));Get get=reinterpret_cast<Get>(x11.resolve("XGetWindowProperty"));Free freeData=reinterpret_cast<Free>(x11.resolve("XFree"));unsigned long actual=0,count=0,left=0;int format=0;unsigned char *data=0;bool verified=false;if(published&&intern&&get&&freeData){get(QX11Info::display(),window.winId(),intern(QX11Info::display(),"_MEEGOTOUCH_ORIENTATION_ANGLE",0),0,1,0,0,&actual,&format,&count,&left,&data);verified=format==32&&count==1&&data&&*reinterpret_cast<unsigned long*>(data)==270;if(data)freeData(data);}fprintf(stdout,"native_portrait_probe published=%d verified=%d unmapped=1 passed=%d\n",published?1:0,verified?1:0,published&&verified?1:0);return published&&verified?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--preferences-preview-probe") {
 QApplication app(argc,argv);app.setApplicationName("weibomm-preferences-probe");app.setOrganizationName("weibomm-diagnostics");QSettings stored;stored.clear();id_settingModel *settings=id_settingModel::Instance();bool defaults=!settings->largeText()&&settings->autoImages()&&settings->latestComments();settings->setLargeText(true);settings->setAutoImages(false);settings->setLatestComments(false);QSettings reload;bool persisted=reload.value("large_text").toBool()&&reload.value("image_mode",-1).toInt()==1&&!reload.value("latest_comments",true).toBool();class PreviewFixture:public id_contentCommentModel{public:void seed(){for(int i=0;i<25;i++){QVariantMap m;m.insert("statusId",QString::number(i));m_list.append(m);}}};PreviewFixture fixture;fixture.seed();settings->setImageMode(2);bool noImages=settings->imageMode()==2&&!settings->autoImages();settings->setImageMode(1);bool manual=settings->imageMode()==1;settings->setImageMode(0);bool automatic=settings->autoImages();
 bool darkDefault=!settings->darkMode();settings->setDarkMode(true);QSettings darkReload;bool darkSaved=darkReload.value("dark_mode",false).toBool()&&settings->darkMode();settings->setDarkMode(false);
 settings->setFontSizeMode(0);bool small=settings->bodyTextSize()==22;settings->setFontSizeMode(1);bool normal=settings->bodyTextSize()==24;settings->setFontSizeMode(2);QSettings fontReload;bool large=settings->bodyTextSize()==28&&settings->largeText()&&fontReload.value("font_size_mode",-1).toInt()==2;
 bool cap=small&&normal&&large&&darkDefault&&darkSaved&&!settings->darkMode()&&noImages&&manual&&automatic&&fixture.previewItems(15).size()==15&&fixture.previewItems(100).size()==15&&fixture.previewItems(-1).isEmpty()&&fixture.previewItems(5).size()==5;stored.clear();stored.sync();if(activeDatabase)activeDatabase->Disconnect();fprintf(stdout,"preferences_preview_probe defaults=%d persisted=%d preview_cap=%d isolated=1 passed=%d\n",defaults?1:0,persisted?1:0,cap?1:0,defaults&&persisted&&cap?1:0);return defaults&&persisted&&cap?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--interaction-ui-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(offlineFixtureLog);QDeclarativeEngine engine;engine.rootContext()->setContextProperty("_UT",idUtility::Instance());bool passed=true;
 for(int mode=0;mode<2;mode++){QDeclarativeComponent component(&engine,QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+QString(mode==0?"/ui-preview/RefreshProbe37.qml":"/ui-preview/ZoomProbe37.qml")));QObject *root=component.create();if(!root){foreach(const QDeclarativeError &e,component.errors())fprintf(stdout,"interaction_qml_error=%s\n",e.description().toUtf8().constData());return 2;}QGraphicsScene scene;scene.addItem(qobject_cast<QDeclarativeItem*>(root));QEventLoop settle;QTimer::singleShot(300,&settle,SLOT(quit()));settle.exec();
 if(mode==0){QObject *home=root->findChild<QObject*>("homePreview"),*list=root->findChild<QObject*>("homeFeedList");if(!home||!list)return 3;list->setProperty("contentY",600);QMetaObject::invokeMethod(home,"returnToTopAndRefresh");QEventLoop halfway;QTimer::singleShot(120,&halfway,SLOT(quit()));halfway.exec();qreal intermediate=list->property("contentY").toReal();bool animated=intermediate>0&&intermediate<600;QEventLoop banner;QTimer::singleShot(320,&banner,SLOT(quit()));banner.exec();bool visible=home->property("userRefresh").toBool()&&home->property("refreshOffset").toReal()>60;QImage image(480,746,QImage::Format_ARGB32);image.fill(qRgb(255,255,255));QPainter painter(&image);scene.setSceneRect(0,0,480,746);scene.render(&painter);painter.end();image.save(QCoreApplication::applicationDirPath()+"/refresh-preview37.png");QEventLoop finish;QTimer::singleShot(1100,&finish,SLOT(quit()));finish.exec();bool once=root->property("requestCount").toInt()==1&&!home->property("userRefresh").toBool();list->setProperty("beganAtTop",true);list->setProperty("gestureOriginY",0);list->setProperty("contentY",-240);list->setProperty("allowRefresh",true);QObject receiver;QEvent release(QEvent::GraphicsSceneMouseRelease);QCoreApplication::sendEvent(&receiver,&release);bool continuous=qAbs(home->property("refreshOffset").toReal()-240)<1&&list->property("contentY").toReal()>-2;QEventLoop snap;QTimer::singleShot(190,&snap,SLOT(quit()));snap.exec();bool normalized=list->property("contentY").toReal()>-2;QImage pullImage(480,746,QImage::Format_ARGB32);pullImage.fill(qRgb(255,255,255));QPainter pullPainter(&pullImage);scene.setSceneRect(0,0,480,746);scene.render(&pullPainter);pullPainter.end();pullImage.save(QCoreApplication::applicationDirPath()+"/pull-preview38.png");QEventLoop pull;QTimer::singleShot(900,&pull,SLOT(quit()));pull.exec();bool again=root->property("requestCount").toInt()==2&&!home->property("userRefresh").toBool();passed=passed&&animated&&visible&&once&&again&&normalized&&continuous;fprintf(stdout,"refresh_ui_probe animated=%d banner=%d tab_requests_once=%d pull_refresh=%d overscroll_normalized=%d release_continuous=%d\n",animated?1:0,visible?1:0,once?1:0,again?1:0,normalized?1:0,continuous?1:0);}
 else {QObject *zoom=root->findChild<QObject*>("zoomPreview");if(!zoom)return 4;QVariant ignored;QMetaObject::invokeMethod(zoom,"zoomTo",Q_RETURN_ARG(QVariant,ignored),Q_ARG(QVariant,QVariant(2.0)));bool doubled=qAbs(zoom->property("zoom").toReal()-2)<0.01&&zoom->property("imageWidth").toReal()>480;QMetaObject::invokeMethod(zoom,"zoomTo",Q_RETURN_ARG(QVariant,ignored),Q_ARG(QVariant,QVariant(10.0)));bool limited=qAbs(zoom->property("zoom").toReal()-4)<0.01;QMetaObject::invokeMethod(zoom,"resetZoom");bool reset=qAbs(zoom->property("zoom").toReal()-1)<0.01&&qAbs(zoom->property("panX").toReal())<1&&qAbs(zoom->property("panY").toReal())<1;passed=passed&&doubled&&limited&&reset;fprintf(stdout,"zoom_ui_probe doubled=%d limited=%d reset=%d\n",doubled?1:0,limited?1:0,reset?1:0);}
 scene.clear();}
 if(activeDatabase)activeDatabase->Disconnect();fprintf(stdout,"interaction_ui_probe offline_only=1 passed=%d\n",passed?1:0);return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--file-import-probe") {
 QApplication app(argc,argv);qInstallMsgHandler(discardLegacyLog);QString dir="/home/user/MyDocs/.weibomm-import-selftest";if(!QDir().mkpath(dir))return 2;QFile file(dir+"/fixture.txt");if(!file.open(QIODevice::WriteOnly))return 3;file.write(QByteArray::fromHex("efbbbf")+"SUB=fixture-invalid\n");file.close();QFile large(dir+"/large.txt");if(!large.open(QIODevice::WriteOnly))return 3;large.write(QByteArray(16385,'x'));large.close();QFile other(dir+"/ignore.json");if(!other.open(QIODevice::WriteOnly))return 3;other.write("{}");other.close();WriteProbeFactory mock;mock.state.login=false;qml_viewer=new QmlApplicationViewer;qml_viewer->engine()->setNetworkAccessManagerFactory(&mock);id_loginModel *model=id_loginModel::Instance();bool restricted=model->browserSessionFiles("/home/user").isEmpty();QVariantMap listing=model->browserSessionFiles(dir);bool filtered=listing.value("entries").toList().size()==2;bool oversized=!model->importBrowserSessionFile(dir+"/large.txt")&&mock.state.gets==0;bool extension=!model->importBrowserSessionFile(dir+"/ignore.json")&&mock.state.gets==0;bool parsed=!model->importBrowserSessionFile(dir+"/fixture.txt")&&mock.state.gets==1&&mock.state.posts==0;QFile::remove(dir+"/fixture.txt");QFile::remove(dir+"/large.txt");QFile::remove(dir+"/ignore.json");QDir().rmdir(dir);bool passed=restricted&&filtered&&oversized&&extension&&parsed;fprintf(stdout,"file_import_probe restricted=%d filtered=%d oversized=%d extension=%d bom_parsed=%d mock_only=1 passed=%d\n",restricted?1:0,filtered?1:0,oversized?1:0,extension?1:0,parsed?1:0,passed?1:0);delete qml_viewer;qml_viewer=0;return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--sina-trust-probe") {
 QCoreApplication app(argc,argv);QString file=QDir::homePath()+"/.cache/weibomm-sina-trust-selftest.json";idNetworkCookieJar *jar=idNetworkCookieJar::Instance();QNetworkCookie cookie("fixture","diagnostic");cookie.setDomain(".sina.cn");cookie.setPath("/");jar->SetAllCookies(QList<QNetworkCookie>()<<cookie);QUrl url("https://passport.sina.cn/sso/crossdomain");bool allowed=PendingLogin::trusted(url),saved=PendingLogin::save(url,jar,0,file);QVariantMap state=PendingLogin::load(file);jar->SetAllCookies(QList<QNetworkCookie>());bool restored=PendingLogin::restore(state)&&jar->GetAllCookies().size()==1;bool blocked=!PendingLogin::trusted(QUrl("https://passport.sina.cn.evil.invalid/"))&&!PendingLogin::trusted(QUrl("https://evil.sina.cn/"));PendingLogin::clear(file);jar->SetAllCookies(QList<QNetworkCookie>());bool passed=allowed&&saved&&restored&&blocked;fprintf(stdout,"sina_trust_probe official=%d saved=%d restored=%d unrelated_blocked=%d passed=%d\n",allowed?1:0,saved?1:0,restored?1:0,blocked?1:0,passed?1:0);return passed?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--pending-login-probe") {
 QCoreApplication app(argc,argv);QString file=QDir::homePath()+"/.cache/weibomm-pending-selftest.json";PendingLogin::clear(file);
 idNetworkCookieJar *jar=idNetworkCookieJar::Instance();QNetworkCookie c("pending-test","diagnostic-only");c.setDomain(".weibo.cn");c.setPath("/");jar->SetAllCookies(QList<QNetworkCookie>()<<c);
 QUrl fixture=QUrl::fromEncoded("https://login.sina.com.cn/sso/v2/crossdomain?ticket=diagnostic%2Binvalid%3D&entry=wapsso&url=https%3A%2F%2Fm.weibo.cn%2F");
 bool saved=PendingLogin::save(fixture,jar,0,file);QVariantMap state=PendingLogin::load(file);bool exact=state.value("url").toString().toUtf8()==fixture.toEncoded();QFile f(file);bool privateFile=!(f.permissions()&(QFile::ReadGroup|QFile::WriteGroup|QFile::ReadOther|QFile::WriteOther));
 jar->SetAllCookies(QList<QNetworkCookie>());bool restored=PendingLogin::restore(state)&&jar->GetAllCookies().size()==1&&jar->GetAllCookies().first().value()=="diagnostic-only";
 bool expired=PendingLogin::save(fixture,jar,QDateTime::currentMSecsSinceEpoch()-900001,file)&&PendingLogin::load(file).isEmpty()&&!QFile::exists(file);
 bool rejected=!PendingLogin::save(QUrl("https://untrusted.invalid/"),jar,0,file);PendingLogin::clear(file);
 fprintf(stdout,"pending_login_probe saved=%d exact=%d private=%d restored=%d expired=%d untrusted_rejected=%d\n",saved?1:0,exact?1:0,privateFile?1:0,restored?1:0,expired?1:0,rejected?1:0);return saved&&exact&&privateFile&&restored&&expired&&rejected?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--sso-uri-chain-probe") {
 QApplication app(argc,argv);QByteArray fixture="https://login.sina.com.cn/sso/v2/crossdomain?ticket=diagnostic%2Binvalid%3D&entry=wapsso&url=https%3A%2F%2Fm.weibo.cn%2F";
 QUrl parsed=QUrl::fromEncoded(fixture);QVariant stored(parsed.toEncoded());QString passed=stored.toString();
 QNetworkAccessManager offline;offline.setNetworkAccessible(QNetworkAccessManager::NotAccessible);QDeclarativeWebView view;view.setWidth(480);view.setHeight(640);view.page()->setNetworkAccessManager(&offline);
 bool invoked=QMetaObject::invokeMethod(&view,"StartSessionSync",Qt::DirectConnection,Q_ARG(QString,passed));
 QUrl requested=view.page()->mainFrame()->requestedUrl();bool ok=invoked && requested.toEncoded()==fixture && requested.queryItemValue("ticket")=="diagnostic+invalid=";
 view.page()->triggerAction(QWebPage::Stop);fprintf(stdout,"offline_sso_uri_chain invoked=%d exact=%d ticket_decoded=%d network_disabled=%d\n",invoked?1:0,requested.toEncoded()==fixture?1:0,requested.queryItemValue("ticket")=="diagnostic+invalid="?1:0,offline.networkAccessible()==QNetworkAccessManager::NotAccessible?1:0);return ok?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--sso-url-probe") {
 QCoreApplication app(argc,argv);idNetworkAccessManager manager;
 QStringList fixtures;fixtures<<"https://login.sina.com.cn/sso/v2/crossdomain?entry=wapsso&url=https%3A%2F%2Fm.weibo.cn%2F&ticket=diagnostic-invalid"<<"https://login.sina.com.cn/sso/v2/crossdomain?entry=wapsso&url=https://m.weibo.cn/&ticket=diagnostic-invalid";
 fixtures<<fixtures[0];
 for(int i=0;i<fixtures.size();i++){
 QUrl url=i==2?QUrl::fromEncoded(fixtures[i].toUtf8()):QUrl(fixtures[i]);fprintf(stdout,"public_fixture case=%d encoded=%s\n",i,url.toEncoded().constData());
 QNetworkRequest req(url);req.setRawHeader("User-Agent","WeiboMM-N9/2026 Qt/4.7.4");req.setRawHeader("Referer","https://passport.weibo.com/sso/signin");QNetworkReply *reply=manager.get(req);QEventLoop loop;QObject::connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));loop.exec();
 QUrl next=reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();fprintf(stdout,"public_fixture case=%d status=%d error=%d bytes=%d next_host=%s next_path=%s self=%d\n",i,reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(),int(reply->error()),reply->readAll().size(),next.host().toAscii().constData(),next.path().toAscii().constData(),next==url?1:0);delete reply;
 }
 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--gzip-probe") {
 QCoreApplication app(argc,argv);QByteArray body;QVariantMap headers;
 int rc=idNetworkConnector::SyncRequest_thread("https://passport.sinaimg.cn/js/yidun/v1/yidunsdk.min.js?n9compression=1",QList<QPair<QByteArray,QByteArray> >(),QByteArray(),0,headers,&body,false);
 QFile expected(QCoreApplication::applicationDirPath()+"/verification-sdk.js");bool ok=expected.open(QIODevice::ReadOnly) && rc==0 && body==expected.readAll();
 fprintf(stdout,"gzip_public_script_probe error=%d bytes=%d exact_match=%d\n",rc,body.size(),ok?1:0);return ok?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--verification-config-probe") {
 QApplication app(argc,argv);QString dir=QCoreApplication::applicationDirPath();QFile html(dir+"/verification.html"),polyfill(dir+"/core-js.min.js"),provider(dir+"/provider-loader.js");
 if(!html.open(QIODevice::ReadOnly)||!polyfill.open(QIODevice::ReadOnly)||!provider.open(QIODevice::ReadOnly))return 2;
 QString source=QString::fromUtf8(html.readAll());QRegExp inlineScript("<script>([\\s\\S]*)</script>");inlineScript.setMinimal(true);if(inlineScript.indexIn(source)<0)return 3;source=inlineScript.cap(1);
 source.replace("var id=decodeURIComponent(window.location.hash.substring(1));","var id='offline_fixture';");source.replace("document.head.appendChild(script);","");
 QNetworkAccessManager offline;offline.setNetworkAccessible(QNetworkAccessManager::NotAccessible);QWebPage page;page.setNetworkAccessManager(&offline);page.mainFrame()->setHtml("<html><head></head><body><p id='state'></p></body></html>",QUrl::fromLocalFile(dir+"/offline-fixture.html"));
 page.mainFrame()->evaluateJavaScript(QString::fromUtf8(polyfill.readAll()));page.mainFrame()->evaluateJavaScript(QString::fromUtf8(provider.readAll()));
 page.mainFrame()->evaluateJavaScript("window.fixtureOriginal=window.initNECaptcha;window.fixtureRequests=0;window.initNECaptcha.ResourceLoader.api=function(){window.fixtureRequests++;var pending=new window.fixtureOriginal.Thenable();pending.resolve(new Error('offline fixture stop'));return pending;};window.ydInit=function(c){window.fixtureFallback=c.defaultFallback===false;window.initNECaptcha({captchaId:'offline_fixture'},function(){},function(){window.fixtureErrorCallback=true;});};");
 QString bootstrap=page.mainFrame()->evaluateJavaScript("(function(){try{"+source+";startVerification();return 'started';}catch(e){return 'exception';}})()").toString();
 QEventLoop settle;QTimer::singleShot(500,&settle,SLOT(quit()));settle.exec();
 QString result=bootstrap=="started"?page.mainFrame()->evaluateJavaScript("window.initNECaptcha===window.fixtureOriginal&&window.fixtureOriginal._plugins&&window.fixtureOriginal._plugins.length===4&&window.fixtureRequests===1&&window.fixtureErrorCallback===true&&window.fixtureFallback?'ok':'failed identity='+String(window.initNECaptcha===window.fixtureOriginal)+' plugins='+String(window.fixtureOriginal._plugins?window.fixtureOriginal._plugins.length:-1)+' requests='+String(window.fixtureRequests)+' fallback='+String(window.fixtureFallback)").toString():bootstrap;
 fprintf(stdout,"offline_real_loader_probe result=%s network_disabled=%d\n",result.toAscii().constData(),offline.networkAccessible()==QNetworkAccessManager::NotAccessible?1:0);return result=="ok"?0:1;
 }
 if(argc==2 && QByteArray(argv[1])=="--cached-scripts-probe") {
 QApplication app(argc,argv);QDir dir("/home/user/.cache/weibomm-static");
 foreach(const QString &name,dir.entryList(QStringList()<<"*.bin",QDir::Files)){
 QFile file(dir.filePath(name));if(!file.open(QIODevice::ReadOnly)||file.size()>1500000)continue;
 QDataStream stream(&file);stream.setVersion(QDataStream::Qt_4_7);quint32 magic; qint64 created,expires;QByteArray type,body;
 stream>>magic>>created>>expires>>type>>body;if(magic!=0x4e395301||stream.status()!=QDataStream::Ok)continue;
 QJson::Serializer serializer;QString payload=QString::fromUtf8(serializer.serialize(QVariant(QString::fromUtf8(body))));payload.replace(QChar(0x2028),"\\u2028");payload.replace(QChar(0x2029),"\\u2029");
 QWebPage page;page.mainFrame()->setHtml("<html><head></head><body></body></html>");QCoreApplication::processEvents();
 QString answer=page.mainFrame()->evaluateJavaScript("try {new Function("+payload+");'ok'} catch(e){String(e.name)+':'+String(e.message)}").toString();
 fprintf(stdout,"public_script %s bytes=%d parse=%s\n",name.toAscii().constData(),body.size(),answer.toUtf8().constData());
 }
 return 0;
 }
 if(argc==2 && QByteArray(argv[1])=="--login-config-probe") { QCoreApplication app(argc,argv);int rc=probeLoginV2();fprintf(stdout,"v2_config_probe result=%d\n",rc);return rc; }
 if(argc==2 && QByteArray(argv[1])=="--verification-engine-probe") {
 QApplication app(argc,argv);VerificationProbePage page;
 QString dir=QCoreApplication::applicationDirPath();QFile polyfill(dir+"/core-js.min.js"),sdk(dir+"/verification-sdk.js");
 if(!polyfill.open(QIODevice::ReadOnly)||!sdk.open(QIODevice::ReadOnly))return 2;
 page.mainFrame()->setHtml("<html><head></head><body></body></html>");
 page.mainFrame()->evaluateJavaScript(QString::fromUtf8(polyfill.readAll()));
 page.mainFrame()->evaluateJavaScript(QString::fromUtf8(sdk.readAll()));
 diagnostic("verification_types "+page.mainFrame()->evaluateJavaScript("typeof Symbol + ',' + typeof Symbol.iterator + ',' + typeof window.ydInit").toString());
 bool ok=page.mainFrame()->evaluateJavaScript("typeof window.ydInit === 'function' && typeof Symbol.iterator !== 'undefined'").toBool();
 diagnostic(QString("verification_engine_probe ok=%1").arg(ok?1:0));fprintf(stdout,"verification_engine_probe result=%d\n",ok?0:1);
 qmlRegisterType<QDeclarativeWebView>("karin.weibomm",1,0,"KarinWebView");
 QDeclarativeEngine engine;engine.setNetworkAccessManagerFactory(&factory);engine.rootContext()->setContextProperty("loginModel",id_loginModel::Instance());
 QDeclarativeComponent component(&engine,QUrl::fromLocalFile(dir+"/VerificationView.qml"));QObject *item=component.create();bool qmlOk=item!=0;
 diagnostic(QString("verification_qml_probe ok=%1").arg(qmlOk?1:0));fprintf(stdout,"verification_qml_probe result=%d\n",qmlOk?0:1);bool viewportOk=item!=0;bool touchOk=false;
 if(item){
 item->setProperty("width",480);
 QDeclarativeWebView *view=item->findChild<QDeclarativeWebView*>("verificationBrowser");
 const int heights[]={720,360,720};
 for(int i=0;i<3;++i){item->setProperty("height",heights[i]);QCoreApplication::processEvents();
 if(!view){viewportOk=false;break;}view->ConfigureVerificationViewport();
 if(view->page()->viewportSize()!=QSize(qRound(view->width()/view->contentsScale()),qRound(view->height()/view->contentsScale())))viewportOk=false;}
 touchOk=probeOfflineTouch(qobject_cast<QDeclarativeItem*>(item),view);
 item->setProperty("largeView",false);QCoreApplication::processEvents();view->ConfigureVerificationViewport();touchOk=probeOfflineTouch(qobject_cast<QDeclarativeItem*>(item),view) && touchOk;
 }
 diagnostic(QString("verification_viewport_probe ok=%1").arg(viewportOk?1:0));fprintf(stdout,"verification_viewport_probe result=%d\n",viewportOk?0:1);delete item;
 VerificationProbePage onlinePage;idNetworkAccessManager network;
 onlinePage.setNetworkAccessManager(&network);
 onlinePage.settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
 QEventLoop loop;QTimer timeout;timeout.setSingleShot(true);
 QObject::connect(&onlinePage,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
 QObject::connect(&timeout,SIGNAL(timeout()),&loop,SLOT(quit()));timeout.start(30000);
 QString html="<html><head><script src='"+QUrl::fromLocalFile(dir+"/core-js.min.js").toString()+"'></script><script src='https://passport.sinaimg.cn/js/yidun/v1/yidunsdk.min.js'></script><script src='https://cstaticdun.126.net/load.min.js'></script></head><body></body></html>";
 onlinePage.mainFrame()->setHtml(html,QUrl::fromLocalFile(dir+"/verification-probe.html"));loop.exec();
 bool onlineOk=onlinePage.mainFrame()->evaluateJavaScript("typeof window.ydInit === 'function' && typeof window.initNECaptcha === 'function'").toBool();
 diagnostic(QString("verification_online_probe ok=%1").arg(onlineOk?1:0));fprintf(stdout,"verification_online_probe result=%d\n",onlineOk?0:1);
 bool smsUiOk=false;QFile loginFile(dir+"/SinaWeiboLoginPage.qml");
 if(loginFile.open(QIODevice::ReadOnly)){
 QDeclarativeComponent loginComponent(&engine);loginComponent.setData(loginFile.readAll(),QUrl::fromLocalFile("/opt/weibomm/qml/SinaWeiboClient/SinaWeiboLoginPage.qml"));QObject *loginPage=loginComponent.create();
 foreach(const QDeclarativeError &error,loginComponent.errors())diagnostic("sms_ui_error "+error.description().left(200));
 if(loginPage){QObject *field=loginPage->findChild<QObject*>("smsCodeField");QObject *send=loginPage->findChild<QObject*>("smsSendButton");
 loginPage->setProperty("visible",true);loginPage->setProperty("smsMode",true);QCoreApplication::processEvents();
 diagnostic(QString("sms_ui_state field=%1 send=%2 echo=%3 visible=%4").arg(field?1:0).arg(send?1:0).arg(field?field->property("echoMode").toInt():-1).arg(send?send->property("visible").toBool():false));
 smsUiOk=field && send && field->property("echoMode").toInt()==0 && send->property("visible").toBool();
 loginPage->setProperty("smsMode",false);QCoreApplication::processEvents();smsUiOk=smsUiOk && field->property("echoMode").toInt()==2 && !send->property("visible").toBool();delete loginPage;}
 }
 diagnostic(QString("sms_ui_probe ok=%1").arg(smsUiOk?1:0));fprintf(stdout,"sms_ui_probe result=%d\n",smsUiOk?0:1);
 QDeclarativeComponent sessionComponent(&engine,QUrl::fromLocalFile(dir+"/SessionSyncView.qml"));QObject *sessionItem=sessionComponent.create();bool sessionUiOk=sessionItem!=0;
 if(sessionItem){QDeclarativeWebView *v=sessionItem->findChild<QDeclarativeWebView*>("sessionSyncBrowser");if(v){v->page()->mainFrame()->setHtml("<html><body>Offline session fixture</body></html>");QCoreApplication::processEvents();bool uaOk=v->page()->mainFrame()->evaluateJavaScript("navigator.userAgent").toString()=="WeiboMM-N9/2026 Qt/4.7.4";diagnostic(QString("sso_user_agent_probe ok=%1").arg(uaOk?1:0));sessionUiOk=sessionUiOk && uaOk;}else sessionUiOk=false;}
 delete sessionItem;
 QVariantMap anonymous=checkLoginSession(&engine);bool sessionGateOk=anonymous.value("_session_state_checked").toBool() && anonymous.value("retcode").toInt()!=20000000;
 diagnostic(QString("session_ui_probe ok=%1 gate=%2").arg(sessionUiOk?1:0).arg(sessionGateOk?1:0));fprintf(stdout,"session_ui_probe result=%d gate=%d\n",sessionUiOk?0:1,sessionGateOk?0:1);
 fprintf(stdout,"offline_touch_probe result=%d\n",touchOk?0:1);
 return ok && qmlOk && viewportOk && touchOk && onlineOk && smsUiOk && sessionUiOk && sessionGateOk?0:1;
 }
 launchStageClock.start();startupProfileMode=argc==2&&QByteArray(argv[1])=="--startup-profile";startupStage("main_enter");if(startupProfileMode)qInstallMsgHandler(offlineFixtureLog);
 QApplication *a;
	const QString RegisterUncreatableTypeMsg(QString("[ERROR]: %1 -> %2").arg("Can not create a single-instance object"));

	a = createApplication(argc, argv);
    startupStage("qapplication_ready");

	QScopedPointer<QApplication> app(a);
	a->setApplicationName(startupProfileMode?"weibomm-startup-profile":ID_PKG);
	a->setApplicationVersion(ID_VER);
	a->setOrganizationName(startupProfileMode?"weibomm-diagnostics":ID_DEV);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	qmlRegisterType<QDeclarativeWebView>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_DEVELOPER "WebView");
	qmlRegisterType<QDeclarativeWebSettings>();
#ifdef _KARIN_MM_EXTENSIONS
	qmlRegisterType<QDeclarativeVideo>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_DEVELOPER "Video");
#endif
	qmlRegisterUncreatableType<idNetworkConnector>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_DEVELOPER "NetworkConnector", RegisterUncreatableTypeMsg.arg("idNetworkConnector"));

	qmlRegisterType<id_contentCommentModel>(QML_MODULE_NAME,QML_MODULE_MAJOR,QML_MODULE_MINOR,"CommentModel");
	// sina weibo
	qmlRegisterType<id_FavoritesModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "FavoritesModel");
	qmlRegisterType<id_FanListModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "FanListModel");
	qmlRegisterType<id_FriendListModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "FriendListModel");
	qmlRegisterType<id_MyWeiboListModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "MyWeiboListModel");
	qmlRegisterType<id_TrendTopicModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "TrendTopicModel");
	qmlRegisterType<id_WeiboProfileModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "WeiboProfileModel");
	qmlRegisterType<id_DetailModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "DetailModel");
	qmlRegisterType<id_DeleteModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "DeleteModel");
	qmlRegisterType<id_TrendBlogModel>(QML_MODULE_NAME, QML_MODULE_MAJOR, QML_MODULE_MINOR, "TrendBlogModel");

	ut = idUtility::Instance();
	connector = idNetworkConnector::Instance();
	qt_funcView = id_qt_funcView::Instance();
	settingModel = id_settingModel::Instance();
	newMsgNotification = id_newMsgNotification::Instance();
	accountModel = id_accountModel::Instance();
	loginModel = id_loginModel::Instance();
	profile = id_profile::Instance();
	commentToMeModel = id_commentToMeModel::Instance();
	messageCenterModel = id_messageCenterModel::Instance();
	atMeModel = id_atMeModel::Instance();
	updateProfileModel = id_updateProfileModel::Instance();
	emotionlist = id_emotionlist::Instance();
	firstPageModel = id_firstPageModel::Instance();
	searchBlogModel = id_searchBlogModel::Instance();
	searchUserModel = id_searchUserModel::Instance();
	searchContactModel = id_searchContactModel::Instance();
	searchTopicModel = id_searchTopicModel::Instance();
	suiBianKanModel = id_suiBianKanModel::Instance();
	microBlog = id_microBlog::Instance();
	insertTopicModel = id_insertTopicModel::Instance();
	hotTrendModel = id_hotTrendModel::Instance();
	contentCommentModel = id_contentCommentModel::Instance();
	frequentContactModel = id_frequentContactModel::Instance();
	pipeline = idPipeline::Create(qApp);

	inited = true;
    startupStage("models_ready");
	id::create_qml_viewer();
    if(startupProfileMode)QTimer::singleShot(2200,qApp,SLOT(quit()));

	int result=app->exec();
    if(activeDatabase)activeDatabase->Disconnect();
    if(startupProfileMode)startupStage("profile_exit");
    return result;
}

// QML model registrations depend on the complete model class layout.
