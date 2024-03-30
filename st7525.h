/*
 * st7525.h
 *
 *  Created on: Mar 29, 2024
 *      Author: pablo-jean
 */

#ifndef DRIVERS_ST7525_ST7525_H_
#define DRIVERS_ST7525_ST7525_H_

/**
 * Includes
 */
#include <stdint.h>
#include <string.h>
#include <stddef.h>

/**
 * Macros
 */

#define ST7525_SUCCESS							0
#define ST7525_FAILED							1

#define ST7525_COLOR_WHITE						1
#define ST7525_COLOR_BLACK						0

#define ST7525_COLUMNS							192
#define ST7525_LINES							64
#define ST7525_PAGES							( ST7525_LINES/8 )

#define ST7525_SET_FXN_LOCK(handle, func)		handle.fxn.Lock = func;
#define ST7525_SET_FXN_UNLOCK(handle, func)		handle.fxn.Unock = func;
#define ST7525_SET_FXN_CSPIN(handle, func)		handle.fxn.CSPin = func;
#define ST7525_SET_FXN_A0Pin(handle, func)		handle.fxn.A0Pin = func;
#define ST7525_SET_FXN_RSTPin(handle, func)		handle.fxn.RSTPin = func;
#define ST7525_SET_FXN_DelayUs(handle, func)	handle.fxn.DelayUs = func;
#define ST7525_SET_FXN_TXDATA(handle, func)		handle.fxn.TxData = func;

#define ST7525_CMD_SET_COLUMN_LSB				0x00
#define ST7525_CMD_SET_COLUMN_MSB				0x10
#define ST7525_CMD_SET_SCROLL_LINE				0x40
#define ST7525_CMD_SET_PAGE_ADRESS				0xB0
#define ST7525_CMD_SET_CONTRAST					0X81
#define ST7525_CMD_SET_PARTIAL_SCREEN			0X84
#define ST7525_CMD_SET_RAM_ADDR_CTRL			0X88
#define ST7525_CMD_SET_FRAME_RATE				0xA0
#define ST7525_CMD_SET_ALL_PIXEL_ON				0xA4
#define ST7525_CMD_SET_INVERSE_DISPLAY			0xA5
#define ST7525_CMD_SET_DISPLAY_EN				0xAE
#define ST7525_CMD_SET_SCAN_DIR					0XC0
#define ST7525_CMD_SOFT_RESET					0xE2
#define ST7525_CMD_SET_BIAS						0xE4
#define ST7525_CMD_SET_COM_END					0xF1
#define ST7525_CMD_SET_PART_START_ADDR			0xF2
#define ST7525_CMD_SET_PART_END_ADDR			0xF3
#define ST7525_CMD_SET_TEST_CONTROL				0xF0
#define ST7525_CMD_NOP							0xE3

/**
 * Enum
 */

/**
 * Typedef and Structs
 */

typedef void 	(*Lock_f)(void);
typedef void 	(*Unlock_f)(void);
typedef void 	(*CSPin_f)(uint8_t Act);
typedef void 	(*A0Pin_f)(uint8_t Act);
typedef void 	(*RSTPin_f)(uint8_t Act);
typedef void	(*DelayUs_f)(uint32_t us);
typedef uint8_t (*TxData_f)(uint8_t *data, uint32_t len);

typedef struct{
	struct{
		Lock_f Lock;
		Unlock_f Unlock;
		CSPin_f CSPin;
		A0Pin_f A0Pin;
		RSTPin_f RSTPin;
		DelayUs_f DelayUs;
		TxData_f TxData;
	}fxn;
	uint8_t Pending;
	uint8_t FrameBuffer[ST7525_PAGES][ST7525_COLUMNS];
}st7525_t;

typedef struct{
	uint8_t X;
	uint8_t Y;
	uint8_t color;
}st7525_write_info_t;

/**
 * Function Prototypes
 */

void ST7525_init(st7525_t *ST7525);

void ST7525_write(st7525_t *ST7525, st7525_write_info_t *WriteInfo);

void ST7525_refresh(st7525_t *ST7525);

#endif /* DRIVERS_ST7525_ST7525_H_ */
