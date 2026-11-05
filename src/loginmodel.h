#ifndef _KARIN_LOGINMODEL_H
#define _KARIN_LOGINMODEL_H

#include <QObject>

#include "id_std.h"

class id_loginModel : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QString userID READ UserID WRITE SetUserID NOTIFY userIDChanged)
		Q_PROPERTY(QString loginName READ LoginName WRITE SetLoginName NOTIFY loginNameChanged)
		Q_PROPERTY(QString loginPassword READ LoginPassword WRITE SetLoginPassword NOTIFY loginPasswordChanged)
		Q_PROPERTY(bool needToSave READ NeedToSave WRITE SetNeedToSave NOTIFY needToSaveChanged)
		Q_PROPERTY(QString profileImageUrl READ ProfileImageUrl WRITE SetProfileImageUrl NOTIFY profileImageUrlChanged)
		Q_PROPERTY(QString nickName READ NickName WRITE SetNickName NOTIFY nickNameChanged)
		Q_PROPERTY(QVariant loginData READ LoginData NOTIFY loginDataChanged)
		Q_PROPERTY(QString sessionSyncUrl READ SessionSyncUrl NOTIFY sessionSyncChanged)
		Q_PROPERTY(QString verificationId READ VerificationId NOTIFY verificationChanged)
		Q_PROPERTY(int loginState READ LoginState NOTIFY loginStateChanged)

	public:
		enum idLoginState_e
		{
			LoginState_Ready = 0,
			LoginState_Fail = 9999,
			LoginState_Pin = 2070,
			LoginState_Success = 200,
			LoginState_Invalid_Login_Data = 403, // 202
			LoginState_Network_Error = 99, // 3
		};
		Q_ENUMS(idLoginState_e)

	public:
		virtual ~id_loginModel();
		ID_SINGLE_INSTANCE_DEF(id_loginModel)
			QString UserID() const;
		void SetUserID(const QString &str);
		QString LoginName() const;
		void SetLoginName(const QString &str);
		QString LoginPassword() const;
		void SetLoginPassword(const QString &str);
		bool NeedToSave() const;
		void SetNeedToSave(bool b);
		QString ProfileImageUrl() const;
		void SetProfileImageUrl(const QString &str);
		QString NickName() const;
		void SetNickName(const QString &str);
		QVariant LoginData() const;
		int LoginState() const;
        QString VerificationId() const;
        QString SessionSyncUrl() const;
		void Restore(const QVariantMap &data);

Q_SIGNALS:
		void getloginResult(const QVariant &errCode);
        void browserSessionImported(int code,const QString &message);
		void verificationChanged();
        void sessionSyncChanged();
		void userIDChanged(const QString &userID);
		void loginNameChanged(const QString &loginName);
		void loginPasswordChanged(const QString &oginPasswor);
		void needToSaveChanged(bool needToSave);
		void profileImageUrlChanged(const QString &profileImageUrl);
		void nickNameChanged(const QString &nickName);
		void loginStateChanged(int loginState);
		void loginDataChanged(const QVariant &loginData);

		public Q_SLOTS:
			void clearData();
        bool importBrowserSession(const QString &text);
        QVariantMap browserSessionFiles(const QString &folder);
        bool importBrowserSessionFile(const QString &file);
        void completeVerification();
        bool checkSessionSync();
        bool hasPendingLogin();
        bool resumePendingLogin();
        void reportVerificationDiagnostic(const QString &,int,int);
        void sendLoginSms(const QString &phone);
        void loginSms(const QString &phone,const QString &code);
        void cancelVerification();
		void login_desktop(const QString &uname, const QString &psw, const QString &pin = QString());
		void login_mobile(const QString &uname, const QString &psw);
		void login(const QString &uname, const QString &psw, const QString &pin = QString());

	private:
		QString m_userID;
		QString m_loginName;
		QString m_loginPassword;
		bool m_needToSave;
		QString m_profileImageUrl;
		QString m_nickName;
		int m_timeout;
		qint64 m_preloginTime;

		QVariantMap m_preloginResult;
		QVariantMap m_loginData;
		QVariantMap m_loginResult;
		QVariant m_data;
		idLoginState_e m_state;

	private:
		void SetState(idLoginState_e state);
		void SetData(const QVariant &data);
		QByteArray EncodeUserName() const;
		QByteArray EncryptPassword(qint64 servertime, const QString &nonce, const QString &pubkey);
		int JSONP(QByteArray &r, const QByteArray &data);
		explicit id_loginModel(QObject *parent = 0);
		Q_DISABLE_COPY(id_loginModel)

};

#endif
