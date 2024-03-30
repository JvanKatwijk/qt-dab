
#pragma once

#include	<qwt.h>
//
//	Peter told me that Qwt 6.2 had the option of using floats rather
//	than doubles for the input data. Helpful for the RSP's!!
//	The raster data is still "double"
#if (QWT_VERSION >> 8) < 0x0602
    using floatQwt = double;
#else
    using floatQwt = float;
#endif

