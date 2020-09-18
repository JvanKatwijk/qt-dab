
#ifndef	__SI_PROCESSOR__
#define	__SI_PROCESSOR__

#include	<QString>
#include	<stdio.h>
#include	<stdint.h>
#include	<stdlib.h>
#include	<vector>

class	ensembleInfo;
class	serviceInfo;

class	siProcessor {
public:
		siProcessor		();
		~siProcessor		();
ensembleInfo	*process_SI		(uint8_t *, int);
private:
int		header			(int *pos);
ensembleInfo	*process_ensemble	(int *pos);
serviceInfo	process_service		(int *pos, int length);

int		process_serviceId	(int pos);
QString		process_serviceName	(int pos);
QString		process_mediaDescription (int pos);
QString		process_keywords	(int pos);
QString		longDescription		(int pos);

QString stringTable [30];
uint8_t	*data;
int	length;
};

#endif

