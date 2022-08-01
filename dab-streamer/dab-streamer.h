#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	rds encoding is heavily based on the "fmstation"
 *	from Jan Roemisch (github.com/maxx23), all rights acknowledged
 */

#ifndef __DAB_STREAMER_H
#define __DAB_STREAMER_H

#include	<thread>
#include	<stdint.h>
#include	<stdio.h>
#include	<complex>
#include	<atomic>
#include	"ringbuffer.h"
#include	"audio-base.h"
#include	"ringbuffer.h"
#include	"fir-filters.h"
#include	"bandpass-filter.h"
#include	<string>

class	plutoHandler;
#define RDS_AF_MAX		26
#define RDS_RT_MAX		256
#define RDS_PS_LEN		8
#define RDS_TP			(1 << 10)

#define RDS_GROUP_0A_TA		(1 << 4)
#define RDS_GROUP_0A_MUSIC	(1 << 3)
#define RDS_GROUP_0A_DI_EON	(1 << 0)
#define RDS_GROUP_0A_DI_COMP	(1 << 1)
#define RDS_GROUP_0A_DI_AHEAD	(1 << 2)
#define RDS_GROUP_0A_DI_STEREO	(1 << 3)

#define RDS_GROUP_TYPE(a, b)	((((a) & 0xf) << 1) | (b & 0x1))
#define RDS_GROUP(x)			(((x) & 0x1f) << 11)
#define RDS_PTY(x)				(((x) & 0x1f) << 5)

#define RDS_BLOCK_A			1
#define RDS_BLOCK_B			2
#define RDS_BLOCK_C			4
#define RDS_BLOCK_D			8

#define RDS_BLOCK_DATA_LEN	16
#define RDS_BLOCK_CRC_LEN	10
#define RDS_BLOCK_LEN		(RDS_BLOCK_DATA_LEN + RDS_BLOCK_CRC_LEN)

#define RDS_GROUP_NUMBLOCKS	4
#define RDS_GROUP_LEN		(RDS_BLOCK_LEN * RDS_GROUP_NUMBLOCKS)

#define RDS_TMC_AID		0xCD46
#define RDS_TMC_GROUP		RDS_GROUP_TYPE(8, 0)
#define RDS_TMC_T		(1 << 4)	/*  */
#define RDS_TMC_F		(1 << 3)	/*  */
#define RDS_TMC_DP(x)		((x) & 0x7)	/* duration and persistence */

#define RDS_TMC_D		(1 << 15)	/* diversion advice */
#define RDS_TMC_PN		(1 << 14)	/* direction */
#define RDS_TMC_EXTENT(x)	(((x) & 0x7) << 11)	/*  */
#define RDS_TMC_EVENT(x)	(x)		/* terrorist incident */
#define RDS_TMC_LOCATION(x)	(x)		/* Ilmenau-Ost */

#define RDS_TMC_VAR(x)	(((x) & 0x3) << 14)	/* Variant */

/* Variant 0 */
#define RDS_TMC_LTN(x)	(((x) & 0x3f) << 6)	/* Location Table Number */
#define RDS_TMC_AFI		(1 << 5)	/* AF Indicator */
#define RDS_TMC_M		(1 << 4)	/* Mode of Transmission */
/* if !M */
#define RDS_TMC_I		(1 << 3)	/* International */
#define RDS_TMC_N		(1 << 2)	/* National */
#define RDS_TMC_R		(1 << 1)	/* Regional */
#define RDS_TMC_U		(1 << 0)	/* Urban */

/* Variant 1 */
#define RDS_TMC_GAP(x)		(((x) & 0x3) << 12)	/* Gap parameter */
#define RDS_TMC_SID(x)		(((x) & 0x3f) << 6)	/* SID */
#define RDS_TMC_TA(x)		(((x) & 0x3) << 4)	/* Activity time */
#define RDS_TMC_TW(x)		(((x) & 0x3) << 2)	/* Window time */
#define RDS_TMC_TD(x)		(((x) & 0x3) << 0)	/* Delay time */

#define MANCH_ENCODE(clk, bit) \
	(((clk ^ bit) << 1) | ((clk ^ 0x1 ^ bit)))

#define NRZ(in) \
	(double)(((int)((in) & 0x1) << 1) - 1)

#define	index_A		0
#define	index_B		1
#define	index_C		2
#define	index_D		3

struct rds_info_s {
	uint16_t	pi;
	uint16_t	af [RDS_AF_MAX];
	uint16_t	di;
	char		pty;
	char		ps_name [RDS_PS_LEN];
	char		radiotext [RDS_RT_MAX];
};

struct rds_group_s {
	char	bits [RDS_GROUP_LEN];
	uint16_t blocks [RDS_GROUP_NUMBLOCKS];
	int info;
	struct rds_info_s *info_block;
};

typedef struct {
	float	leftChannel;
	float	rightChannel;
} floatSample;

class dabStreamer : public audioBase {
public:
		dabStreamer		(int, int, plutoHandler *);
		~dabStreamer		(void);
	void	audioOutput		(float *, int);
	void	addRds			(const std::string);
	void	addName			(const std::string);
	void	stop			(void);
private:
	LowPassFIR	lowPassFilter;
	BandPassFIR	lmrFilter;
	BandPassFIR	rdsFilter;
	RingBuffer<float> pcmBuffer;
	RingBuffer<char>  rdsBuffer;
	void		run			(void);
	std::thread	threadHandle;
	int		inRate;
	int		outRate;
	plutoHandler	*generator;
	std::complex<float>	*oscillatorTable;
	float		*sinTable;
	void		modulateData	(float *, int, int);
	std::atomic<bool>	running;
	std::atomic<bool>	messageIn;
	float		nextPhase;
	int		pos;
        char		m;
        char		e;
        char		prev_e;
        int		bpos;
        double		symclk_p;
        char		symclk_b;

	struct rds_group_s *group;
	struct rds_info_s rds_info;
	
	struct rds_group_s group_0a;
	struct rds_group_s group_2a;
	struct rds_group_s group_3a;
	struct rds_group_s group_8a;

	int		rt_pos;
	uint16_t rds_crc		(uint16_t);
	void	rds_bits_to_values	(char *, uint16_t, int);
	void	rds_serialize		(struct rds_group_s *, char);
	void	rds_init_groups		(struct rds_info_s *info);

	void	rds_group_0A_update	(void);
	void	rds_group_2A_update	(void);
	void	rds_group_3A_update	(void);
	void	rds_group_8A_update	(void);
	struct rds_group_s*
	   	rds_group_schedule	(void);
};

#endif

