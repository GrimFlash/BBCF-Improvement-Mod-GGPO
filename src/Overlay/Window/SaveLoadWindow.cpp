#include "SaveLoadWindow.h"

#include "Core/interfaces.h"
#include "Game/gamestates.h"


void SaveLoadWindow::Draw() 
{
	DrawSaveLoadStateWindow();
}

void SaveLoadWindow::DrawSaveLoadStateWindow() 
{
	if (!isSaveLoadStateEnabledInCurrentState())
		return;
	
	//static SavedGameState savedState;
	static int nFramesToSkipRender = 0;
	static int nMinSkip = 0;
	static int nMaxSkip = 60;
	static int nFrameStep = 1;

	if (nFramesToSkipRender < 0) {
		nFramesToSkipRender = 0;
	}
	else if (nFramesToSkipRender > 60) {
		nFramesToSkipRender = 60;
	}

	ImGui::InputInt("Num of frames to skip", &nFramesToSkipRender);

	if (ImGui::Button("Save")) {

		//savedState = SaveGameState();

	}

	if (ImGui::Button("Load")) {
		//LoadGameState(savedState);
		/*
		lpGameState->nFramesSkipped = 0;
		lpGameState->nFramesToSkipRender = nFramesToSkipRender;
		LoadGameState(lpGameState, &savedState);
		*/
	}

	/*This will be the workspace for getting the save/load state functioning*/
}