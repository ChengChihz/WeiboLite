#ifndef _KARIN_CONTENTCOMMENTMODEL_H
#define _KARIN_CONTENTCOMMENTMODEL_H

#include "acclistmodel_base.h"
#include "id_std.h"

class id_contentCommentModel : public id_accListModel_base
{
	Q_OBJECT
 Q_PROPERTY(int previewLimit READ previewLimit WRITE setPreviewLimit)
 Q_PROPERTY(bool latest READ Latest WRITE setLatest NOTIFY orderChanged)
	public:
		virtual ~id_contentCommentModel();
        explicit id_contentCommentModel(QObject *parent = 0);
        Q_INVOKABLE QVariantList previewItems(int limit)const;
        int previewLimit()const{return m_previewLimit;}
        void setPreviewLimit(int value){m_previewLimit=qBound(0,value,15);}
		ID_SINGLE_INSTANCE_DEF(id_contentCommentModel)

			public Q_SLOTS:
		bool Latest() const {return m_latest;}
        void setLatest(bool value){if(m_latest!=value){m_latest=value;emit orderChanged();}}
        void getContentCommentFromModel(const QString &statusId, int type = 0);

Q_SIGNALS:
		void orderChanged();
        void getContentCommentResult(int errCode);

	protected:
		bool MakeModelData(QVariantMap &map, const QVariantMap &item);

	private:

		void SetMaxId(const QString &max = QString());

	private:
		QString m_maxId;
        bool m_latest;
        int m_previewLimit;
};

#endif

