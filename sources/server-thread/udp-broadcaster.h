
#pragma once

#include	<QUdpSocket>
#include	<QByteArray>




class	udpBroadcaster: public QUdpSocket {
Q_OBJECT
public:
		udpBroadcaster	(int);
		~udpBroadcaster	();
	void	sendData	(QByteArray &);

private:
	int	port;
};

