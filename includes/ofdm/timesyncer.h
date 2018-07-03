
#ifndef	__TIMESYNCER__
#define	__TIMESYNCER__

#include	"dab-constants.h"

#define	TIMESYNC_ESTABLISHED	0100
#define	NO_DIP_FOUND		0101
#define	NO_END_OF_DIP_FOUND	0102

class	sampleReader;

class	timeSyncer {
public:
	timeSyncer	(sampleReader *mr);
	~timeSyncer	(void);
int	sync		(int, int);
private:
	sampleReader	*myReader;
	int32_t         syncBufferIndex = 0;
const	int32_t         syncBufferSize  = 4096;
};
#endif

