#include "nRF24L01P.h"

#define _NRF24L01P_REG_SETUP_RETR            0x04

/**
 * This functions is somehow missing in library version from
 * repository.
 */
void nRF24L01P::enableAutoRetransmit(int delay, int count) {
	delay = (0x00F0 & (delay << 4));
	count = (0x000F & count);

	setRegister(_NRF24L01P_REG_SETUP_RETR, delay|count);
}
