#ifndef _KARIN_SETTINGMODEL_H
#define _KARIN_SETTINGMODEL_H

#include <QObject>

#include "id_std.h"

class QSettings;
class idDatabase;

class id_settingModel : public QObject
{
	Q_OBJECT
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY preferencesChanged)
    Q_PROPERTY(int fontSizeMode READ fontSizeMode WRITE setFontSizeMode NOTIFY preferencesChanged)
    Q_PROPERTY(int bodyTextSize READ bodyTextSize NOTIFY preferencesChanged)
    Q_PROPERTY(bool largeText READ largeText WRITE setLargeText NOTIFY preferencesChanged)
    Q_PROPERTY(int imageMode READ imageMode WRITE setImageMode NOTIFY preferencesChanged)
    Q_PROPERTY(bool autoImages READ autoImages WRITE setAutoImages NOTIFY preferencesChanged)
    Q_PROPERTY(bool latestComments READ latestComments WRITE setLatestComments NOTIFY preferencesChanged)
    Q_PROPERTY(bool portraitLocked READ portraitLocked WRITE setPortraitLocked NOTIFY portraitLockedChanged)
		Q_PROPERTY(int readMode READ ReadMode WRITE SetReadMode NOTIFY readModeChanged)

	public:
		virtual ~id_settingModel();
		ID_SINGLE_INSTANCE_DEF(id_settingModel)

			void SetReadMode(int m);
		int ReadMode() const;
		static const QString TableName;

		template <class T> void SetSettingv(const QString &name, const T &value);
		template <class T> T GetSettingv(const QString &name);
	void SetSetting(const QString &name, const QVariant &value);
	QVariant GetSetting(const QString &name);
		
public Q_SLOTS:
		bool darkMode()const;void setDarkMode(bool value);
        int fontSizeMode()const;void setFontSizeMode(int value);
        int bodyTextSize()const;
        bool largeText()const; void setLargeText(bool value);
        int imageMode()const;void setImageMode(int value);
        bool autoImages()const; void setAutoImages(bool value);
        bool latestComments()const; void setLatestComments(bool value);
        bool readAudioMode() const;
        bool portraitLocked() const;
        void setPortraitLocked(bool locked);
		void saveAudioMode(bool on);
		void saveReadMode(int m);
		QVariantList loadRecentTopic(const QString &userID);
		void saveRecentTopic(const QString &userID, const QString &topic);
		
Q_SIGNALS:
		void preferencesChanged();
        void readModeChanged(int readMode);
        void portraitLockedChanged();

	private:
		QSettings *m_settings;
		idDatabase *m_db;
		int m_readMode;
		bool m_audioMode;

		explicit id_settingModel(QObject *parent = 0);
		Q_DISABLE_COPY(id_settingModel)

};

template <class T> 
void id_settingModel::SetSettingv(const QString &name, const T &value)
{
	SetSetting(name, QVariant(value));
}

template <class T> 
T id_settingModel::GetSettingv(const QString &name)
{
	QVariant value = GetSetting(name);
	if(value.canConvert<T>())
		return value.value<T>();
	return T();
}
#endif
