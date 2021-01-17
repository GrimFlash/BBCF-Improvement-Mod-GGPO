#pragma once

enum MatchState_
{
	MatchState_NotStarted = 0,
	MatchState_RebelActionRoundSign = 2,
	MatchState_Fight = 3,
	MatchState_FinishSign = 4,
	MatchState_WinLoseSign = 5,
	MatchState_VictoryScreen = 7
};

enum GameState
{
	GameState_ArcsysLogo = 2,
	GameState_IntroVideoPlaying = 3,
	GameState_TitleScreen = 4,
	GameState_CharacterSelectionScreen = 6,
	GameState_ArcadeActSelectScreen = 11,
	GameState_ScoreAttackModeSelectScreen = 11,
	GameState_SpeedStarModeSelectScreen = 11,
	GameState_ArcadeCharInfoScreen = 12,
	GameState_ArcadeStageSelectScreen = 13,
	GameState_VersusScreen = 14,
	GameState_InMatch = 15,
	GameState_VictoryScreen = 16,
	GameState_StoryMenu = 24,
	GameState_GalleryMenu = 25,
	GameState_ItemMenu = 25,
	GameState_ReplayMenu = 26,
	GameState_MainMenu = 27,
	GameState_TutorialMenu = 28,
	GameState_LibraryMenu = 28,
	GameState_Lobby = 31,
	GameState_StoryPlaying = 33,
	GameState_AbyssMenu = 34,
	GameState_DCodeEdit = 39,
};

enum GameMode
{
	GameMode_Arcade = 1,
	GameMode_Story = 4,
	GameMode_Versus = 5,
	GameMode_Training = 6,
	GameMode_Tutorial = 7,
	GameMode_Challenge = 8,
	// GameMode_Gallery = 9,
	// GameMode_ItemShop = 10,
	GameMode_ReplayTheater = 11,
	// GameMode_TitleScreen = 12,
	// GameMode_MainMenuScreen = 13,
	GameMode_Online = 15,
	GameMode_Abyss = 16,
	// GameMode_DCodeEdit = 18,
};

bool isPaletteEditingEnabledInCurrentState();
bool isHitboxOverlayEnabledInCurrentState();
bool isGameModeSelectorEnabledInCurrentState();
bool isStageSelectorEnabledInCurrentState();
bool isSaveLoadStateEnabledInCurrentState();

bool isInMatch();
bool isInMenu();
bool isOnVersusScreen();
bool isOnReplayMenuScreen();
bool isOnCharacterSelectionScreen();