

#include	"udp-broadcaster.h"
#include	<stdint.h>

	udpBroadcaster::udpBroadcaster	(int port):
	                                       QUdpSocket () {
	this	-> port	= 8888;
}

	udpBroadcaster::~udpBroadcaster	() {
}

void	udpBroadcaster::sendData (QByteArray &buffer) {
	int x = this -> writeDatagram ((char *)(buffer. data ()),
	                               buffer. size (),
	                               QHostAddress ("255.255.255.255"),
	                               port);
	(void)x;
}


