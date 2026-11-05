#ifndef _KARIN_FIRSTPAGEMODEL_H
#define _KARIN_FIRSTPAGEMODEL_H

#include "acclistmodel_base.h"
#include "id_std.h"

class id_firstPageModel : public id_accListModel_base
{
	Q_OBJECT
    Q_PROPERTY(bool recommended READ recommended NOTIFY feedModeChanged)

	public:
		virtual ~id_firstPageModel();
		ID_SINGLE_INSTANCE_DEF(id_firstPageModel)

		public Q_SLOTS:
		void getSinaContentFromModel(int type = 0);
        void setRecommended(bool value);
        bool recommended() const { return m_recommended; }
		QString getSinaStudusId(int index) const;
		void deleteItemByStatusId(const QString &statusId);

Q_SIGNALS:
		void getFriendsTimeLineResult(int errCode);
        void feedModeChanged();

	private:
		explicit id_firstPageModel(QObject *parent = 0);
        bool m_recommended;
        QString m_feedCursor;
};

#endif
