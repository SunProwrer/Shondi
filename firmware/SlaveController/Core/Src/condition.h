/*
 * Conditional.h
 *
 *  Created on: Mar 2, 2025
 *      Author: tissa
 */

#ifndef SRC_CONDITION_H_
#define SRC_CONDITION_H_

struct Condition {
	uint16_t speed : 10;
	uint16_t brake : 10;
	uint8_t left : 1;
	uint8_t right : 1;
};

#endif /* SRC_CONDITION_H_ */
