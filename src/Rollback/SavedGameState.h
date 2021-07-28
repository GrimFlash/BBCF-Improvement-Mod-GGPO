#pragma once

#include "Game/CharData.h"

const int ENTITY_SIZE = sizeof(CharData);

struct SavedGameState
{
	int timer;
	int screenScroll_x;
	int screenScroll_y;
	int cameraZoom;
	int distortionBackground;
	int distortionFadeInOut;
	int pStartOfPRNG;
	int pCurrentPRNG;
	int pRemainingPRNGSeeds;
	char PRNGMap[0x9C0]; //Credit to Gneiss for finding the PRNG Addresses
	char player1[ENTITY_SIZE];
	char player2[ENTITY_SIZE];
};

// Call in OnMatchInit
void initGameStateAddresses();

// Saves into gameState ref
void saveGameState(SavedGameState& gameState);

// Loads into gameState ref
void loadGameState(SavedGameState& gameState);
