
#ifndef	__ALARM_SELECTOR__
#define	__ALARM_SELECTOR__
#
#include	<QDialog>
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QStringList>
#include	<cstdint>

class	alarmSelector: public QDialog {
Q_OBJECT
public:
			alarmSelector	();
			~alarmSelector	();
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

