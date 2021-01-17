#include "gamestates.h"

#include "Core/interfaces.h"

bool isPaletteEditingEnabledInCurrentState()
{
	bool isEnabledInCurrentState =
		*g_gameVals.pGameState == GameState_InMatch;

	bool isEnabledInCurrentMode =
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus;

	return isEnabledInCurrentState && isEnabledInCurrentMode;
}

bool isHitboxOverlayEnabledInCurrentState()
{
	bool isEnabledInCurrentState =
		*g_gameVals.pGameState == GameState_InMatch;

	bool isEnabledInCurrentMode =
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus ||
		*g_gameVals.pGameMode == GameMode_ReplayTheater;

	return isEnabledInCurrentState && isEnabledInCurrentMode;
}

bool isGameModeSelectorEnabledInCurrentState()
{
	bool isEnabledInCurrentState =
		*g_gameVals.pGameState == GameState_CharacterSelectionScreen ||
		*g_gameVals.pGameState == GameState_ReplayMenu;

	bool isEnabledInCurrentMode =
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus ||
		*g_gameVals.pGameMode == GameMode_Online ||
		*g_gameVals.pGameState == GameState_ReplayMenu;

	return isEnabledInCurrentState && isEnabledInCurrentMode;
}

bool isStageSelectorEnabledInCurrentState()
{
	return *g_gameVals.pGameState == GameState_CharacterSelectionScreen;
}

bool isSaveLoadStateEnabledInCurrentState()
{
	bool isEnabledInCurrentState =
		*g_gameVals.pGameState == GameState_InMatch;

	bool isEnabledInCurrentMode =
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus;

	return isEnabledInCurrentState && isEnabledInCurrentMode;
}

bool isInMatch()
{
	return *g_gameVals.pGameState == GameState_InMatch;
}

bool isInMenu()
{
	return *g_gameVals.pGameState == GameState_MainMenu;
}

bool isOnVersusScreen()
{
	return *g_gameVals.pGameState == GameState_VersusScreen;
}

bool isOnReplayMenuScreen()
{
	return *g_gameVals.pGameState == GameState_ReplayMenu;
}

bool isOnCharacterSelectionScreen()
{
	return *g_gameVals.pGameState == GameState_CharacterSelectionScreen;
}
