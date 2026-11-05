#include <QFileInfo>
#include <QDir>
#include "browsersession.h"
#include "loginmodel.h"

#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include "qjson/parser.h"

#include "networkconnector.h"
#include "networkmanager.h"
#include "crypt.h"
#include "qmlapplicationviewer.h"
#include "api.h"
#include "authv2.h"
#include "pendinglogin.h"
#include "diagnostic.h"
extern QmlApplicationViewer *qml_viewer;
#include "utility.h"
#include "accountmodel.h"
#include "weiboprofilemodel.h"

#define TEST_USERNAME ""
#define TEST_PASSWORD ""

#define ID_HTTP

#define PRELOGIN_URL "https://login.sina.com.cn/sso/prelogin.php"
#define PIN_URL "http://login.sina.com.cn/cgi/pin.php"
#define LOGIN_URL ID_HTTP "http://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.19)"
#define PAGEREFER_URL "http://login.sina.com.cn/sso/logout.php?entry=miniblog&r=http%3A%2F%2Fweibo.com%2Flogout.php%3Fbackurl%3D%252F"
#define LOGIN_PARAMS_URL "https://www.weibo.com/ajaxlogin.php?framelogin=1&callback=parent.sinaSSOController.feedBackUrlCallBack"
#define SSOLOGIN_URL "https://i.sso.sina.com.cn/js/ssologin.js"
#define LOGIN_PARAMS_SUDAREF_URL LOGIN_PARAMS_URL "&sudaref=www.weibo.com"
#define PASSPORT_URL "http://passport.weibo.com/wbsso/login"

id_loginModel::id_loginModel(QObject *parent)
	: QObject(parent),
    m_loginName(TEST_USERNAME),
    m_loginPassword(TEST_PASSWORD),
	m_needToSave(true),
	m_timeout(60),
	m_preloginTime(0),
	m_state(id_loginModel::LoginState_Ready)
{
	setObjectName("id_loginModel");
}

id_loginModel::~id_loginModel()
{
	ID_QOBJECT_DESTROY_DBG
}

QString id_loginModel::UserID() const
{
	return m_userID;
}

void id_loginModel::SetUserID(const QString &str)
{
	if(m_userID != str)
	{
		m_userID = str;
		emit userIDChanged(m_userID);
	}
}

QString id_loginModel::LoginName() const
{
	return m_loginName;
}

void id_loginModel::SetLoginName(const QString &str)
{
	if(m_loginName != str)
	{
		m_loginName = str;
		emit loginNameChanged(m_loginName);
	}
}

QString id_loginModel::LoginPassword() const
{
	return m_loginPassword;
}

void id_loginModel::SetLoginPassword(const QString &str)
{
	if(m_loginPassword != str)
	{
		m_loginPassword = str;
		emit loginPasswordChanged(m_loginPassword);
	}
}

bool id_loginModel::NeedToSave() const
{
	return m_needToSave;
}

void id_loginModel::SetNeedToSave(bool b)
{
	if(m_needToSave != b)
	{
		m_needToSave = b;
		emit needToSaveChanged(m_needToSave);
	}
}

QString id_loginModel::ProfileImageUrl() const
{
	return m_profileImageUrl;
}

void id_loginModel::SetProfileImageUrl(const QString &str)
{
	if(m_profileImageUrl != str)
	{
		m_profileImageUrl = str;
		emit profileImageUrlChanged(m_profileImageUrl);
	}
}

QString id_loginModel::NickName() const
{
	return m_nickName;
}

void id_loginModel::SetNickName(const QString &str)
{
	if(m_nickName != str)
	{
		m_nickName = str;
		emit nickNameChanged(m_nickName);
	}
}

void id_loginModel::clearData()
{
	SetUserID("");
    SetLoginName(TEST_USERNAME);
    SetLoginPassword(TEST_PASSWORD);
	SetNeedToSave(true);
	SetProfileImageUrl("");
	SetNickName("");

	m_preloginTime = 0;
	m_preloginResult.clear();
	m_loginData.clear();
	SetData(QVariant());
	SetState(id_loginModel::LoginState_Ready);
}

void id_loginModel::SetState(idLoginState_e state)
{
	if(m_state != state)
	{
		m_state = state;
		emit loginStateChanged(m_state);
	}
}

void id_loginModel::SetData(const QVariant &data)
{
	if(m_data != data)
	{
		m_data = data;
		emit loginDataChanged(m_data);
	}
}

QVariant id_loginModel::LoginData() const
{
	return m_data;
}

int id_loginModel::LoginState() const
{
	return m_state;
}

QByteArray id_loginModel::EncodeUserName() const
{
	return QByteArray().append(m_loginName).toBase64();
}

QByteArray id_loginModel::EncryptPassword(qint64 servertime, const QString &nonce, const QString &pubkey)
{
#define __PK_E "10001" // hex
	QString secret = QString("%1\t%2\n%3").arg(servertime).arg(nonce).arg(m_loginPassword);
	return idCrypt::Encrypt(secret, pubkey, __PK_E);

#undef __PK_E
}

int id_loginModel::JSONP(QByteArray &r, const QByteArray &data)
{
	bool ok;
	QString str(data);
	QRegExp p("\\S+\\((.*)\\)");

	ok = p.exactMatch(str);
	if(p.captureCount() > 0)
	{
		r = QByteArray().append(p.capturedTexts()[1]);
		return 0;
	}
	return -1;
}

void id_loginModel::login_desktop(const QString &uname, const QString &psw, const QString &pin)
{
	extern QmlApplicationViewer *qml_viewer;
	idNetworkConnector connector;
	int ret;
	QByteArray data;
	QByteArray tmp;
	QUrl url;
	QJson::Parser parser;
	bool ok;
	QVariant json;
	QByteArray encrypted_pwd;
	QList<QPair<QByteArray, QByteArray> > querys;
	QVariantMap headers;
	QByteArray un;

	ret = -1;
	if(!qml_viewer)
		goto __Error;

	if(m_loginName != uname || m_loginPassword != psw)
	{
		SetLoginName(uname);
		SetLoginPassword(psw);
		SetState(id_loginModel::LoginState_Ready);
	}

	emit getloginResult(-1);
	connector.SetEngine(qml_viewer->engine());
	un = EncodeUserName();
	headers.insert("Content-Type", "application/x-www-form-urlencoded");

	if(!pin.isEmpty())
		goto __Pin;
	else
		goto __Prelogin;

__Prelogin:
	querys << qMakePair<QByteArray, QByteArray>("_", QByteArray::number(QDateTime::currentMSecsSinceEpoch()))
		<< qMakePair<QByteArray, QByteArray>("callback", "sinaSSOController.preloginCallBack")
		<< qMakePair<QByteArray, QByteArray>("checkpin", QByteArray::number(1))
		<< qMakePair<QByteArray, QByteArray>("client", "ssologin.js(v1.4.19)")
		<< qMakePair<QByteArray, QByteArray>("entry", "weibo")
		<< qMakePair<QByteArray, QByteArray>("rsakt", "mod")
		<< qMakePair<QByteArray, QByteArray>("su", un);
	m_preloginTime = QDateTime::currentMSecsSinceEpoch();
	ret = connector.SyncRequest(PRELOGIN_URL, querys, QByteArray(), idNetworkConnector::Connect_Get, QVariant(), &data);
	ID_REQ_ERR("prelogin")
	JSONP(tmp, data);
	json = parser.parse(tmp, &ok);
	ID_JSON_ERR("prelogin")
	m_preloginResult = json.toMap();

	m_preloginTime = QDateTime::currentMSecsSinceEpoch() - m_preloginTime - (m_preloginResult.contains("exectime") ? m_preloginResult["exectime"].toInt() : 0);
	encrypted_pwd = EncryptPassword(m_preloginResult["servertime"].toLongLong(), m_preloginResult["nonce"].toByteArray(), m_preloginResult["pubkey"].toByteArray());

	m_loginData.clear();
	m_loginData.insert("encoding", "UTF-8");
	m_loginData.insert("entry", "weibo");
	m_loginData.insert("from", "");
	m_loginData.insert("gateway", 1);
	m_loginData.insert("pagerefer", PAGEREFER_URL);
	m_loginData.insert("prelt", m_preloginTime);
	m_loginData.insert("pwencode", "rsa2");
	m_loginData.insert("qrcode_flag", "false");
	m_loginData.insert("returntype", "TEXT");
	m_loginData.insert("savestate", 7);
	m_loginData.insert("service", "miniblog");
	m_loginData.insert("sr", "1280*720");
	m_loginData.insert("su", un);
	m_loginData.insert("url", LOGIN_PARAMS_URL);
	m_loginData.insert("useticket", 1);
	m_loginData.insert("vsnf", 1);
	m_loginData.insert("nonce", m_preloginResult["nonce"]);
	m_loginData.insert("rsakv", m_preloginResult["rsakv"]);
	m_loginData.insert("servertime", m_preloginResult["servertime"]);
	m_loginData.insert("sp", encrypted_pwd);

	if(m_preloginResult["showpin"].toInt() == 1)
	{
		querys.clear();
		querys << qMakePair<QByteArray, QByteArray>("p", m_preloginResult["pcid"].toByteArray())
			<< qMakePair<QByteArray, QByteArray>("r", QByteArray::number(rand() % (100000 - 10000) + 10000))
			<< qMakePair<QByteArray, QByteArray>("s", "0");
		QUrl url(PIN_URL);
		url.setEncodedQueryItems(querys);
		qDebug() << "Need to input captcha" << url;

		SetData(QVariant::fromValue(url));
		SetState(id_loginModel::LoginState_Pin);
		emit getloginResult(id_loginModel::LoginState_Pin);
		goto __Exit;
	}
	else
		goto __Login;

__Pin:
	m_loginData.insert("door", pin);
	m_loginData.insert("cduit", 2);
	m_loginData.insert("pcid", m_preloginResult["pcid"]);
	m_loginData.insert("prelt", m_preloginTime);

__Login:
	ret = connector.SyncRequest(LOGIN_URL, QList<QPair<QByteArray, QByteArray> >(), idNetworkConnector::MakePostData(m_loginData), idNetworkConnector::Connect_Post, headers, &data);
	ID_REQ_ERR("login")
	json = parser.parse(data, &ok);
	ID_JSON_ERR("login")
	m_loginResult = json.toMap();

	if(m_loginResult["retcode"].toString() == "0")
	{
		ret = connector.SyncRequest(SSOLOGIN_URL, QList<QPair<QByteArray, QByteArray> >(), QByteArray(), idNetworkConnector::Connect_Get);
		querys.clear();
		querys << qMakePair<QByteArray, QByteArray>("callback", "sinaSSOController.doCrossDomainCallback")
			<< qMakePair<QByteArray, QByteArray>("client", "ssologin.js(v1.4.19)")
			<< qMakePair<QByteArray, QByteArray>("display", "0")
			<< qMakePair<QByteArray, QByteArray>("ticket", m_loginResult["ticket"].toByteArray())
			<< qMakePair<QByteArray, QByteArray>("retcode", "0")
			<< qMakePair<QByteArray, QByteArray>("ssosavestate", "7")
			<< qMakePair<QByteArray, QByteArray>("url", LOGIN_PARAMS_SUDAREF_URL);

		ret = connector.SyncRequest(PASSPORT_URL, querys, QByteArray(), idNetworkConnector::Connect_Get);
		ID_REQ_ERR("passport")
		ret = connector.SyncRequest(LOGIN_PARAMS_SUDAREF_URL, QList<QPair<QByteArray, QByteArray> >(), QByteArray(), idNetworkConnector::Connect_Get);
		ID_REQ_ERR("get_cookie")

		SetUserID(m_loginResult["uid"].toString());
		SetNickName(m_loginResult["nick"].toString());
		goto __Success;
	}
	else
	{
		SetState(id_loginModel::LoginState_Fail);
		SetData(m_loginResult["reason"]);

		querys.clear();
		querys << qMakePair<QByteArray, QByteArray>("p", m_preloginResult["pcid"].toByteArray())
			<< qMakePair<QByteArray, QByteArray>("r", QByteArray::number(rand() % (100000 - 10000) + 10000))
			<< qMakePair<QByteArray, QByteArray>("s", "0");
		QUrl url(PIN_URL);
		url.setEncodedQueryItems(querys);

		SetData(QVariant::fromValue(url));
		emit getloginResult(m_loginResult["retcode"].toInt());
		goto __Exit;
	}

__Error:
	SetData(ret);
	SetState(id_loginModel::LoginState_Fail);
	emit getloginResult(id_loginModel::LoginState_Fail);
	return;

__Success:
	SetData(m_nickName);
	SetState(id_loginModel::LoginState_Success);
	emit getloginResult(id_loginModel::LoginState_Success);
	return;

__Exit:
	return;
}

void id_loginModel::login_mobile(const QString &uname, const QString &psw)
{
	int ret;
	QVariantMap params;

	emit getloginResult(-1);
	SetLoginName(uname);
	SetLoginPassword(psw);
	SetState(id_loginModel::LoginState_Ready);

	params.insert("username", m_loginName);
	params.insert("password", m_loginPassword);
	params.insert("savestate", m_needToSave || 1 ? 1 : 0);
 params.insert("action",property("nextLoginAction").toString().isEmpty()?QString("password"):property("nextLoginAction"));setProperty("nextLoginAction",QVariant());
	if(property("verificationPassed").toBool()) {
 params.insert("verification_id",m_loginData.value("_mfa_id"));params.insert("verification_config",m_loginData.value("_mfa_config"));setProperty("verificationPassed",false);
 }
 m_loginData = idAPI::Login(params, &ret);emit verificationChanged();emit sessionSyncChanged();
	if(ret != 0)
	{
		goto __Error;
	}

	if(!SessionSyncUrl().isEmpty()){SetState(LoginState_Ready);SetData(QString::fromUtf8("正在完成登录"));emit getloginResult(202);return;}
 if(m_loginData.value("_sms_sent").toBool()){SetLoginPassword(QString());SetState(LoginState_Ready);SetData(QString::fromUtf8("短信已发送，请输入验证码"));emit getloginResult(201);return;}
 ret = m_loginData["retcode"].toInt();
	if(ret != 20000000)
		goto __Error;
	else
	{
		SetUserID(m_loginData["data"].toMap()["uid"].toString());
		id_WeiboProfileModel profile;
		profile.getSinaWeiboProfile(m_userID);
		SetNickName(profile.Nick());
		SetProfileImageUrl(profile.UserIcon());
		goto __Success;
	}

__Error:
	SetData(m_loginData.value("_diagnostic", QString("Login failed")));
	SetState(id_loginModel::LoginState_Fail);
	emit getloginResult(id_loginModel::LoginState_Fail);
	return;

__Success:
	SetData(m_userID);
	SetState(id_loginModel::LoginState_Success);
	emit getloginResult(id_loginModel::LoginState_Success);
	return;

__Exit:
	return;
}

void id_loginModel::login(const QString &uname, const QString &psw, const QString &pin)
{
	if(0)
		login_desktop(uname, psw, pin);
	else
        login_mobile(uname, psw);

    if(m_state == id_loginModel::LoginState_Success) {SetLoginPassword(QString());id_accountModel::Instance()->UpdateAccountFromLoginModel(this);}
}

void id_loginModel::Restore(const QVariantMap &data)
{
	QByteArray cookie = data["cookie"].toByteArray();
	QString name = data["login_name"].toString();
	QString psw = data["login_password"].toString();

	clearData();

	if(cookie.isEmpty())
	{
        login(name, psw);
        /*
        if(m_state == id_loginModel::LoginState_Success)
            id_accountModel::Instance()->UpdateAccountFromLoginModel(this);
            */
	}
	else
	{
		SetUserID(data["user_id"].toString());
		SetLoginName(data["login_name"].toString());
		SetLoginPassword(data["login_password"].toString());
		SetNickName(data["nick_name"].toString());
		SetProfileImageUrl(data["profile_image_url"].toString());
		idNetworkCookieJar::Instance()->SetAllCookies(QNetworkCookie::parseCookies(data["cookie"].toByteArray()));

		SetState(id_loginModel::LoginState_Success);
		id_accountModel::Instance()->UpdateAccountFromLoginModel(this);
	}
}

ID_SINGLE_INSTANCE_DECL(id_loginModel)

QString id_loginModel::VerificationId() const {return m_loginData.value("_mfa_id").toString();}
void id_loginModel::completeVerification() {
 if(VerificationId().isEmpty())return;
 setProperty("verificationPassed",true);setProperty("nextLoginAction",m_loginData.value("_action","password"));QString name=m_loginName,password=m_loginPassword;
 login(name,password);password.fill(QChar(0));
}
void id_loginModel::cancelVerification() {m_loginData.remove("_mfa_id");m_loginData.remove("_mfa_config");m_loginData.remove("_session_url");SetLoginPassword(QString());emit verificationChanged();emit sessionSyncChanged();}

static QString smsPhone(QString phone){phone=phone.trimmed();phone.remove(' ');phone.remove('-');if(phone.startsWith("+86"))phone.remove(0,3);else if(phone.startsWith('+'))phone="00"+phone.mid(1);return phone;}
void id_loginModel::sendLoginSms(const QString &phone){
 QString number=smsPhone(phone);if(!QRegExp("^[0-9]{5,20}$").exactMatch(number)){SetData(QString::fromUtf8("请输入正确的手机号"));emit getloginResult(9999);return;}
 setProperty("nextLoginAction","sms_send");login_mobile(number,QString());
}
void id_loginModel::loginSms(const QString &phone,const QString &code){
 if(!QRegExp("^[0-9]{5,20}$").exactMatch(smsPhone(phone))){SetData(QString::fromUtf8("请输入正确的手机号"));emit getloginResult(9999);return;}
 if(!QRegExp("^[0-9]{4,10}$").exactMatch(code.trimmed())){SetData(QString::fromUtf8("请输入短信验证码"));emit getloginResult(9999);return;}
 setProperty("nextLoginAction","sms_login");login(smsPhone(phone),code.trimmed());
}

QString id_loginModel::SessionSyncUrl() const {return m_loginData.value("_session_url").toString();}
bool id_loginModel::hasPendingLogin(){return !PendingLogin::load().isEmpty();}
bool id_loginModel::resumePendingLogin(){
 QVariantMap saved=PendingLogin::load();if(saved.isEmpty()||!PendingLogin::restore(saved))return false;
 m_loginData.clear();m_loginData.insert("retcode",202);m_loginData.insert("_session_url",saved.value("url"));SetLoginPassword(QString());SetState(LoginState_Ready);emit verificationChanged();emit sessionSyncChanged();emit getloginResult(202);return true;
}
bool id_loginModel::checkSessionSync() {
 if(SessionSyncUrl().isEmpty())return false;
 QVariantMap result=checkLoginSession(qml_viewer->engine());
 if(result.value("retcode").toInt()!=20000000){diagnostic("session_check authenticated=0");return false;}
 QVariantMap data=result.value("data").toMap();SetUserID(data.value("uid").toString());
 SetNickName(data.value("nick").toString().isEmpty()?UserID():data.value("nick").toString());SetProfileImageUrl(data.value("avatar").toString());
 id_WeiboProfileModel ownProfile;ownProfile.getSinaWeiboProfile(UserID());if(ownProfile.Result()){SetNickName(ownProfile.Nick());SetProfileImageUrl(ownProfile.UserIcon());}
 SetLoginPassword(QString());m_loginData=result;SetState(LoginState_Success);SetData(UserID());
 id_accountModel::Instance()->UpdateAccountFromLoginModel(this);
 PendingLogin::clear();diagnostic("session_check authenticated=1");emit verificationChanged();emit sessionSyncChanged();emit getloginResult(200);return true;
}

void id_loginModel::reportVerificationDiagnostic(const QString &stage,int code,int millis){
 const QStringList allowed=QStringList()<<"sdk_loading"<<"provider_loading"<<"initializing"<<"ready"<<"failed"<<"closed"<<"script_error"<<"load_error"<<"verified";
 if(!allowed.contains(stage))return;
 diagnostic(QString("verification_stage name=%1 code=%2 millis=%3").arg(stage).arg(code>=-999999 && code<=999999?code:0).arg(millis>=0 && millis<=120000?millis:0));
}

bool id_loginModel::importBrowserSession(const QString &text){
 QByteArray raw=text.toUtf8();QList<QNetworkCookie> imported;bool parsed=parseBrowserSession(raw,&imported);raw.fill(0);
 if(!parsed){emit browserSessionImported(9999,QString::fromUtf8("格式不正确。请只粘贴 m.weibo.cn 请求头中的 Cookie 字段，不要粘贴网页、JSON 或整段请求；长度不能超过 16 KB。"));return false;}
 idNetworkCookieJar *jar=idNetworkCookieJar::Instance();QList<QNetworkCookie> original=jar->GetAllCookies();emit browserSessionImported(-1,QString::fromUtf8("正在检查登录状态…"));jar->SetAllCookies(QList<QNetworkCookie>());
 bool accepted=jar->setCookiesFromUrl(imported,QUrl("https://m.weibo.cn/"));imported.clear();QVariantMap result=accepted?checkLoginSession(qml_viewer->engine()):QVariantMap();
 if(result.value("retcode").toInt()!=20000000){jar->SetAllCookies(original);emit browserSessionImported(9999,QString::fromUtf8("会话未通过验证。请确认电脑已在 m.weibo.cn 登录，并重新复制完整的 Cookie 字段；会话也可能已失效或被限制迁移。"));return false;}
 QVariantMap data=result.value("data").toMap();QString uid=data.value("uid").toString(),name="session:"+uid;id_accountModel *accounts=id_accountModel::Instance();accounts->getAccountList();for(int i=0;i<accounts->getAccountCount();i++)if(accounts->getAccountId(i)==uid){name=accounts->getAccountLoginName(i);break;}
 cancelVerification();PendingLogin::clear();SetLoginName(name);SetLoginPassword(QString());SetUserID(uid);SetNickName(data.value("nick").toString().isEmpty()?uid:data.value("nick").toString());SetProfileImageUrl(data.value("avatar").toString());id_WeiboProfileModel person;person.getSinaWeiboProfile(uid);if(person.Result()){SetNickName(person.Nick());SetProfileImageUrl(person.UserIcon());}
 SetState(LoginState_Success);SetData(uid);accounts->UpdateAccountFromLoginModel(this);original.clear();emit browserSessionImported(200,QString::fromUtf8("登录成功"));return true;
}

static bool sessionFilePath(const QString &path,QString *canonical){
 QString root=QFileInfo("/home/user/MyDocs").canonicalFilePath();if(root.isEmpty())return false;QString resolved=QFileInfo(path).canonicalFilePath();if(resolved.isEmpty()||(resolved!=root&&!resolved.startsWith(root+"/")))return false;*canonical=resolved;return true;
}
QVariantMap id_loginModel::browserSessionFiles(const QString &folder){
 QVariantMap result;QString path;QString target=folder.isEmpty()?QString("/home/user/MyDocs"):folder;if(!sessionFilePath(target,&path)||!QFileInfo(path).isDir())return result;
 QString base=QFileInfo("/home/user/MyDocs").canonicalFilePath();QDir dir(path);QFileInfoList files=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot|QDir::NoSymLinks,QDir::DirsFirst|QDir::Name|QDir::IgnoreCase);QVariantList entries;
 foreach(const QFileInfo &info,files){if(!info.isDir()&&info.suffix().toLower()!="txt")continue;QVariantMap item;item.insert("name",info.fileName());item.insert("path",info.absoluteFilePath());item.insert("directory",info.isDir());entries<<item;if(entries.size()>=500)break;}
 result.insert("folder",path);result.insert("parent",path==base?QString():QFileInfo(path).absolutePath());result.insert("entries",entries);return result;
}
bool id_loginModel::importBrowserSessionFile(const QString &file){
 QString path;QFileInfo info(file);if(!sessionFilePath(file,&path)||!info.isFile()||info.isSymLink()||info.suffix().toLower()!="txt"||info.size()>16384||info.size()==0){emit browserSessionImported(9999,QString::fromUtf8("请选择文档目录内不超过 16 KB 的 TXT 会话文件。"));return false;}
 QFile input(path);if(!input.open(QIODevice::ReadOnly)){emit browserSessionImported(9999,QString::fromUtf8("无法读取文件，请确认手机没有处于 USB 存储模式。"));return false;}QByteArray bytes=input.read(16385);input.close();if(bytes.startsWith("\xEF\xBB\xBF"))bytes.remove(0,3);QString text=QString::fromUtf8(bytes);bytes.fill(0);bool ok=importBrowserSession(text);text.fill(QChar(0));return ok;
}
