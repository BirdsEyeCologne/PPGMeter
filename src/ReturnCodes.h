/*
 * ReturnCodes.h
 *
 *  Created on: Sep 28, 2019
 *      Author: fox
 */

#ifndef RETURNCODES_H_
#define RETURNCODES_H_

enum class RC : uint8_t {INIT, OK, FAIL, NC, ERROR};
using status_t = RC;


#endif /* RETURNCODES_H_ */
