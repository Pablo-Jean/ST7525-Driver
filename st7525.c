/*
 * st7525.c
 *
 *  Created on: Mar 29, 2024
 *      Author: pablo-jean
 */

#include "st7525.h"

/**
 * Macros
 */

#define _RST_ON			0
#define _RST_OFF		1
#define _CS_ON			0
#define _CS_OFF			1
#define _A0_CMD			0
#define _A0_DATA		1

/**
 * Privates
 */

static void _lock_interface(st7525_t *ST7525){
	if (ST7525->fxn.Lock != NULL){
		ST7525->fxn.Lock();
	}
}

static void _unlock_interface(st7525_t *ST7525){
	if (ST7525->fxn.Unlock != NULL){
		ST7525->fxn.Unlock();
	}
}

static void _cs_write(st7525_t *ST7525, uint8_t signal){
	ST7525->fxn.CSPin(signal);
}

static void _a0_write(st7525_t *ST7525, uint8_t Sig){
	ST7525->fxn.A0Pin(Sig);
}

static void _rst_write(st7525_t *ST7525, uint8_t Sig){
	ST7525->fxn.RSTPin(Sig);
}

static void _delay_us(st7525_t *ST7525, uint32_t delay){
	ST7525->fxn.DelayUs(delay);
}

static uint8_t _tx_data(st7525_t *ST7525, uint8_t *data, uint32_t len){
	return ST7525->fxn.TxData(data, len);
}

static uint8_t __command(st7525_t *ST7525, uint8_t cmd){
	uint8_t ret;
	_lock_interface(ST7525);

	_a0_write(ST7525, _A0_CMD);
	_cs_write(ST7525, _CS_ON);
	ret = _tx_data(ST7525, &cmd, 1);
	_cs_write(ST7525, _CS_OFF);

	_unlock_interface(ST7525);
	if (ret != ST7525_SUCCESS){
		return ST7525_FAILED;
	}
	return ST7525_SUCCESS;
}

static uint8_t __data(st7525_t *ST7525, uint8_t *data, uint32_t len){
	uint8_t ret;
	_lock_interface(ST7525);

	_a0_write(ST7525, _A0_DATA);
	_cs_write(ST7525, _CS_ON);
	ret = _tx_data(ST7525, data, len);
	_cs_write(ST7525, _CS_OFF);

	_unlock_interface(ST7525);
	if (ret != ST7525_SUCCESS){
		return ST7525_FAILED;
	}
	return ST7525_SUCCESS;
}

static void _set_address(st7525_t *ST7525, uint8_t page, uint8_t columns){
	uint8_t lowNibbleCol, highNibbleCol;

	lowNibbleCol = (columns & 0xF);
	highNibbleCol = ((columns >> 4) & 0xF);
	__command(ST7525, ST7525_CMD_SET_PAGE_ADRESS | page);
	__command(ST7525, ST7525_CMD_SET_COLUMN_MSB  | highNibbleCol);
	__command(ST7525, ST7525_CMD_SET_COLUMN_LSB  | lowNibbleCol);
}

/**
 * Publics
 */

void ST7525_init(st7525_t *ST7525){
	if (ST7525 == NULL){
		return;
	}

	memset(ST7525->FrameBuffer, 0, sizeof(ST7525->FrameBuffer));

	_cs_write(ST7525, _CS_OFF);
	_rst_write(ST7525, _RST_OFF);
	_delay_us(ST7525, 1000);
	_rst_write(ST7525, _RST_ON);
	_delay_us(ST7525, 2000);
	_rst_write(ST7525, _RST_OFF);
	_delay_us(ST7525, 5000);

//	__command(ST7525, ST7525_CMD_SOFT_RESET);
//	_delay_us(ST7525, 5000);
	// Set Frame rate for maximum
	__command(ST7525, ST7525_CMD_SET_FRAME_RATE | 0x3);
	// Set Bias to 1/9
	__command(ST7525, ST7525_CMD_SET_BIAS | 0x3);
	// Set scan direction to Normal
	__command(ST7525, ST7525_CMD_SET_SCAN_DIR | 0x0);
	// Set DDRAM Strategy
	__command(ST7525, ST7525_CMD_SET_RAM_ADDR_CTRL | 0x1);
	// Set Contrast to 0x79
	__command(ST7525, ST7525_CMD_SET_CONTRAST);
	__command(ST7525, 0x70);
	// Set Address
	ST7525->Pending = 1;
	_set_address(ST7525, 0, 0);
	ST7525_refresh(ST7525);
	// Eanbles the Dispaly
	__command(ST7525, ST7525_CMD_SET_DISPLAY_EN | 0x1);

}

void ST7525_refresh(st7525_t *ST7525){
	if (ST7525 == NULL){
		return;
	}

	if (ST7525->Pending == 0){
		// no refresh is pending on the display, just return
		return;
	}
	_set_address(ST7525, 0, 0);
	__data(ST7525, ST7525->FrameBuffer, ST7525_COLUMNS*ST7525_PAGES);
//	for (Page=0 ; Page < ST7525_PAGES ; Page++){
//		for (Col=0 ; Col < ST7525_COLUMNS ; Col++){
//		}
//	}
	ST7525->Pending = 0;
}

void ST7525_write(st7525_t *ST7525, st7525_write_info_t *WriteInfo){
	uint8_t Page, Line, Col;
	if (ST7525 == NULL || WriteInfo == NULL){
		return;
	}

	// On the HS19264 display mode, we have an offset of 15 units
	// in the Y axys
//	WriteInfo->Y = ( (WriteInfo->Y+49) % ST7525_LINES );
	Col = WriteInfo->X;
	Page = WriteInfo->Y/8;
	Line = WriteInfo->Y%8;
	if (WriteInfo->color == ST7525_COLOR_BLACK){
		ST7525->FrameBuffer[(Page*ST7525_COLUMNS) + Col] &= ~(1 << Line);
	}
	else{
		ST7525->FrameBuffer[(Page*ST7525_COLUMNS) + Col] |= (1 << Line);
	}
	ST7525->Pending = 1;
}

void ST7525_frame_rate(st7525_t *ST7525, st7525_frame_rate_e fr){
	if (ST7525 == NULL || fr >= ST7525_FR_LIMIT){
		return;
	}

	__command(ST7525, ST7525_CMD_SET_FRAME_RATE | fr);
}

