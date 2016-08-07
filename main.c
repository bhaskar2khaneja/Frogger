#include "myLib.h"
#include "gameStartImage.h"
#include "gameOverImage.h"
#include "gameWinImage.h"
#include "frog.h"
#include "car.h"
#include "house.h"
#include "river.h"
#include <stdlib.h>
#include <stdio.h>

#define NUMCARS 3
#define NUMHOUSES 3

int isCollision(int obj1Row, int obj1Col, int obj1Size, int obj2Row, int obj2Col, int obj2Size);

enum GameState {
	START,
	START_NODRAW,
	PLAY,
	WIN,
	LOSE
};

typedef struct {
	int row;
	int col;
	int size;
} FROG;

typedef struct {
	int row;
	int col;
	int cdel;
	int size;
} CAR;

typedef struct {
	int row;
	int col;
	int size;
	int isOccupied;
} HOUSE;

int main() {

	REG_DISPCTL = MODE3 | BG2_ENABLE;
	enum GameState state = START;
	u16 bgcolor = BLACK;
	int dels[] = {1, 2, 3};
	int numdels = sizeof(dels) / (sizeof(dels[0]));
	int livesLeft = 3;
	int housesCovered = 0;
	int numVblanksPassed = 0;
	char timeBuffer[2];
	int timeLeft = 60;

	FROG frogObj = {150, 120, FROG_WIDTH};
	int frogOldRow = 150;
	int frogOldCol = 120;
	HOUSE houses[NUMHOUSES];
	CAR cars[NUMCARS];
	CAR oldCars[NUMCARS];

	for (int i = 0; i < NUMCARS; i++) {
		cars[i].row =  35 * (i + 1) + 5 * i;
		cars[i].col = 0;
		cars[i].cdel = dels[rand() % numdels];
		cars[i].size = CAR_WIDTH;
		oldCars[i] = cars[i];
	}

	for (int i = 0; i < NUMHOUSES; i++) {
		houses[i].row = 10;
		houses[i].col = (40 * (i + 1)) + (25 * i);
		houses[i].size = HOUSE_WIDTH;
		houses[i].isOccupied = 0;
	}

	while(1) {

		for (int i = 0; i < NUMCARS; i++) {
			cars[i].col += cars[i].cdel;
			if (cars[i].col >= 239 - cars[i].size + 1) {
				cars[i].col = 0;
				cars[i].cdel = dels[rand() % numdels];
			} 
		}

		if (KEY_DOWN_NOW(BUTTON_DOWN)) {
			frogObj.row++;
			if (frogObj.row > 159 - frogObj.size + 1) {
				frogObj.row = 159 - frogObj.size + 1;
			}
		} 
		if (KEY_DOWN_NOW(BUTTON_UP)) {
			frogObj.row--;
			if (frogObj.row < 30) {
				int unoccupiedHouseFound = 0;
				int occupiedHouseFound = 0;
				for (int i = 0; i < NUMHOUSES && !unoccupiedHouseFound && !occupiedHouseFound; i++) {
					if (isCollision(frogObj.row, frogObj.col, frogObj.size, houses[i].row, houses[i].col, houses[i].size)) {
						if (!houses[i].isOccupied) {
							unoccupiedHouseFound = 1;
							houses[i].isOccupied = 1;
							housesCovered++;
							frogObj.row = 150;
							frogObj.col = 120;
						} else {
							occupiedHouseFound = 1;
						}
					}
				}
				if (occupiedHouseFound) {
					frogObj.row = 30;
				}
				if (!unoccupiedHouseFound && !occupiedHouseFound) {
					livesLeft--;
					frogObj.row = 150;
					frogObj.col = 120;
				}
			}
		} 
		if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
			frogObj.col++;
			if (frogObj.col > 239 - frogObj.size + 1) {
				frogObj.col = 239 - frogObj.size + 1;
			}
		} 
		if (KEY_DOWN_NOW(BUTTON_LEFT)) {
			frogObj.col--;
			if (frogObj.col < 0) {
				frogObj.col = 0;
			}
		}
		
		int collisionFound = 0;
		for (int i = 0; i < NUMCARS && !collisionFound; i++) {
			if (isCollision(cars[i].row, cars[i].col, cars[i].size, frogObj.row, frogObj.col, frogObj.size)) {
					livesLeft--;
					frogObj.row = 150;
					frogObj.col = 120;
					collisionFound = 1;
			}
		}

		waitForVblank();
		switch(state) {

			case START:
				livesLeft = 3;
				housesCovered = 0;
				timeLeft = 60;
				numVblanksPassed = 0;
				frogObj.row = 150;
				frogObj.col = 120;
				for (int i = 0; i < NUMHOUSES; i++) {
					houses[i].isOccupied = 0;
				}
				drawImage3(0, 0, GAMESTARTIMAGE_WIDTH, GAMESTARTIMAGE_HEIGHT, gameStartImage);
				drawString(140, 60, "Press Start to play!", YELLOW);
				if (KEY_DOWN_NOW(BUTTON_START)) {
					state = START_NODRAW;
				}
				break;

			case START_NODRAW:
				drawRectangle(0, 0, 240, 30, CYAN);
				drawString(0, 0, "Lives Left: ", BLACK); 
				drawString(0, 165, "Time Left: ", BLACK);
				drawRectangle(30, 0, 240, 130, BLACK);
				state = PLAY;
				break;

			case PLAY:
				drawRectangle(0, 11 * 6, 6, 8, CYAN);
				drawChar(0, 11 * 6, livesLeft + 48, BLACK);
				sprintf(timeBuffer, "%d", timeLeft);
				drawRectangle(0, 225, 6 * 2, 8, CYAN);
				drawString(0, 225, timeBuffer, BLACK);
				for (int i = 0; i < NUMHOUSES; i++) {
					drawImage3(houses[i].row, houses[i].col, HOUSE_WIDTH, HOUSE_HEIGHT, house);
					if (houses[i].isOccupied) {
						drawImage3(houses[i].row + 10, houses[i].col + 10, frogObj.size, frogObj.size, frog);
					}
				}

				drawRectangle(frogOldRow, frogOldCol, frogObj.size, frogObj.size, bgcolor);
				drawImage3(frogObj.row, frogObj.col, frogObj.size, frogObj.size, frog);
				frogOldRow = frogObj.row;
				frogOldCol = frogObj.col;

				for (int i = 0; i < NUMCARS; i++) {
					drawRectangle(oldCars[i].row, oldCars[i].col, oldCars[i].size, oldCars[i].size, bgcolor);
				}
				for (int i = 0; i < NUMCARS; i++) {
					drawImage3(cars[i].row, cars[i].col, cars[i].size, cars[i].size, car);
					oldCars[i] = cars[i];
				}

				if (livesLeft == 0 || numVblanksPassed >= 3500) {
					state = LOSE;
				}
				if (housesCovered == 3) {
					state = WIN;
				}
				numVblanksPassed++;
				if (numVblanksPassed % 60 == 0) {
					timeLeft--;
				}
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				break;

			case WIN:
				drawImage3(0, 0, GAMEWINIMAGE_WIDTH, GAMEWINIMAGE_HEIGHT, gameWinImage);
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				break;

			case LOSE:
				drawImage3(0, 0, GAMEOVERIMAGE_WIDTH, GAMEOVERIMAGE_HEIGHT, gameOverImage);
				drawString(140, 60, "SELECT to play again!", RED);
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				break;

		}

	}
	while(1);
	return 0;

}

int isCollision(int obj1Row, int obj1Col, int obj1Size, int obj2Row, int obj2Col, int obj2Size) {
	if ((obj1Col + obj1Size - 1 >= obj2Col) &&
		(obj1Col <= obj2Col + obj2Size - 1) &&
		(obj1Row + obj1Size - 1 >= obj2Row) &&
		(obj1Row <= obj2Row + obj2Size - 1)) {
		return 1;
	}
	return 0;
}