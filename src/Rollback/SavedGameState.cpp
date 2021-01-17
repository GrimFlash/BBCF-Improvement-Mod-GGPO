#include "SavedGameState.h"

#include "Core/interfaces.h"
#include "Core/logger.h"

namespace pointer_offsets
{
	const unsigned int screenScrollPtrOffset_x          = 0xDC2130;
	const unsigned int screenScrollPtrOffset_y          = 0xDC2134;
	const unsigned int cameraZoomPtrOffset              = 0xDC20B0;
	const unsigned int distortionBackgroundPtrOffset    = 0xD3587C;
	const unsigned int distortionFadeInOutPtrOffset     = 0xDA0F44;
}

namespace pointer_addresses
{
	int* pTimer                = 0;
	int* pScreenScroll_x       = 0;
	int* pScreenScroll_y       = 0;
	int* pCameraZoom           = 0;
	int* pDistortionBackground = 0;
	int* pDistortionFadeInOut  = 0;
	CharData* pPlayer1         = 0;
	CharData* pPlayer2         = 0;
}

void logGameState(const SavedGameState& gameState)
{
	LOG(2, "SavedGameState:\n");

	LOG(2, "\t-timer: 0x%p - %d\n", pointer_addresses::pTimer, gameState.timer);
	LOG(2, "\t-screenScroll_x: 0x%p - %d\n", pointer_addresses::pScreenScroll_x, gameState.screenScroll_x);
	LOG(2, "\t-screenScroll_y: 0x%p - %d\n", pointer_addresses::pScreenScroll_y, gameState.screenScroll_y);
	LOG(2, "\t-cameraZoom: 0x%p - %d\n", pointer_addresses::pCameraZoom, gameState.cameraZoom);
	LOG(2, "\t-distortionBackground: 0x%p - %d\n", pointer_addresses::pDistortionBackground, gameState.distortionBackground);
	LOG(2, "\t-distortionFadeInOut: 0x%p - %d\n", pointer_addresses::pDistortionFadeInOut, gameState.distortionFadeInOut);
}

void initGameStateAddresses()
{
	const int base = (int)g_gameProc.hBBCFGameModule;

	pointer_addresses::pTimer                = g_gameVals.pMatchTimer;
	pointer_addresses::pScreenScroll_x       = (int*)(base + pointer_offsets::screenScrollPtrOffset_x);
	pointer_addresses::pScreenScroll_y       = (int*)(base + pointer_offsets::screenScrollPtrOffset_y);
	pointer_addresses::pCameraZoom           = (int*)(base + pointer_offsets::cameraZoomPtrOffset);
	pointer_addresses::pDistortionBackground = (int*)(base + pointer_offsets::distortionBackgroundPtrOffset);
	pointer_addresses::pDistortionFadeInOut  = (int*)(base + pointer_offsets::distortionFadeInOutPtrOffset);
	pointer_addresses::pPlayer1              = g_interfaces.player1.GetData();
	pointer_addresses::pPlayer2              = g_interfaces.player2.GetData();
}

void saveGameState(SavedGameState& gameState)
{
	LOG(2, "Saving game state...\n");

	gameState.timer                = *pointer_addresses::pTimer;
	gameState.screenScroll_x       = *pointer_addresses::pScreenScroll_x;
	gameState.screenScroll_y       = *pointer_addresses::pScreenScroll_y;
	gameState.cameraZoom           = *pointer_addresses::pCameraZoom;
	gameState.distortionBackground = *pointer_addresses::pDistortionBackground;
	gameState.distortionFadeInOut  = *pointer_addresses::pDistortionFadeInOut;

	memcpy(&gameState.player1, pointer_addresses::pPlayer1, ENTITY_SIZE);
	memcpy(&gameState.player2, pointer_addresses::pPlayer2, ENTITY_SIZE);

	logGameState(gameState);

	LOG(2, "Finished saving\n");
}

void loadGameState(SavedGameState& gameState)
{
	LOG(2, "Loading game state...\n");
	logGameState(gameState);

	*pointer_addresses::pTimer                = gameState.timer;
	*pointer_addresses::pScreenScroll_x       = gameState.screenScroll_x;
	*pointer_addresses::pScreenScroll_y       = gameState.screenScroll_y;
	*pointer_addresses::pCameraZoom           = gameState.cameraZoom;
	*pointer_addresses::pDistortionBackground = gameState.distortionBackground;
	*pointer_addresses::pDistortionFadeInOut  = gameState.distortionFadeInOut;

	memcpy(pointer_addresses::pPlayer1, &gameState.player1, ENTITY_SIZE);
	memcpy(pointer_addresses::pPlayer2, &gameState.player2, ENTITY_SIZE);

	LOG(2, "Finished loading\n");
}
