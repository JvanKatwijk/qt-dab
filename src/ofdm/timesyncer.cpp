
#include	"timesyncer.h"
#include	"sample-reader.h"

#define C_LEVEL_SIZE    50

	timeSyncer::timeSyncer (sampleReader *mr) {
	myReader	= mr;
}

	timeSyncer::~timeSyncer	(void) {}

int	timeSyncer::sync (int T_null, int T_F) {
float	cLevel		= 0;
int	counter		= 0;
float	envBuffer       [syncBufferSize];
const
int	syncBufferMask	= syncBufferSize - 1;
int	i;

	syncBufferIndex = 0;
	for (i = 0; i < C_LEVEL_SIZE; i ++) {
	   std::complex<float> sample        = myReader -> getSample (0);
	   envBuffer [syncBufferIndex]       = jan_abs (sample);
	   cLevel                            += envBuffer [syncBufferIndex];
	   syncBufferIndex ++;
	}
//SyncOnNull:
	counter      = 0;
	while (cLevel / C_LEVEL_SIZE  > 0.50 * myReader -> get_sLevel ()) {
	   std::complex<float> sample        =
	         myReader -> getSample (0);
//	         myReader. getSample (coarseOffset + fineCorrector);
	   envBuffer [syncBufferIndex] = jan_abs (sample);
//      update the levels
	   cLevel += envBuffer [syncBufferIndex] -
	        envBuffer [(syncBufferIndex - C_LEVEL_SIZE) & syncBufferMask];
	   syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
           counter ++;
           if (counter > T_F) { // hopeless
	      return NO_DIP_FOUND;
           }
        }
/**
  *     It seemed we found a dip that started app 65/100 * 50 samples earlier.
  *     We now start looking for the end of the null period.
  */
	counter      = 0;
//SyncOnEndNull:
	 while (cLevel / C_LEVEL_SIZE < 0.75 * myReader -> get_sLevel ()) {
	   std::complex<float> sample =
	           myReader -> getSample (0);
	   envBuffer [syncBufferIndex] = jan_abs (sample);
//      update the levels
	   cLevel += envBuffer [syncBufferIndex] -
	         envBuffer [(syncBufferIndex - C_LEVEL_SIZE) & syncBufferMask];
	   syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	   counter   ++;
	   if (counter > T_null + 50) { // hopeless
	      return NO_END_OF_DIP_FOUND;
	   }
	}

	return TIMESYNC_ESTABLISHED;
}
