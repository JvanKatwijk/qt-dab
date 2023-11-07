
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

bool	audioPlayer::hasMissed		() {return false;}

