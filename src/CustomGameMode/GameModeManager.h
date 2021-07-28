#pragma once
#include "customGameMode.h"

class GameModeManager
{
public:
	GameModeManager();

	void InitGameMode();
	void EndGameMode();
	void SetActiveGameMode(CustomGameMode gameMode);

	CustomGameMode& GetActiveGameModeRef();
	const CustomGameMode GameModeManager::GetActiveGameMode() const;
	const std::string& GetCurrentGameModeName() const;
	const std::string& GetGameModeName(CustomGameMode gameMode) const;
	const std::string& GetGameModeDesc(CustomGameMode gameMode) const;
	int GetGameModesCount() const;

private:
	void ActivateGameModeHooks();
	void ResetAllHooks();

	std::vector<GameMode_t> m_gameModes;
	CustomGameMode m_activeGameMode;
};