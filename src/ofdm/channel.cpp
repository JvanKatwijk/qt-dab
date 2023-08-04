
#include	"channel.h"

#ifdef	__WITH_JAN__
#include        <Eigen/QR>
#include        <Eigen/Dense>

static inline
complex<float> createExp (float s) {
        return complex<float> (cos (s), - sin (s));
}


	channel::channel	(std::vector<std::complex<float>> &refTable,
	                         int nrPilots, int nrTaps) {
	fftSize                 = 2048;
        numberofCarriers	= 1536;
        numberofPilots		= nrPilots;
        numberofTaps		= nrTaps;
        F_p                     = MatrixXd (numberofPilots, numberofTaps);
        S_p                     = MatrixXd (numberofPilots,
                                                  numberofPilots);
        S_pxF_p                 = MatrixXd (numberofPilots, numberofTaps);
//
        pilotTable. resize (numberofPilots);

	for (int i = 0; i < numberofPilots; i ++)
	   pilotTable [i] = i + 1;

	for (int row = 0; row < numberofPilots; row ++)
           for (int col = 0; col < numberofPilots; col ++)
              S_p (row, col) = std::complex<float> (0, 0);
        for (int index = 0; index < numberofPilots; index ++)
	   S_p (index, index) = refTable [pilotTable [index]];

	for (int pilotIndex = 0; pilotIndex < numberofPilots; pilotIndex ++) {
	   for (int tap = 0; tap < numberofTaps; tap ++)
	      F_p (pilotIndex, tap) =
	         cdiv (createExp (2 * M_PI *
	                        (fftSize / 2 + pilotTable [pilotIndex]) *
	                                          tap / fftSize), sqrt (fftSize));
	}
	S_pxF_p                 = S_p * F_p;
}

	channel::~channel	() {}

void	channel::estimate	(std::complex<float> *testRow,
	                         std::complex<float> *resultRow) {
Vector  h_td (numberofTaps);
Vector  H_fd (numberofPilots);
Vector  X_p  (numberofPilots);
//
        for (int index = 0; index < numberofPilots; index ++)
           X_p (index) = testRow [pilotTable [index]];
//
////    Ok, the matrices are filled, now computing the channelvalues
        h_td    = S_pxF_p. bdcSvd (ComputeThinU | ComputeThinV). solve (X_p);
        H_fd    = F_p * h_td;
//
        for (int index = 0; index < numberofPilots; index ++)
           resultRow [index] = h_td (index);
}
#endif

