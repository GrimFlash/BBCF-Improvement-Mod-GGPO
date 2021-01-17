#include "SaveLoadWindow.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Game/gamestates.h"
#include "Overlay/imgui_utils.h"
#include "Rollback/SavedGameState.h"

void SaveLoadWindow::Draw() 
{
	DrawSaveLoadStateWindow();
}

void SaveLoadWindow::DrawSaveLoadStateWindow() 
{
	if (!isSaveLoadStateEnabledInCurrentState())
		return;
	
	const int nMinSkip = 0;
	const int nMaxSkip = 60;
	static int nFramesToSkipRender = 0;
	static int nFrameStep = 1;
	static SavedGameState gameState = {};

	ImGui::SliderInt("Num of frames to skip", &nFramesToSkipRender, nMinSkip, nMaxSkip);

	ImGui::VerticalSpacing(10);

	if (ImGui::Button("Save", ImVec2(50, 25))) {
		saveGameState(gameState);

		//savedState = SaveGameState();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(50, 25))) {
		loadGameState(gameState);

		//LoadGameState(savedState);
		/*
		lpGameState->nFramesSkipped = 0;
		lpGameState->nFramesToSkipRender = nFramesToSkipRender;
		LoadGameState(lpGameState, &savedState);
		*/
	}

	/*This will be the workspace for getting the save/load state functioning*/
}
