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
	if (!isSaveLoadStateEnabledInCurrentState()) {
		ImGui::Text("NOT IN VERSUS/TRAINING");
		return;
	}
	
	const int nMinSkip = 0;
	const int nMaxSkip = 60;
	static int nFramesToSkipRender = 0;
	static int nFrameStep = 1;
	static SavedGameState gameState = {};

	static bool isTrue = false;

	ImGui::SliderInt("Num of frames to skip", &nFramesToSkipRender, nMinSkip, nMaxSkip);

	ImGui::VerticalSpacing(10);

	if (ImGui::Button("Save", ImVec2(50, 25))) 
		saveGameState(gameState);

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(50, 25))) {
		/*
		lpGameState->nFramesSkipped = 0;
		lpGameState->nFramesToSkipRender = nFramesToSkipRender;
		*/
		loadGameState(gameState);
	}

	ImGui::Checkbox("Enable Keyboard input", &isTrue);

	if (isTrue) {
		ImGui::Text("Press F8 to save\nPress F9 to load");

		if (ImGui::IsKeyPressed(119, false)) { // F8 will save state
			saveGameState(gameState);
		}

		if (ImGui::IsKeyPressed(120, false)) { // F9 will load state
			/*
			lpGameState->nFramesSkipped = 0;
			lpGameState->nFramesToSkipRender = nFramesToSkipRender;
			*/
			loadGameState(gameState);
		}
	}

	

	/*This will be the workspace for getting the save/load state functioning*/
}
