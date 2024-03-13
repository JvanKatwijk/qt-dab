
#include	"audio-player.h"
#include	<stdio.h>


	audioPlayer::audioPlayer	() {}
	audioPlayer::~audioPlayer	() {}

void	audioPlayer::audioOutput	(float *buffer, int amount) {
	fprintf (stderr, "You should not be here\n");
	(void)buffer; (void) amount;
}

void	audioPlayer::stop		() {}

void	audioPlayer::restart		() {}

void	audioPlayer::suspend		() {stop ();}

void	audioPlayer::resume		() {restart ();}

bool	audioPlayer::selectDevice	(int16_t k) {(void)k; return false;}

bool	audioPlayer::hasMissed		() {return false;}

int	audioPlayer::missed		() { return -1;}

