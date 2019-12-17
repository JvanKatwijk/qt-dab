

#ifndef	__CONTROL_QUEUE__
#define	__CONTROL_QUEUE__
#include	<queue>
#define	RESTART_REQUEST		0100
#define	STOP_REQUEST		0101
#define	FREQUENCY_REQUEST	0102
#define	AGC_REQUEST		0103
#define	GRDB_REQUEST		0104
#define	PPM_REQUEST		0105
#define	LNA_REQUEST		0106
#define	ANTENNASELECT_REQUEST	0107


#define	FREQUENCY_UPDATE_ERROR	0100
#define	AGC_UPDATE_ERROR	0101
#define	GRDB_UPDATE_ERROR	0102
#define	PPM_UPDATE_ERROR	0103
#define	LNA_UPDATE_ERROR	0104
#define	ANTENNA_UPDATE_ERROR	0105

class	controlQueue {
public:
	controlQueue	();
	~controlQueue	();
int	getHead		();
bool	isEmpty		();
int	size		();
void	add		(int);
void	add		(int, int);
void	add		(int, int, int);
private:
	std::queue<int> theQueue;
};

#endif

