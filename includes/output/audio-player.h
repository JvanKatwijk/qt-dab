#pragma once

#include	<QObject>

class	audioPlayer: public QObject {
Q_OBJECT
public:
		audioPlayer	();
		~audioPlayer	();
virtual void	audioOutput	(float *, int);
virtual	void	stop		();
virtual	void	restart		();
virtual	bool	hasMissed	();
};

