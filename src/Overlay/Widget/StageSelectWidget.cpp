#include "StageSelectWidget.h"

#include "Core/interfaces.h"
#include "Game/gamestates.h"
#include "Game/stages.h"
#include "Overlay/imgui_utils.h"

void StageSelectWidget()
{
	if (g_gameVals.stageSelect_X != nullptr &&
		g_gameVals.stageSelect_Y != nullptr)
	{
		static unsigned char selectedStage = 0;

		// Sync slider with the selected stage
		for (int i = 0; i < STAGES_COUNT; i++)
		{
			if (stages[i][0] == *g_gameVals.stageSelect_X &&
				stages[i][1] == *g_gameVals.stageSelect_Y)
			{
				selectedStage = i;
				break;
			}
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Stage"); ImGui::SameLine();
		if (ImGui::SliderByte("##Stage", &selectedStage, 0, STAGES_COUNT - 1))
		{
			*g_gameVals.stageSelect_X = stages[selectedStage][0];
			*g_gameVals.stageSelect_Y = stages[selectedStage][1];
		}
	}
}
