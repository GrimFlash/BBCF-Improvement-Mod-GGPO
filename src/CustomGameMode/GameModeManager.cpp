#include "GameModeManager.h"

#include "Core/logger.h"
#include "Hooks/HookManager.h"
#include "Hooks/hooks_customGameModes.h"

GameModeManager::GameModeManager()
	: m_gameModes(CreateGameModesVector()), m_activeGameMode(CustomGameMode_None)
{
}

void GameModeManager::InitGameMode()
{
	LOG(2, "InitGameMode\n");

	ResetAllHooks();
	m_gameModes[m_activeGameMode].init_func();
	ActivateGameModeHooks();
}

void GameModeManager::EndGameMode()
{
	LOG(2, "EndGameMode\n");

	m_activeGameMode = CustomGameMode_None;
	ResetAllHooks();
}

void GameModeManager::SetActiveGameMode(CustomGameMode gameMode)
{
	LOG(2, "SetActiveGameMode\n");

	m_activeGameMode = gameMode;
}

CustomGameMode& GameModeManager::GetActiveGameModeRef()
{
	return m_activeGameMode;
}

const CustomGameMode GameModeManager::GetActiveGameMode() const
{
	return m_activeGameMode;
}

const std::string& GameModeManager::GetCurrentGameModeName() const
{
	return m_gameModes[m_activeGameMode].name;
}


const std::string& GameModeManager::GetGameModeName(CustomGameMode gameMode) const
{
	return m_gameModes[gameMode].name;
}

const std::string& GameModeManager::GetGameModeDesc(CustomGameMode gameMode) const
{
	return m_gameModes[gameMode].desc;
}

int GameModeManager::GetGameModesCount() const
{
	return m_gameModes.size();
}

void GameModeManager::ActivateGameModeHooks()
{
	for (const std::string& hookLabel : m_gameModes[m_activeGameMode].hook_labels)
	{
		HookManager::ActivateHook(hookLabel.c_str());
	}
}

void GameModeManager::ResetAllHooks()
{
	LOG(2, "ResetAllHooks\n");

	for (const GameMode_t& gameMode : m_gameModes)
	{
		for (const std::string& hookLabel : gameMode.hook_labels)
		{
			HookManager::DeactivateHook(hookLabel.c_str());
		}
	}

	// Change heat limits of 15000 back to 10000
	int result = HookManager::OverWriteBytes(
		(char*)steroid_HeatModifyJmpBackAddr,
		(char*)steroid_HeatModifyJmpBackAddr + 0x28,
		"\x98\x3A\x00\x00",
		"xxxx",
		"\x10\x27\x00\x00");

	if (result)
	{
		LOG(2, "Original steroid_HeatModify has been reset, overwritten bytes: %d\n", result);
	}
}
