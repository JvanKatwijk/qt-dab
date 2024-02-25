
#pragma once

#include	<string>
#include	<stdint.h>

typedef struct {
	std::string	country;
	std::string	channel;
	std::string ensemble;
	uint16_t Eid;
	uint8_t mainId;
	uint8_t	subId;
	std::string transmitterName;
	float	latitude;
	float	longitude;
	float	power;
} cacheElement;

