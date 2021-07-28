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
	const unsigned int prngOffset						= 0x59BC30;
}

namespace pointer_addresses
{
	int* pTimer                = 0;
	int* pScreenScroll_x       = 0;
	int* pScreenScroll_y       = 0;
	int* pCameraZoom           = 0;
	int* pDistortionBackground = 0;
	int* pDistortionFadeInOut  = 0;
	int* pPRNGOffset = 0;
	int* pStartOfPRNG = 0;
	int* pCurrentPRNG = 0;
	int* pRemainingPRNGSeeds = 0;
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
	LOG(2, "\t-pPRNGOffset: 0x%p - %d\n", pointer_addresses::pPRNGOffset, pointer_addresses::pPRNGOffset);
	LOG(2, "\t-pStartOfPRNG: 0x%p - %d\n", pointer_addresses::pStartOfPRNG, gameState.pStartOfPRNG);
	LOG(2, "\t-pCurrentPRNG: 0x%p - %d\n", pointer_addresses::pCurrentPRNG, gameState.pCurrentPRNG);
	LOG(2, "\t-pRemainingPRNGSeeds: 0x%p - %d\n", pointer_addresses::pRemainingPRNGSeeds, gameState.pRemainingPRNGSeeds);
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
	pointer_addresses::pPRNGOffset			 = (int*)(base + pointer_offsets::prngOffset);
	pointer_addresses::pStartOfPRNG			 = (int*)(*pointer_addresses::pPRNGOffset + 0x9CC);
	pointer_addresses::pCurrentPRNG			 = (int*)(*pointer_addresses::pPRNGOffset + 0x1394);
	pointer_addresses::pRemainingPRNGSeeds   = (int*)(*pointer_addresses::pPRNGOffset + 0x138C);
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
	gameState.pStartOfPRNG		   = *pointer_addresses::pStartOfPRNG;
	gameState.pCurrentPRNG		   = *pointer_addresses::pCurrentPRNG;
	gameState.pRemainingPRNGSeeds  = *pointer_addresses::pRemainingPRNGSeeds;
	memcpy(&gameState.PRNGMap, pointer_addresses::pStartOfPRNG, 0x9C0); //Credit to Gneiss for finding the PRNG Addresses
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
	*pointer_addresses::pStartOfPRNG		  = gameState.pStartOfPRNG;
	*pointer_addresses::pCurrentPRNG		  = gameState.pCurrentPRNG;
	*pointer_addresses::pRemainingPRNGSeeds   = gameState.pRemainingPRNGSeeds;
	memcpy(pointer_addresses::pStartOfPRNG, &gameState.PRNGMap, 0x9C0); //Credit to Gneiss for finding the PRNG Addresses
	memcpy(pointer_addresses::pPlayer1, &gameState.player1, ENTITY_SIZE);
	memcpy(pointer_addresses::pPlayer2, &gameState.player2, ENTITY_SIZE);

	LOG(2, "Finished loading\n");
}
