#
#ifndef	__UP_FILTER__
#define	__UP_FILTER__

#include	<complex>
#include	<vector>
#include	<math.h>


class	upFilter {
	std::vector<std::complex<float>> kernel;
	std::vector<std::complex<float>> buffer;
	int		ip;
	int		order;
	int		bufferSize;
	int		multiplier;
public:
	upFilter	(int, int, int);
	~upFilter	();
void	Filter	(std::complex<float>, std::complex<float> *);
};
#endif


	
