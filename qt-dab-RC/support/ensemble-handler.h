
#pragma once
#include	<stdio.h>
#include	<stdint.h>
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QTableWidgetItem>
#include	<QFile>
#include	<QDataStream>
#include	"dab-constants.h"

class	QSettings;
class	RadioInterface;

#define	SHOW_ENSEMBLE	0
#define	SHOW_PRESETS	1

class	ensembleHandler: public QTableWidget {
Q_OBJECT
public:
		ensembleHandler		(RadioInterface *parent,
	                                 QSettings *ensembleSettings,
	                                 const QString & favoritesFile);
		~ensembleHandler	();
	void	reset			();
	bool	add_to_ensemble		(serviceId &);
	void	add_favorite_from_scanList	(const QString &);

	void	handle_scheduledSelect	(const QString &, const QString &);
	QStringList	getSelectables	();
	QStringList	get_epgServices	();
	int	get_serviceCount	();
	void	selectPrevService	();
	void	selectNextService	();

	void	reportStart		(const QString &);
	void	setMode			(bool);
	int	get_showMode		();
	void	set_showMode		(int);

	int	nrFavorites		();
	bool	hasFavorite		(const QString &);
	uint16_t extract_SId		(const QString &);

private	slots:
	void	click_on_service	(int, int);
public slots:
	void	handle_fontSelect	();
	void	handle_fontColorSelect	();
	void	handle_fontSizeSelect	(int);

	void	handle_rightMouseClick	(const QString &);
private:
	QFont		normalFont;
	QFont		markedFont;
	QFont		channelFont;
	QSettings	*ensembleSettings;
	QString		favFile;
	struct	service {
	   QString	name;
	   QString channel;
	   bool		selected;
	};

	std::vector<serviceId>	ensembleList;
	std::vector<struct service>	favorites;
	uint8_t	ensembleMode;
	bool	handlePresets;
	QString	fontColor;
	void	clearTable		();
	void	unSelect		();
	void	setFont			(int, int);
	void	loadFavorites		(const QString &);
	void	storeFavorites		(const QString &);
	void	updateList		();
	void	add_to_favorites	(const QString &, const QString &);
	void	remove_from_favorites	(const QString &);

	int	inEnsembleList		(const QString &);
	int	inFavorites		(const QString &);
	void	addRow			(const QString &, const QString &,
	                                 bool, int);
std::vector<serviceId>
		insert			(std::vector<serviceId> &,
	                                 const serviceId &, int);
signals:
	void	selectService		(const QString &, const QString &);
	void	start_background_task	(const QString &);
};


