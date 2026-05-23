#
#pragma once
#include	<QObject>
#include	<stdint.h>
#include	<QUdpSocket>

class tpegClient: public QObject {
Q_OBJECT
private:
	QUdpSocket	* socket;	
	int		port;
	void		process_frame_0	(const QByteArray &);
	void		process_frame_1	(const QByteArray &);
public:
		tpegClient	(int port);
		~tpegClient	();
public slots:
	void	readyRead ();
};
