
#ifndef	__SCHEDULE_SELECTOR__
#define	__SCHEDULE_SELECTOR__
#
#include	<QDialog>
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QStringList>
#include	<cstdint>

class	scheduleSelector: public QDialog {
Q_OBJECT
public:
			scheduleSelector	();
			~scheduleSelector	();
	void		addtoList		(const QString &);
private:
	QLabel		*toptext;
	QListView	*servicesDisplay;
	QStringListModel	serviceList;
	QStringList	services;
	int16_t		selectedItem;
private slots:
	void		selectService	(QModelIndex);
};

#endif

