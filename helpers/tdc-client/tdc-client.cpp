#
#include        <QApplication>

#include	"tdc-client.h"
#include	<unistd.h>

	tpegClient::tpegClient (int port) {
	(void)port;
	socket = new QUdpSocket ();
	bool b =  socket -> bind (QHostAddress::Any, 8888);
	if (!b)
	   fprintf (stderr, "cannot bind %s\n",
	                 socket -> errorString (). toLatin1 (). data ());
	else
	connect (socket, &QUdpSocket::readyRead,
	         this, &tpegClient::readyRead);
}

	tpegClient::~tpegClient	() {
	delete socket;
}

void	tpegClient::readyRead () {     //Read something
QByteArray Buffer;
	Buffer. resize (socket -> pendingDatagramSize ());
	QHostAddress sender;
	quint16 senderPort;
	socket -> readDatagram (Buffer. data (),
	                        Buffer. size (), &sender,&senderPort);
	uint8_t type	= Buffer [7];
	switch (type) {
	   case 0:
	      process_frame_0 (Buffer);
	      return;
	   case 0xFF:
	   default:
	      process_frame_1 (Buffer);
	      return;
	}
}

void	tpegClient::process_frame_0	(const QByteArray &buffer) {
int base	= 0;
	fprintf (stderr, " frame 0: nrServices %d, SID-A %d SID-B %d SID-C %d\n",
                          buffer [base], buffer [base + 1],
	                  buffer [base + 2], buffer [base + 3]);
}

void	tpegClient::process_frame_1	(const QByteArray &buffer) {
int base	= 8;
	fprintf (stderr, " frame 1: encryption %d\n", buffer [8 + 3]);
}

int	main	(int argc, char *argv []) {
	QCoreApplication a (argc, argv);

	tpegClient	*theReader  = new tpegClient (8888);

	return a.exec ();
}


