#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *) 0x6000000;

void drawImage3(int r, int c, int width, int height, const u16* image) {
	for (int row = 0; row < height; row++) {
		DMA[3].src = &image[OFFSET(row, 0, width)];
		DMA[3].dst = &videoBuffer[OFFSET(row + r, c, 240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
	}
}

void setPixel(int r, int c, u16 color) {
	videoBuffer[OFFSET(r, c, 240)] = color;
}

void drawChar(int r, int c, char ch, u16 color) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 6; col++) {
			if (fontdata_6x8[OFFSET(row, col, 6) + ch * 48]) {
				setPixel(row + r, col + c, color);
			}
		}
	}
}

void drawString(int r, int c, char *str, u16 color)
{
	while (*str) {
		drawChar(r, c, *str++, color);
		c += 6;
	}
}

void drawRectangle(int r, int c, int width, int height, u16 color) {
	for (int row = 0; row < height; row++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(row + r, c, 240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT;
	}
}


void waitForVblank() {
	while (SCANLINECOUNTER > 160);
	while (SCANLINECOUNTER < 160);
}