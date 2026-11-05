#ifndef _KARIN_MICROBLOG_H
#define _KARIN_MICROBLOG_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QPointer>
class QNetworkReply;
#include <QGeoSearchReply>
#include <QGeoSearchManagerEngine>

#include "id_std.h"

QTM_USE_NAMESPACE

class id_microBlog : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QString lastError READ LastError NOTIFY lastErrorChanged)

	public:
		explicit id_microBlog(QObject *parent = 0);
		virtual ~id_microBlog();
		ID_SINGLE_INSTANCE_DEF(id_microBlog)

		public Q_SLOTS:
			QString LastError() const {return m_lastError;}
        QVariantMap toggleLike(const QString &statusId, bool currentlyLiked);
        bool requestLike(const QString &statusId, bool currentlyLiked);
        void reqGeoAddress(qreal latitude, qreal longitude);
		void handleTimeout();
		void addLocation(qreal latitude, qreal longitude); // UNUSED
		QString getGeoAddress();
		void removeLocation();
		void sendNewMicroBlog(const QString &content, const QString &selectImageSrc = QString());
		void sendCommentMicroBlog(const QString &statusId, const QString &content, bool checked);
		void sendCommentReply(const QString &commentStatusID, const QString &content, const QString &replyRetweetedId);
		void sendForwordMicroBlog(const QString &content, const QString &statusId, bool checked = false);

		void SendNewMicroBlog(const QString &content, const QStringList &selectImageSrc = QStringList());

Q_SIGNALS:
        void likeFinished(const QString &statusId, bool success, const QString &message);
		void lastErrorChanged();
        void readyFinishNew(int errCode); // UNUSED, using qml
		void getPosFromGPS(int errCode);
		void readyFinishComment(int errCode);
		void readyFinishForword(int errCode);
		void readyFinishCommentReply(int errCode);
		void readyFinishGeoAddress(int errCode);

		private Q_SLOTS:
		void LikeReplyFinished();
			void replyFinished_Slot();

	private:
        QMap<QString,QPointer<QNetworkReply> > m_likeReplies;
		QGeoSearchManagerEngine *m_engine;
		QGeoSearchReply *m_reply;
		QString m_address;
        QString m_lastError;
        void SetActionError(const QVariantMap &result, int transport);
};

#endif
