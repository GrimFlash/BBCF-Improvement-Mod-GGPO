#include "PaletteEditorWindow.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Game/gamestates.h"
#include "Overlay/imgui_utils.h"
#include "Overlay/Logger/ImGuiLogger.h"
#include "Palette/impl_format.h"

#include <imgui.h>

#include <Shlwapi.h>

#define NUMBER_OF_COLOR_BOXES (IMPL_PALETTE_DATALEN / sizeof(int)) // 256
#define COLUMNS 16

const int COLOR_BLACK = 0xFF000000;
const int COLOR_WHITE = 0xFFFFFFFF;
const ImVec4 COLOR_ONLINE(0.260f, 0.590f, 0.980f, 1.000f);

static char palNameBuf[IMPL_PALNAME_LENGTH] = "";
static char palDescBuf[IMPL_DESC_LENGTH] = "";
static char palCreatorBuf[IMPL_CREATOR_LENGTH] = "";
static bool palBoolEffect = false;

void PaletteEditorWindow::ShowAllPaletteSelections(const std::string& windowID)
{
	if (HasNullPointer())
	{
		return;
	}

	const char* p1BtnText = " Player1 ";
	const char* p2BtnText = " Player2 ";
	const std::string p1PopupID = "select1-1" + windowID;
	const std::string p2PopupID = "select2-1" + windowID;

	if (g_interfaces.pRoomManager->IsRoomFunctional())
	{
		uint16_t thisPlayerMatchPlayerIndex = g_interfaces.pRoomManager->GetThisPlayerMatchPlayerIndex();

		ImGui::BeginGroup();

		if (thisPlayerMatchPlayerIndex == 0)
		{
			ShowPaletteSelectButton(g_interfaces.player1, p1BtnText, p1PopupID.c_str());
		}
		else
		{
			ShowOnlinePaletteResetButton(g_interfaces.player1, thisPlayerMatchPlayerIndex, p1BtnText);
		}

		if (thisPlayerMatchPlayerIndex == 1)
		{
			ShowPaletteSelectButton(g_interfaces.player2, p2BtnText, p2PopupID.c_str());
		}
		else
		{
			ShowOnlinePaletteResetButton(g_interfaces.player2, thisPlayerMatchPlayerIndex, p2BtnText);
		}

		ImGui::EndGroup();

		return;
	}

	ImGui::BeginGroup();

	ShowPaletteSelectButton(g_interfaces.player1, p1BtnText, p1PopupID.c_str());
	ShowPaletteSelectButton(g_interfaces.player2, p2BtnText, p2PopupID.c_str());

	ImGui::EndGroup();
}

void PaletteEditorWindow::ShowReloadAllPalettesButton()
{
	if (ImGui::Button("Reload custom palettes"))
	{
		g_interfaces.pPaletteManager->ReloadAllPalettes();
	}
}

void PaletteEditorWindow::OnMatchInit()
{
	if (HasNullPointer())
	{
		return;
	}

	InitializeSelectedCharacters();

	m_selectedCharIndex = (CharIndex)m_playerHandles[0]->GetData()->charIndex;
	m_selectedCharName = m_allSelectedCharNames[0].c_str();
	m_selectedCharPalHandle = &m_playerHandles[0]->GetPalHandle();
	m_selectedPalIndex = g_interfaces.pPaletteManager->GetCurrentCustomPalIndex(*m_selectedCharPalHandle);
	CopyImplDataToEditorFields(*m_selectedCharPalHandle);
	m_selectedFile = PaletteFile_Character;

	m_colorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha;
	m_highlightMode = false;
	m_showAlpha = false;

	CopyPalFileToEditorArray(m_selectedFile, *m_selectedCharPalHandle);
}

void PaletteEditorWindow::Draw()
{
	if (!isPaletteEditingEnabledInCurrentState() || HasNullPointer())
	{
		Close();
		return;
	}

	CheckSelectedPalOutOfBound();

	CharacterSelection();
	PaletteSelection();
	FileSelection();
	EditingModesSelection();
	ShowPaletteBoxes();
	SavePaletteToFile();
}

bool PaletteEditorWindow::HasNullPointer()
{
	return g_interfaces.player1.IsCharDataNullPtr() ||
		g_interfaces.player2.IsCharDataNullPtr();
}

void PaletteEditorWindow::InitializeSelectedCharacters()
{
	m_playerHandles[0] = &g_interfaces.player1;
	m_playerHandles[1] = &g_interfaces.player2;

	m_allSelectedCharNames[0] = getCharacterNameByIndexA(m_playerHandles[0]->GetData()->charIndex);
	m_allSelectedCharNames[1] = getCharacterNameByIndexA(m_playerHandles[1]->GetData()->charIndex);
}

void PaletteEditorWindow::CharacterSelection()
{
	LOG(7, "PaletteEditorWindow CharacterSelection\n");

	if (ImGui::Button("Select character"))
	{
		ImGui::OpenPopup("select_char_pal");
	}

	ImGui::SameLine();
	ImGui::Text(m_selectedCharName);

	if (ImGui::BeginPopup("select_char_pal"))
	{
		const int NUMBER_OF_CHARS = 2;
		
		for (int i = 0; i < NUMBER_OF_CHARS; i++)
		{
			ImGui::PushID(i);

			if (ImGui::Selectable(m_allSelectedCharNames[i].c_str()))
			{
				DisableHighlightModes();

				m_selectedCharIndex = (CharIndex)m_playerHandles[i]->GetData()->charIndex;
				m_selectedCharName = m_allSelectedCharNames[i].c_str();
				m_selectedCharPalHandle = &m_playerHandles[i]->GetPalHandle();
				m_selectedPalIndex = g_interfaces.pPaletteManager->GetCurrentCustomPalIndex(*m_selectedCharPalHandle);
				CopyPalFileToEditorArray(m_selectedFile, *m_selectedCharPalHandle);
				CopyImplDataToEditorFields(*m_selectedCharPalHandle);
			}

			ImGui::PopID();
		}

		ImGui::EndPopup();
	}
}

void PaletteEditorWindow::PaletteSelection()
{
	LOG(7, "PaletteEditorWindow PaletteSelection\n");

	if (ImGui::Button("Select palette  "))
	{
		ImGui::OpenPopup("select_custom_pal");
	}

	ImGui::SameLine();
	ImGui::Text(m_customPaletteVector[m_selectedCharIndex][m_selectedPalIndex].palInfo.palName);

	ShowPaletteSelectPopup(*m_selectedCharPalHandle, m_selectedCharIndex, "select_custom_pal");
}

void PaletteEditorWindow::FileSelection()
{
	LOG(7, "PaletteEditorWindow FileSelection\n");

	if (ImGui::Button("Select file     "))
	{
		ImGui::OpenPopup("select_file_pal");
	}

	ImGui::SameLine();
	ImGui::Text(palFileNames[m_selectedFile]);

	if (ImGui::BeginPopup("select_file_pal"))
	{
		for (int i = 0; i < TOTAL_PALETTE_FILES; i++)
		{
			if (ImGui::Selectable(palFileNames[i]))
			{
				DisableHighlightModes();
				m_selectedFile = (PaletteFile)(i);
				CopyPalFileToEditorArray(m_selectedFile, *m_selectedCharPalHandle);
			}
		}

		ImGui::EndPopup();
	}
}

void PaletteEditorWindow::EditingModesSelection()
{
	LOG(7, "PaletteEditorWindow EditingModesSelection\n");

	ImGui::Separator();
	if (ImGui::Checkbox("Show transparency values", &m_showAlpha))
	{
		m_colorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha;
		if (m_showAlpha)
		{
			m_colorEditFlags &= ~ImGuiColorEditFlags_NoAlpha;
			m_colorEditFlags |= ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar;
		}
	}

	ImGui::SameLine();
	int nextLineColumnPosX = ImGui::GetCursorPosX();
	ImGui::Checkbox("Freeze frame", &g_gameVals.isFrameFrozen);
	
	if (ImGui::Checkbox("Highlight mode", &m_highlightMode))
	{
		if (m_highlightMode)
		{
			// Fill the array with black
			for (int i = 0; i < NUMBER_OF_COLOR_BOXES; i++)
			{
				((int*)m_highlightArray)[i] = COLOR_BLACK;
			}
			g_interfaces.pPaletteManager->ReplacePaletteFile(m_highlightArray, m_selectedFile, *m_selectedCharPalHandle);
		}
		else
		{
			DisableHighlightModes();
		}
	}

	if (ImGui::Button("Gradient generator"))
	{
		ImGui::OpenPopup("gradient");
	}

	ShowGradientPopup();

	ImGui::Separator();
}

void PaletteEditorWindow::ShowPaletteBoxes()
{
	LOG(7, "PaletteEditorWindow ShowPaletteBoxes\n");

	ImGui::VerticalSpacing(10);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

	ImGui::TextUnformatted("001 "); ImGui::SameLine();

	for (int i = 0, col = 1; i < NUMBER_OF_COLOR_BOXES; i++)
	{
		ImGui::PushID(i);

		bool pressed = false;
		int curColorBoxOffset = (i * sizeof(int));
		int idx = i + 1;

		if (m_highlightMode)
		{
			ImGui::ColorButtonOn32Bit("##PalColorButton", idx, (unsigned char*)m_paletteEditorArray + curColorBoxOffset, m_colorEditFlags);

			if (ImGui::IsItemHovered())
				pressed = true;
		}
		else
		{
			pressed = ImGui::ColorEdit4On32Bit("##PalColorEdit", idx, (unsigned char*)m_paletteEditorArray + curColorBoxOffset, m_colorEditFlags);
		}

		if (pressed)
		{
			if (m_highlightMode)
			{
				UpdateHighlightArray(i);
			}
			else
			{
				g_interfaces.pPaletteManager->ReplacePaletteFile(m_paletteEditorArray, m_selectedFile, *m_selectedCharPalHandle);
			}
		}

		if (col < COLUMNS)
		{
			// Continue the row
			ImGui::SameLine();
			col++;
		}
		else
		{
			// Start a new row
			col = 1;
			if (i < NUMBER_OF_COLOR_BOXES - 1)
			{
				ImGui::Text("%.3d ", i + 2);
				ImGui::SameLine();
			}
		}

		ImGui::PopID();
	}

	ImGui::PopStyleVar();
}

void PaletteEditorWindow::DisableHighlightModes()
{
	m_highlightMode = false;
	g_interfaces.pPaletteManager->ReplacePaletteFile(m_paletteEditorArray, m_selectedFile, *m_selectedCharPalHandle);
}

void PaletteEditorWindow::SavePaletteToFile()
{
	static char message[200] = "";

	ImGui::VerticalSpacing(10);
	ImGui::Separator();

	if (m_highlightMode)
	{
		ImGui::TextDisabled("Cannot save with Highlight mode on!");
		return;
	}

	struct TextFilters { static int FilterAllowedChars(ImGuiTextEditCallbackData* data) { if (data->EventChar < 256 && strchr(" qwertzuiopasdfghjklyxcvbnmQWERTZUIOPASDFGHJKLYXCVBNM0123456789_.()[]!@&+-'^,;{}$=", (char)data->EventChar)) return 0; return 1; } };


	ImGui::Checkbox("Save with bloom effect", &palBoolEffect);
	ImGui::HoverTooltip("Bloom effects cannot be changed until a new round is started");
	ImGui::Spacing();

	ImGui::Text("Palette name:");
	ImGui::PushItemWidth(250);
	ImGui::InputText("##palName", palNameBuf, IMPL_PALNAME_LENGTH, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterAllowedChars);
	ImGui::PopItemWidth();

	ImGui::Text("Creator (optional):");
	ImGui::PushItemWidth(250);
	ImGui::InputText("##palcreator", palCreatorBuf, IMPL_CREATOR_LENGTH, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterAllowedChars);
	ImGui::PopItemWidth();

	ImGui::Text("Palette description (optional):");
	ImGui::PushItemWidth(250);
	ImGui::InputText("##palDesc", palDescBuf, IMPL_DESC_LENGTH, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterAllowedChars);
	ImGui::PopItemWidth();

	ImGui::Spacing();

	bool pressed = ImGui::Button("Save palette", ImVec2(125, 25));
	ImGui::Text(message);

	static bool show_overwrite_popup = false;

	if (!pressed && !show_overwrite_popup)
		return;

	if (strncmp(palNameBuf, "", IMPL_PALNAME_LENGTH) == 0)
	{
		std::string errorMsg = "Error, no filename given";
		memcpy_s(message, sizeof(message), errorMsg.c_str(), errorMsg.length());
		g_imGuiLogger->Log("[error] Could not save custom palette, no filename was given\n");
		return;
	}

	if (strncmp(palNameBuf, "Default", IMPL_PALNAME_LENGTH) == 0 || strncmp(palNameBuf, "Random", IMPL_PALNAME_LENGTH) == 0)
	{
		std::string errorMsg = "Error, not a valid filename";
		memcpy_s(message, sizeof(message), errorMsg.c_str(), errorMsg.length());
		g_imGuiLogger->Log("[error] Could not save custom palette: not a valid filename\n");
		return;
	}

	TCHAR pathBuf[MAX_PATH];
	GetModuleFileName(NULL, pathBuf, MAX_PATH);
	std::wstring::size_type pos = std::wstring(pathBuf).find_last_of(L"\\");
	std::wstring wFullPath = std::wstring(pathBuf).substr(0, pos);

	wFullPath += L"\\BBCF_IM\\Palettes\\";
	wFullPath += getCharacterNameByIndexW(m_selectedCharIndex);
	wFullPath += L"\\";

	std::string filenameTemp(palNameBuf);
	std::wstring wFilename(filenameTemp.begin(), filenameTemp.end());
	wFullPath += wFilename;

	if (wFilename.find(IMPL_FILE_EXTENSION_W) == std::wstring::npos)
	{
		wFullPath += IMPL_FILE_EXTENSION_W;
		filenameTemp += IMPL_FILE_EXTENSION;
	}

	if (ShowOverwritePopup(&show_overwrite_popup, wFullPath.c_str(), filenameTemp.c_str()))
	{

		IMPL_data_t curPalData = g_interfaces.pPaletteManager->GetCurrentPalData(*m_selectedCharPalHandle);

		strncpy(curPalData.palInfo.creator, palCreatorBuf, IMPL_CREATOR_LENGTH);
		strncpy(curPalData.palInfo.palName, palNameBuf, IMPL_PALNAME_LENGTH);
		strncpy(curPalData.palInfo.desc, palDescBuf, IMPL_DESC_LENGTH);
		curPalData.palInfo.hasBloom = palBoolEffect;

		std::string messageText = "'";
		messageText += filenameTemp.c_str();

		if (g_interfaces.pPaletteManager->WritePaletteToFile(m_selectedCharIndex, &curPalData))
		{
			std::string fullPath(wFullPath.begin(), wFullPath.end());
			g_imGuiLogger->Log("[system] Custom palette '%s' successfully saved to:\n'%s'\n", filenameTemp.c_str(), fullPath.c_str());
			messageText += "' saved successfully";

			ReloadSavedPalette(palNameBuf);
		}
		else
		{
			g_imGuiLogger->Log("[error] Custom palette '%s' failed to be saved.\n", filenameTemp.c_str());
			messageText += "' save failed";
		}

		memcpy(message, messageText.c_str(), messageText.length() + 1);
	}
}

void PaletteEditorWindow::ReloadSavedPalette(const char* palName)
{
	g_imGuiLogger->EnableLog(false);
	g_interfaces.pPaletteManager->ReloadAllPalettes();
	g_imGuiLogger->EnableLog(true);

	//find the newly loaded custom pal
	m_selectedPalIndex = g_interfaces.pPaletteManager->FindCustomPalIndex(m_selectedCharIndex, palName);

	if (m_selectedPalIndex < 0)
	{
		g_imGuiLogger->Log("[error] Saved custom palette couldn't be reloaded. Not found.\n");
		m_selectedPalIndex = 0;
	}

	g_interfaces.pPaletteManager->SwitchPalette(m_selectedCharIndex, *m_selectedCharPalHandle, m_selectedPalIndex);
	CopyPalFileToEditorArray(m_selectedFile, *m_selectedCharPalHandle);
}

bool PaletteEditorWindow::ShowOverwritePopup(bool *p_open, const wchar_t* wFullPath, const char* filename)
{
	bool isOverwriteAllowed = true;

	if (PathFileExists(wFullPath))
	{
		ImGui::OpenPopup("Overwrite?");
		*p_open = true;
	}

	if (ImGui::BeginPopupModal("Overwrite?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("'%s' already exists.\nAre you sure you want to overwrite it?\n\n", filename);
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			*p_open = false;
			isOverwriteAllowed = true;
			return isOverwriteAllowed;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			*p_open = false;
		}

		ImGui::EndPopup();
		isOverwriteAllowed = false;
	}

	return isOverwriteAllowed;
}

void PaletteEditorWindow::CheckSelectedPalOutOfBound()
{
	if (m_selectedPalIndex != 0 && m_selectedPalIndex >= m_customPaletteVector[m_selectedCharIndex].size())
	{
		// Reset back to default
		m_selectedPalIndex = 0;
		g_interfaces.pPaletteManager->SwitchPalette(m_selectedCharIndex, *m_selectedCharPalHandle, m_selectedPalIndex);
		CopyPalFileToEditorArray(m_selectedFile, *m_selectedCharPalHandle);
	}
}

void PaletteEditorWindow::ShowOnlinePaletteResetButton(Player& playerHandle, uint16_t thisPlayerMatchPlayerIndex, const char* btnText)
{
	CharPaletteHandle& charPalHandle = playerHandle.GetPalHandle();
	CharIndex charIndex = (CharIndex)playerHandle.GetData()->charIndex;

	char buf[32];
	sprintf_s(buf, " X ##%s", btnText);

	if (ImGui::Button(buf))
	{
		g_interfaces.pPaletteManager->RestoreOrigPal(charPalHandle);
	}

	ImGui::HoverTooltip("Reset palette");

	// Dummy button
	ImGui::SameLine();
	ImGui::Button(btnText);

	ImGui::HoverTooltip(getCharacterNameByIndexA(charIndex).c_str());

	ImGui::SameLine();

	const IMPL_info_t& palInfo = g_interfaces.pPaletteManager->GetCurrentPalInfo(charPalHandle);
	ImGui::TextUnformatted(palInfo.palName);

	ShowHoveredPaletteInfoToolTip(palInfo, charIndex, 0);
}

void PaletteEditorWindow::ShowPaletteSelectButton(Player & playerHandle, const char * btnText, const char * popupID)
{
	CharPaletteHandle& charPalHandle = playerHandle.GetPalHandle();
	int selected_pal_index = g_interfaces.pPaletteManager->GetCurrentCustomPalIndex(charPalHandle);
	CharIndex charIndex = (CharIndex)playerHandle.GetData()->charIndex;

	if (charIndex >= getCharactersCount() || m_customPaletteVector[charIndex].size() <= selected_pal_index)
	{
		ImGui::TextUnformatted("Out of bounds");
		return;
	}

	ShowPaletteRandomizerButton(popupID, playerHandle);
	ImGui::SameLine();

	if (ImGui::Button(btnText))
	{
		ImGui::OpenPopup(popupID);
	}

	ImGui::HoverTooltip(getCharacterNameByIndexA(playerHandle.GetData()->charIndex).c_str());

	const IMPL_info_t& palInfo = m_customPaletteVector[charIndex][selected_pal_index].palInfo;

	ImGui::SameLine();
	ImGui::TextUnformatted(palInfo.palName);
	ShowHoveredPaletteInfoToolTip(palInfo, charIndex, 0);

	ShowPaletteSelectPopup(charPalHandle, charIndex, popupID);
}

void PaletteEditorWindow::ShowPaletteSelectPopup(CharPaletteHandle& charPalHandle, CharIndex charIndex, const char* popupID)
{
	static int hoveredPalIndex = 0;
	bool pressed = false;
	int onlinePalsStartIndex = g_interfaces.pPaletteManager->GetOnlinePalsStartIndex(charIndex);
	ImGui::SetNextWindowSizeConstraints(ImVec2(-1.0f, 25.0f), ImVec2(-1.0f, 300.0f));

	if (ImGui::BeginPopup(popupID))
	{
		ImGui::TextUnformatted(getCharacterNameByIndexA(charIndex).c_str());
		ImGui::Separator();
		for (int i = 0; i < m_customPaletteVector[charIndex].size(); i++)
		{
			const IMPL_info_t& palInfo = m_customPaletteVector[charIndex][i].palInfo;

			if (i == onlinePalsStartIndex)
			{
				ImGui::PushStyleColor(ImGuiCol_Separator, COLOR_ONLINE);
				ImGui::Separator();
				ImGui::PopStyleColor();
			}

			if (ImGui::Selectable(palInfo.palName))
			{
				pressed = true;
				g_interfaces.pPaletteManager->SwitchPalette(charIndex, charPalHandle, i);

				// Updating palette editor's array if this is the currently selected character
				if (&charPalHandle == m_selectedCharPalHandle)
				{
					m_selectedPalIndex = i;
					CopyPalFileToEditorArray(m_selectedFile, charPalHandle);
					DisableHighlightModes();

					CopyImplDataToEditorFields(charPalHandle);
				}

				if (g_interfaces.pRoomManager->IsRoomFunctional())
				{
					g_interfaces.pOnlinePaletteManager->SendPalettePackets();
				}
			}

			if (ImGui::IsItemHovered())
			{
				hoveredPalIndex = i;
			}

			ShowHoveredPaletteInfoToolTip(palInfo, charIndex, i);
		}

		ImGui::EndPopup();
	}

	HandleHoveredPaletteSelection(&charPalHandle, charIndex, hoveredPalIndex, popupID, pressed);
}

void PaletteEditorWindow::ShowHoveredPaletteInfoToolTip(const IMPL_info_t& palInfo, CharIndex charIndex, int palIndex)
{
	if (!ImGui::IsItemHovered())
	{
		return;
	}

	const char* creatorText = palInfo.creator;
	const char* descText = palInfo.desc;
	const int creatorLen = strnlen(creatorText, IMPL_CREATOR_LENGTH);
	const int descLen = strnlen(descText, IMPL_DESC_LENGTH);
	bool isOnlinePal = palIndex >= g_interfaces.pPaletteManager->GetOnlinePalsStartIndex(charIndex);
	bool hasBloom = palInfo.hasBloom;

	if (creatorLen || descLen || isOnlinePal || hasBloom)
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(300.0f);

		if (isOnlinePal)
			ImGui::TextColored(COLOR_ONLINE, "ONLINE PALETTE");

		if (creatorLen)
			ImGui::Text("Creator: %s", creatorText);

		if (descLen)
			ImGui::Text("Description: %s", descText);

		if (hasBloom)
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Has bloom effect");

		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void PaletteEditorWindow::HandleHoveredPaletteSelection(CharPaletteHandle* charPalHandle, CharIndex charIndex, int palIndex, const char* popupID, bool pressed)
{
	static CharPaletteHandle* prevCharHndl = 0;
	static int prevPalIndex = 0;
	static int origPalIndex = 0;
	static bool paletteSwitched = false;
	static char popupIDbkp[32];
	const char* palFileAddr = 0;

	if (pressed)
	{
		paletteSwitched = false;
	}
	else if (!ImGui::IsPopupOpen(popupID) && strcmp(popupIDbkp, popupID) == 0 &&
		paletteSwitched && prevCharHndl == charPalHandle && !pressed)
	{
		palFileAddr = g_interfaces.pPaletteManager->GetCustomPalFile(charIndex, origPalIndex, PaletteFile_Character, *charPalHandle);
		g_interfaces.pPaletteManager->ReplacePaletteFile(palFileAddr, PaletteFile_Character, *charPalHandle);
		paletteSwitched = false;
	}
	else if (ImGui::IsPopupOpen(popupID) && prevPalIndex != palIndex)
	{
		if (!paletteSwitched)
		{
			origPalIndex = g_interfaces.pPaletteManager->GetCurrentCustomPalIndex(*charPalHandle);
		}

		palFileAddr = g_interfaces.pPaletteManager->GetCustomPalFile(charIndex, palIndex, PaletteFile_Character, *charPalHandle);
		g_interfaces.pPaletteManager->ReplacePaletteFile(palFileAddr, PaletteFile_Character, *charPalHandle);
		prevPalIndex = palIndex;
		prevCharHndl = charPalHandle;
		paletteSwitched = true;
		strcpy(popupIDbkp, popupID);
	}
}

void PaletteEditorWindow::ShowPaletteRandomizerButton(const char * btnID, Player& playerHandle)
{
	int charIndex = playerHandle.GetData()->charIndex;
	char buf[32];
	sprintf_s(buf, " ? ##%s", btnID);
	
	if (ImGui::Button(buf) && m_customPaletteVector[charIndex].size() > 1)
	{
		CharPaletteHandle& charPalHandle = playerHandle.GetPalHandle();
		int curPalIndex = g_interfaces.pPaletteManager->GetCurrentCustomPalIndex(charPalHandle);
		int newPalIndex = curPalIndex;

		while (curPalIndex == newPalIndex)
		{
			newPalIndex = rand() % m_customPaletteVector[charIndex].size();
		}

		g_interfaces.pPaletteManager->SwitchPalette((CharIndex)charIndex, charPalHandle, newPalIndex);

		if (g_interfaces.pRoomManager->IsRoomFunctional())
		{
			g_interfaces.pOnlinePaletteManager->SendPalettePackets();
		}
	}

	ImGui::HoverTooltip("Random selection");
}

void PaletteEditorWindow::CopyToEditorArray(const char * pSrc)
{
	memcpy(m_paletteEditorArray, pSrc, IMPL_PALETTE_DATALEN);
}

void PaletteEditorWindow::CopyPalFileToEditorArray(PaletteFile palFile, CharPaletteHandle & charPalHandle)
{
	const char* fileAddr = g_interfaces.pPaletteManager->GetCurPalFileAddr(palFile, charPalHandle);
	CopyToEditorArray(fileAddr);
}

void PaletteEditorWindow::UpdateHighlightArray(int selectedBoxIndex)
{
	static int previousSelectedBoxIndex = 0;

	if (previousSelectedBoxIndex == selectedBoxIndex)
		return;

	// Set previously pressed box back to black
	((int*)m_highlightArray)[previousSelectedBoxIndex] = COLOR_BLACK;

	// Set currently pressed box to white
	((int*)m_highlightArray)[selectedBoxIndex] = COLOR_WHITE;

	g_interfaces.pPaletteManager->ReplacePaletteFile(m_highlightArray, m_selectedFile, *m_selectedCharPalHandle);

	previousSelectedBoxIndex = selectedBoxIndex;
}

void PaletteEditorWindow::CopyImplDataToEditorFields(CharPaletteHandle & charPalHandle)
{
	const IMPL_info_t& palInfo = g_interfaces.pPaletteManager->GetCurrentPalInfo(charPalHandle);

	std::string newPalName = strncmp(palInfo.palName, "Default", IMPL_PALNAME_LENGTH) == 0
		? ""
		: palInfo.palName;

	strncpy(palNameBuf, newPalName.c_str(), IMPL_PALNAME_LENGTH);
	strncpy(palDescBuf, palInfo.desc, IMPL_DESC_LENGTH);
	strncpy(palCreatorBuf, palInfo.creator, IMPL_CREATOR_LENGTH);
	palBoolEffect = palInfo.hasBloom;
}

void PaletteEditorWindow::ShowGradientPopup()
{
	if (ImGui::BeginPopup("gradient"))
	{
		ImGui::TextUnformatted("Gradient generator");

		static int idx1 = 1;
		static int idx2 = 2;
		int minVal_idx2 = idx1 + 1;

		if (idx2 <= idx1)
		{
			idx2 = minVal_idx2;
		}

		ImGui::SliderInt("Start index", &idx1, 1, NUMBER_OF_COLOR_BOXES - 1);
		ImGui::SliderInt("End index", &idx2, minVal_idx2, NUMBER_OF_COLOR_BOXES);

		static int color1 = 0xFFFFFFFF;
		static int color2 = 0xFFFFFFFF;
		int alpha_flag = m_colorEditFlags & ImGuiColorEditFlags_NoAlpha;

		ImGui::ColorEdit4On32Bit("Start color", NULL,(unsigned char*)&color1, alpha_flag);
		ImGui::ColorEdit4On32Bit("End color", NULL, (unsigned char*)&color2, alpha_flag);

		if (ImGui::Button("Swap colors"))
		{
			int temp = color2;
			color2 = color1;
			color1 = temp;
		}

		if (ImGui::Button("Generate gradient"))
		{
			DisableHighlightModes();
			GenerateGradient(idx1, idx2, color1, color2);
		}

		ImGui::EndPopup();
	}
}

void PaletteEditorWindow::GenerateGradient(int idx1, int idx2, int color1, int color2)
{
	idx1 -= 1;
	idx2 -= 1;

	int steps = idx2 - idx1;
	if (steps < 1)
	{
		return;
	}

	float frac = 1.0 / (float)(idx2 - idx1);

	unsigned char a1 = (color1 & 0xFF000000) >> 24;
	unsigned char a2 = (color2 & 0xFF000000) >> 24;
	unsigned char r1 = (color1 & 0xFF0000) >> 16;
	unsigned char r2 = (color2 & 0xFF0000) >> 16;
	unsigned char g1 = (color1 & 0xFF00) >> 8;
	unsigned char g2 = (color2 & 0xFF00) >> 8;
	unsigned char b1 = color1 & 0xFF;
	unsigned char b2 = color2 & 0xFF;

	((int*)m_paletteEditorArray)[idx1] = color1;

	for (int i = 1; i <= steps; i++)
	{
		int a = ((int)((a2 - a1) * i * frac + a1) & 0xFF) << 24;
		int r = ((int)((r2 - r1) * i * frac + r1) & 0xFF) << 16;
		int g = ((int)((g2 - g1) * i * frac + g1) & 0xFF) << 8;
		int b = (int)((b2 - b1) * i * frac + b1) & 0xFF;
		int color = r | g | b;

		((int*)m_paletteEditorArray)[idx1 + i] = color ^ ((int*)m_paletteEditorArray)[idx1 + i] & a;
	}

	g_interfaces.pPaletteManager->ReplacePaletteFile(m_paletteEditorArray, m_selectedFile, *m_selectedCharPalHandle);
}