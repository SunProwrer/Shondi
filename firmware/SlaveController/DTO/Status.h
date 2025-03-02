/*
 * Status.h
 *
 *  Created on: Mar 2, 2025
 *      Author: tissa
 */

#ifndef STATUS_H_
#define STATUS_H_

struct status {
	uint16_t speed : 10;
	uint16_t brake : 10;
	uint8_t left : 1;
	uint8_t right : 1;
};

#endif /* STATUS_H_ */
