/*
 * Copyright (c) 2017 Ian Burgwin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "titles_jpn.h"
#include "titles_usa.h"
#include "titles_eur.h"
#include "titles_chn.h"
#include "titles_kor.h"
#include "titles_twn.h"

#define arraysizeof(x) (sizeof(x) / sizeof(x[0]))

typedef enum {
	STATE_NONE,
	STATE_MENU,
	STATE_LIST,
	STATE_DELETE,
	STATE_SAVE,
	STATE_TIK_LIST,
	STATE_TIK_DELETE,
	STATE_TIK_SAVE,
	STATE_CONTINUE,
	STATE_EXIT,
} state_t;

const char *regionIDs[]  = { // http://3dbrew.org/wiki/Nandrw/sys/SecureInfo_A
	"JPN",
	"USA",
	"EUR",
	"JPN", // "AUS"
	"CHN",
	"KOR",
	"TWN"
};

const u64 allowedTIDHigh[] = {
	0x00040010,
	0x0004001B,
	0x00040030,
	0x0004009B,
	0x000400DB,
};

u32 dumpList(u64 *list, u32 count, const char *filepath) {
	Result res;
	Handle fileHandle;
	u32 bytesWritten = 0;
	res = FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, filepath), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
	if (R_FAILED(res)) {
		printf("FSUSER_OpenFileDirectly failed: %lx\n", res);
		return 0;
	}

	res = FSFILE_Write(fileHandle, &bytesWritten, 0, list, count * sizeof(u64), 0);
	if (R_FAILED(res)) {
		printf("FSFILE_Write failed: %lx\n", res);
		FSFILE_Close(fileHandle);
		return 0;
	}
	FSFILE_Close(fileHandle);
	return bytesWritten;
}

bool inArray(u64 item, const u64 *list, size_t length) {
	for (int i = 0; i < length; i++) {
		if (list[i] == 0) break;
		else if (list[i] == item) return true;
	}
	return false;
}

void resetScreen(PrintConsole *screen) {
	consoleSelect(screen);
	consoleClear();
	printf("rc-nand-cleanup v%s by ihaveamac\n\n", VERSION);
}

int main(int argc, char **argv) {

	PrintConsole topScreen, bottomScreen;

	amInit();
	cfguInit();
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);

	consoleSelect(&topScreen);
	resetScreen(&topScreen);

	int i;
	bool isNew3DS;
	Result res;
	u32 kDown, bytesWritten;
	size_t titleIDList_count;
	u32 titleCount = 0, titlesRead = 0, toCheck_count = 0, toDelete_count = 0;
	u32 ticketCount = 0, ticketsRead = 0, tikToCheck_count = 0, tikToDelete_count = 0;
	u8 region;
	u64 *titleIDs = NULL, *toCheck = NULL, *toDelete = NULL;
	u64 *ticketIDs = NULL, *tikToCheck = NULL, *tikToDelete = NULL;
	const u64 *titleIDList = NULL;

	bool exiting = false;
	state_t currentState = STATE_NONE;
	state_t nextState = STATE_MENU;

	res = APT_CheckNew3DS(&isNew3DS);
	// better safe than sorry...
	if (R_FAILED(res)) {
		printf("APT_CheckNew3DS failed: 0x%lx\nSomething must have fucked up badly for this to happen...", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	res = CFGU_SecureInfoGetRegion(&region);
	if (R_FAILED(res)) {
		printf("CFGU_SecureInfoGetRegion failed: 0x%lx\n", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	switch (region) {
		case 0: titleIDList = titleIDs_jpn; titleIDList_count = arraysizeof(titleIDs_jpn); break;
		case 1: titleIDList = titleIDs_usa; titleIDList_count = arraysizeof(titleIDs_usa); break;
		case 2: titleIDList = titleIDs_eur; titleIDList_count = arraysizeof(titleIDs_eur); break;
		case 4: titleIDList = titleIDs_chn; titleIDList_count = arraysizeof(titleIDs_chn); break;
		case 5: titleIDList = titleIDs_kor; titleIDList_count = arraysizeof(titleIDs_kor); break;
		case 6: titleIDList = titleIDs_twn; titleIDList_count = arraysizeof(titleIDs_twn); break;
		default:
			printf("Region unknown (%u). This is bad.\n", region);
			nextState = STATE_EXIT;
			goto mainLoop;
	}

	// titles
	res = AM_GetTitleCount(MEDIATYPE_NAND, &titleCount);
	if (R_FAILED(res)) {
		printf("AM_GetTitleCount failed: 0x%lx\n", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	titleIDs = calloc(titleCount, sizeof(u64));
	if (!titleIDs) {
		puts("Failed to allocate memory for title IDs");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	res = AM_GetTitleList(&titlesRead, MEDIATYPE_NAND, titleCount, titleIDs);
	if (R_FAILED(res)) {
		printf("AM_GetTitleList failed: 0x%lx\n", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	toCheck = calloc(titlesRead, sizeof(u64));
	if (!toCheck) {
		puts("Failed to allocate memory to check title IDs");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	for (i = 0; i < titlesRead; i++) {
		u64 tidHigh = titleIDs[i] >> 32;
		if (inArray(tidHigh, allowedTIDHigh, arraysizeof(allowedTIDHigh))) {
			toCheck[toCheck_count] = titleIDs[i];
			toCheck_count++;
		}
	}

	toDelete = calloc(toCheck_count, sizeof(u64));
	if (!toDelete) {
		puts("Failed to allocate memory title IDs to delete");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	for (i = 0; i < toCheck_count; i++) {
		if (!inArray(toCheck[i], titleIDList, titleIDList_count)) {
			toDelete[toDelete_count] = toCheck[i];
			toDelete_count++;
		}
	}

	// tickets
	res = AM_GetTicketCount(&ticketCount);
	if (R_FAILED(res)) {
		printf("AM_GetTicketCount failed: 0x%lx\n", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	ticketIDs = calloc(ticketCount, sizeof(u64));
	if (!ticketIDs) {
		puts("Failed to allocate memory for ticket IDs");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	res = AM_GetTicketList(&ticketsRead, ticketCount, 0, ticketIDs);
	if (R_FAILED(res)) {
		printf("AM_GetTicketList failed: 0x%lx\n", res);
		nextState = STATE_EXIT;
		goto mainLoop;
	}

	tikToCheck = calloc(titlesRead, sizeof(u64));
	if (!tikToCheck) {
		puts("Failed to allocate memory to check title IDs");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	for (i = 0; i < ticketsRead; i++) {
		u64 tidHigh = ticketIDs[i] >> 32;
		if (inArray(tidHigh, allowedTIDHigh, arraysizeof(allowedTIDHigh))) {
			tikToCheck[tikToCheck_count] = ticketIDs[i];
			tikToCheck_count++;
		}
	}

	tikToDelete = calloc(tikToCheck_count, sizeof(u64));
	if (!tikToDelete) {
		puts("Failed to allocate memory title IDs to delete");
		nextState = STATE_EXIT;
		goto mainLoop;
	}
	for (i = 0; i < tikToCheck_count; i++) {
		if (!inArray(tikToCheck[i], titleIDList, titleIDList_count)) {
			tikToDelete[tikToDelete_count] = tikToCheck[i];
			tikToDelete_count++;
		}
		// svcSleepThread(100000000LL);
	}

mainLoop:

	while (aptMainLoop()) {

		gspWaitForVBlank();
		hidScanInput();

		kDown = hidKeysDown();

		if (nextState != currentState) {
			switch (nextState) {
				case STATE_MENU:
					printf("%lu NAND titles found, %lu listed to be deleted.\n", titleCount, toDelete_count);
					printf("%lu tickets found, %lu listed to be deleted.\n", ticketCount, tikToDelete_count);
					printf("System region is %s.\nSystem model is %s3DS.\n\n", regionIDs[region], isNew3DS ? "New" : "Old");

					printf("Titles to delete: %lu\n", toDelete_count);
					puts  ("  A:     List titles to be deleted");
					puts  ("  X:     Delete titles");
					puts  ("  Y:     Save titles to SD\n");

					printf("Tickets to delete: %lu\n", tikToDelete_count);
					puts  ("  RIGHT: List ticket IDs to be deleted");
					puts  ("  UP:    Delete tickets");
					puts  ("  LEFT:  Save ticket IDs to SD\n");

					puts  ("B: Exit\n");
					break;

				case STATE_CONTINUE:
					puts("\nPress A to continue.");
					break;

				// titles
				case STATE_LIST:
					puts("----\n\nTitles that will be deleted are listed on the\nbottom screen. If the list is too long, save it\nto a file to read.");
					consoleSelect(&bottomScreen);
					consoleClear();
					puts("Titles:\n");
					for (i = 0; i < toDelete_count; i++) {
						printf("%016llx, %s", toDelete[i], i % 2 ? "\n" : "");
					}
					consoleSelect(&topScreen);
					break;

				case STATE_DELETE:
					puts("----\n\nDeleting titles...");
					consoleSelect(&bottomScreen);
					consoleClear();
					for (i = 0; i < toDelete_count; i++) {
						printf("Deleting: %016llx... \r", toDelete[i]);
						res = AM_DeleteTitle(MEDIATYPE_NAND, toDelete[i]);
						if (R_FAILED(res))
							printf("\nAM_DeleteTitle failed: 0x%04lx\n", res);
						else
							// re-print due to \r
							printf("Deleting: %016llx... OK\n", toDelete[i]);
					}
					consoleSelect(&topScreen);
					// only setting count, no need to modify toDelete here
					toDelete_count = 0;
					puts("Done deleting titles.");
					break;

				case STATE_SAVE:
					puts("----\n\nSaving to /nand-titles.bin...");
					bytesWritten = dumpList(toCheck, toCheck_count, "/nand-titles.bin");
					if (bytesWritten) printf("Wrote 0x%lx bytes.\n", bytesWritten);
					puts("Saving to /nand-titles-todelete.bin...");
					bytesWritten = dumpList(toDelete, toDelete_count, "/nand-titles-todelete.bin");
					if (bytesWritten) printf("Wrote 0x%lx bytes.\n", bytesWritten);
					break;

				// tickets
				case STATE_TIK_LIST:
					puts("----\n\nTickets that will be deleted are listed on the\nbottom screen. If the list is too long, save it\nto a file to read.");
					consoleSelect(&bottomScreen);
					consoleClear();
					puts("Tickets:\n");
					for (i = 0; i < tikToDelete_count; i++) {
						printf("%016llx, %s", tikToDelete[i], i % 2 ? "\n" : "");
					}
					consoleSelect(&topScreen);
					break;

				case STATE_TIK_DELETE:
					puts("----\n\nDeleting tickets...");
					consoleSelect(&bottomScreen);
					consoleClear();
					for (i = 0; i < tikToDelete_count; i++) {
						printf("Deleting: %016llx... \r", tikToDelete[i]);
						res = AM_DeleteTicket(tikToDelete[i]);
						if (R_FAILED(res))
							printf("\nAM_DeleteTicket failed: 0x%04lx\n", res);
						else
							// re-print due to \r
							printf("Deleting: %016llx... OK\n", tikToDelete[i]);
					}
					consoleSelect(&topScreen);
					// only setting count, no need to modify tikToDelete here
					tikToDelete_count = 0;
					puts("Done deleting tickets.");
					break;

				case STATE_TIK_SAVE:
					puts("----\n\nSaving to /nand-tickets.bin...");
					bytesWritten = dumpList(tikToCheck, tikToCheck_count, "/nand-tickets.bin");
					if (bytesWritten) printf("Wrote 0x%lx bytes.\n", bytesWritten);
					puts("Saving to /nand-tickets-todelete.bin...");
					bytesWritten = dumpList(tikToDelete, tikToDelete_count, "/nand-tickets-todelete.bin");
					if (bytesWritten) printf("Wrote 0x%lx bytes.\n", bytesWritten);
					break;

				case STATE_EXIT:
					puts("----\n\nPress START or B to exit.");
					break;

				default:
					break;
			}

			currentState = nextState;
		}

		switch (currentState) {
			case STATE_MENU:
				if      (kDown & KEY_A) nextState = STATE_LIST;
				else if (kDown & KEY_X) nextState = STATE_DELETE;
				else if (kDown & KEY_Y) nextState = STATE_SAVE;

				else if (kDown & KEY_DRIGHT) nextState = STATE_TIK_LIST;
				else if (kDown & KEY_DUP)    nextState = STATE_TIK_DELETE;
				else if (kDown & KEY_DLEFT)  nextState = STATE_TIK_SAVE;

				else if (kDown & KEY_B) nextState = STATE_EXIT;
				break;

			case STATE_DELETE:
				nextState = STATE_CONTINUE;
				break;

			case STATE_CONTINUE:
				if (kDown & KEY_A) {
					resetScreen(&topScreen);
					nextState = STATE_MENU;
				}
				break;

			case STATE_SAVE:
				nextState = STATE_CONTINUE;
				break;

			case STATE_EXIT:
				if (kDown & (KEY_START | KEY_B)) exiting = true;
				break;

			default:
				nextState = STATE_CONTINUE;
				break;
		}

		gfxFlushBuffers();
		gfxSwapBuffers();

		if (exiting) break;
	}

	free(titleIDs);
	free(ticketIDs);
	free(toCheck);
	free(toDelete);
	free(tikToCheck);
	free(tikToDelete);
	gfxExit();
	cfguExit();
	amExit();
	return 0;
}
