#include "../../include/ImGui/ImGuiSystem.h"
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include "Shlwapi.h"
#include "../../include/ImGui/fonts.h"
#include "../../include/utils.h"
#include "../../include/update_check.h"
#include "../../include/containers.h"
#include "../../include/custom_palette.h"
#include "../../include/steamapi_helper.h"
#include "../../include/custom_gamemodes.h"
#include "../../include/gamestates_defines.h"
#include "../../include/bbcf_im_networking.h"
#include "../../include/internal_palette_datas.h"
#include "../../src/Game/game.h"

// reminder to myself, pitfall of using imgui in your project:
// ALWAYS ADD THE IMGUI SOURCE FILES TO YOUR PROJECT OR YOU WILL GET UNRESOLVED EXTERNAL SYMBOL ERRORS

#define MAX_LOG_MSG_LEN 1024

bool show_main_window = true;
bool show_demo_window = false;
bool show_notification = false;
bool show_notification_window = false;
bool show_log_window = false;
bool show_debug_window = false;
bool show_palette_editor_window = false;
bool ImGuiSystem::IsUpdateAvailable = false;
bool ImGuiSystem::DoLogging = true;
bool* NO_CLOSE_FLAG = NULL;
bool is_main_window_visible = false;
bool show_overwrite_popup = false;
bool ImGuiSystem::DrawHitbox = false;
bool show_ggpo_host_join_window = false;
bool show_ggpo_spectate_window = false;
bool show_save_load_state_window = false;

std::string notificationText;
float notificationTimer = 0;

bool ImGuiSystem::Initialized = false;
std::string main_title = "";

int toggle_key;

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ImGuiLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	float				PrevScrollMaxY = 0;

	void _Clear() { Buf.clear(); LineOffsets.clear(); }

	void _AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
		{
			if (Buf[old_size] == '\n')
			{
				LineOffsets.push_back(old_size);
			}
		}
	}

	void _Draw(const char* title, bool* p_open = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(title, p_open, ImGuiWindowFlags_NoCollapse);
		if (ImGui::Button("Clear")) _Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy)
		{
			ImGui::LogToClipboard();
			ImGuiSystem::AddLog("[system] Log has been copied to clipboard\n");
		}

		if (Filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
				if (Filter.PassFilter(line, line_end))
				{
					ImGui::TextUnformatted(line, line_end);
					//ImGui::TextWrapped(line, line_end); causes a line limit in the logging for some reason
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
			//ImGui::TextWrapped(Buf.begin()); //causes a line limit in the logging for some reason
		}

		//handle automatic scrolling
		if (PrevScrollMaxY < ImGui::GetScrollMaxY())
		{
			//scroll down automatically only if we didnt scroll up or we closed the window
			if (ImGui::GetScrollY() >= PrevScrollMaxY - 5)
			{
				ImGui::SetScrollY(ImGui::GetScrollMaxY());
			}
			PrevScrollMaxY = ImGui::GetScrollMaxY();
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void _ToFile(FILE *file)
	{
		fprintf(file, "%s", Buf.begin());
	}
};

ImGuiLog ImGuiSystem::Log;

void ImGuiSystem::SetMainWindowTitle(const char *text)
{
	if (text)
		main_title = text;
	else
	{
		main_title = "BBCF Improvement Mod + GGPO ";
		main_title += version_num;
#ifndef RELEASE_VER
		main_title += " (DEBUG) ";
#endif
	}
	main_title += "###MainTitle"; //set unique identifier
}

bool ImGuiSystem::Init(void *hwnd, IDirect3DDevice9 *device)
{
	if (Initialized)
		return true; 

	LOG(2, "ImGuiSystem::Init\n");

	if (!hwnd)
	{
		LOG(2, "HWND not found!\n");
		return false;
	}
	if (!device)
	{
		LOG(2, "Direct3DDevice9 not found!\n");
		return false;
	}

	bool ret = ImGui_ImplDX9_Init(hwnd, device);
	if (!ret)
	{
		LOG(2, "ImGui_ImplDX9_Init failed!\n");
		return false;
	}

	SetMainWindowTitle();

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 1;
	style.FrameBorderSize = 1;
	style.ScrollbarSize = 14;
	style.Alpha = 0.87;
	//style.ItemSpacing = ImVec2(8,3);
	//disable ini saving
	//ImGui::GetIO().IniFilename = NULL;
	
	if (Settings::settingsIni.menusize == 1)
	{
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(TinyFont_compressed_data_base85, 10);
		font->DisplayOffset.y += 1;
	}
	else if(Settings::settingsIni.menusize == 3)
		ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 20);
	
	Initialized = true;
	ImGuiSystem::AddLog("[system] Initialization starting...\n");


	toggle_key = Settings::getToggleButtonValue();
	ImGuiSystem::AddLog("[system] Toggling key set to '%s'\n", Settings::settingsIni.togglebutton.c_str());

	LoadPaletteFiles();

	if (Settings::settingsIni.checkupdates)
	{
		HANDLE thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheckUpdate, NULL, NULL, NULL);
		CloseHandle(thread);
	}

	Containers::g_interfaces.pSteamApiHelper = new SteamApiHelper(Containers::g_interfaces.pSteamUserStatsWrapper, Containers::g_interfaces.pSteamFriendsWrapper);
	//HANDLE thread2 = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FetchTotalIngamePlayers, NULL, NULL, NULL);
	//CloseHandle(thread2);

	srand(time(NULL));

	ImGuiSystem::AddLog("[system] Finished initialization\n");
	ImGuiSystem::AddLogSeparator();

	return ret;
}

void ImGuiSystem::Shutdown()
{
	if (!Initialized)
		return;

	LOG(2, "ImGuiSystem::Shutdown\n");
	ImGui_ImplDX9_Shutdown();
}

void ImGuiSystem::InvalidateDeviceObjects()
{
	if (!Initialized)
		return;

	LOG(2, "ImGuiSystem::InvalidateDeviceObjects\n");
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void ImGuiSystem::CreateDeviceObjects()
{
	if (!Initialized)
		return;

	LOG(2, "ImGuiSystem::CreateDeviceObjects\n");
	ImGui_ImplDX9_CreateDeviceObjects();
}

void ImGuiSystem::Render()
{
	if (!Initialized)
		return;

	LOG(7, "ImGuiSystem::Render\n");
	ImGui::Render();
}

void ImGuiSystem::HandleImGuiWindows()
{
	if (!Initialized)
		return;

	LOG(7, "ImGuiSystem::HandleImGui\n");
	ImGui_ImplDX9_NewFrame();

	//First run settings
	ImGui::SetNextWindowPos(ImVec2(12, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!

	if (Settings::settingsIni.menusize == 1)
		ImGui::SetNextWindowSizeConstraints(ImVec2(250, 190), ImVec2(1000, 1000));
	else if (Settings::settingsIni.menusize == 3)
		ImGui::SetNextWindowSizeConstraints(ImVec2(400, 230), ImVec2(1000, 1000));
	else
		ImGui::SetNextWindowSizeConstraints(ImVec2(330, 230), ImVec2(1000, 1000));

	ImGuiIO& io = ImGui::GetIO();

	io.MouseDrawCursor = show_log_window | show_notification_window | show_palette_editor_window | show_main_window | IsUpdateAvailable | show_ggpo_host_join_window | show_ggpo_spectate_window | show_save_load_state_window | show_demo_window;
	
	if (Settings::settingsIni.viewport == 2)
	{
		io.DisplaySize = ImVec2(Settings::settingsIni.renderwidth, Settings::settingsIni.renderheight);
	}
	else if (Settings::settingsIni.viewport == 3)
	{
		io.DisplaySize = ImVec2(1280, 768);
	}

	static game::SavedGameState game_state;
	if (ImGui::IsKeyPressed(119, false)) { // F8
		game_state = game::SaveGameState();
	}

	if (ImGui::IsKeyPressed(120, false)) { // F9
		game::LoadGameState(game_state);
	}

	if (ImGui::IsKeyPressed(toggle_key))
	{
		show_main_window = !show_main_window;
	}

	//toggle window on/off not working???
	if(show_main_window)
	{
		ImGui::Begin(main_title.c_str(), NO_CLOSE_FLAG, ImGuiWindowFlags_AlwaysAutoResize);

		is_main_window_visible = !ImGui::IsWindowCollapsed();

		ImGui::Text("Toggle me with %s", Settings::settingsIni.togglebutton.c_str());
		ImGui::Separator();
		ImGui::Text("");

		{
			if (Containers::gameVals.pGameMode != 0 && *Containers::gameVals.pGameMode == GAME_MODE_ONLINE)
			{
				//we are spectators
				if (Containers::gameVals.thisPlayerNum == 0)
				{
					ImGui::Text("Player1 using BBCFIM: "); ImGui::SameLine();
					if (Containers::gameVals.bP1UsingBBCFIM)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "YES (%s)", Containers::gameVals.sP1BBCFIMvernum.c_str());
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NO");

					ImGui::Text("Player2 using BBCFIM: "); ImGui::SameLine();
					if (Containers::gameVals.bP2UsingBBCFIM)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "YES (%s)", Containers::gameVals.sP2BBCFIMvernum.c_str());
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NO");
				}
				else
				{
					ImGui::Text("Opponent using BBCFIM: "); ImGui::SameLine();
					if (Containers::gameVals.bOpponentUsingBBCFIM)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "YES (%s)", Containers::gameVals.sOpponentBBCFIMvernum.c_str());
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NO");
				}
			}
			else
			{
				ImGui::Text("Opponent using BBCFIM: "); ImGui::SameLine();
				ImGui::TextDisabled("NOT PLAYING");
			}
		}

		ImGui::Text("P$"); ImGui::SameLine();
		if (Containers::gameVals.pGameMoney)
			ImGui::InputInt("##P$", *&Containers::gameVals.pGameMoney);

		if (ImGui::CollapsingHeader("Gameplay settings"))
		{
			ShowGameplaySettingsMenu();
		}

		if (ImGui::CollapsingHeader("Palette settings"))
		{
			ShowCustomPalettesMenu();
		}

		if (ImGui::CollapsingHeader("Avatar settings"))
		{
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::SliderInt("Avatar", Containers::gameVals.playerAvatarAddr, 0, 0x2F);
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::SliderInt("Color", Containers::gameVals.playerAvatarColAddr, 0, 0x3);
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::SliderByte("Accessory 1", Containers::gameVals.playerAvatarAcc1, 0, 0xCF);
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::SliderByte("Accessory 2", Containers::gameVals.playerAvatarAcc2, 0, 0xCF);
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("GGPO Netplay"))
		{
			ShowGGPONetplayMenu();
		}

		if (ImGui::CollapsingHeader("Loaded settings.ini values"))
		{
			//not using columns because its buggy with windows having always_autoresize flag, as the window 
			//gets extending beyond infinity if the left edge of the window touches the edge of the screen
			ImGui::Separator();
			ImGui::Text("\tToggleButton"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %s", Settings::settingsIni.togglebutton.c_str());
			ImGui::Separator();

			ImGui::Text("\tShowNotifications"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= "); ImGui::SameLine();
			ImGui::Checkbox("##shownotificationwindows", &Settings::settingsIni.notificationpopups);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To change it permanently, set the value in settings.ini then restart the game!");
				ImGui::EndTooltip();
			}
			ImGui::Separator();

			ImGui::Text("\tRenderingWidth"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.renderwidth);
			ImGui::Separator();

			ImGui::Text("\tRenderingHeight"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.renderheight);
			ImGui::Separator();

			ImGui::Text("\tViewport"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.viewport);
			ImGui::Separator();

			ImGui::Text("\tAntiAliasing"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.antialiasing); 
			ImGui::Separator();

			ImGui::Text("\tV-sync"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.vsync);
			ImGui::Separator();

			ImGui::Text("\tMenuSize"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.menusize);
			ImGui::Separator();

			ImGui::Text("\tRegionlock"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.regionlock); 
			ImGui::Separator();

			ImGui::Text("\tCpuUsageFix"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.cpuusagefix); 
			ImGui::Separator();

			ImGui::Text("\tCheckUpdates"); ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("= %d", Settings::settingsIni.checkupdates); 
			ImGui::Separator();
		}
#ifndef RELEASE_VER
		if (ImGui::Button("Debug"))
			show_debug_window ^= 1;
#endif
		if (ImGui::Button("Log"))
			show_log_window ^= 1;

		ImGui::Text("Current online players:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", 
			Containers::g_interfaces.pSteamApiHelper->current_players <= 0 ? "<No data>" : std::to_string(Containers::g_interfaces.pSteamApiHelper->current_players).c_str()); //GetIngamePlayersNum().c_str());

		if(ImGui::Button("Discord"))
			ShellExecute(NULL, L"open", L"https://discord.gg/j2mCX9s", NULL, NULL, SW_SHOWNORMAL);
		ImGui::SameLine();
		if (ImGui::Button("Forum"))
			ShellExecute(NULL, L"open", L"https://steamcommunity.com/app/586140/discussions/0/1291817208497395528/", NULL, NULL, SW_SHOWNORMAL);
		ImGui::SameLine();
		if (ImGui::Button("GitHub"))
			ShellExecute(NULL, L"open", L"https://github.com/kovidomi/BBCF-Improvement-Mod", NULL, NULL, SW_SHOWNORMAL);
	
		ImGui::End();
	}

	// 3. Show the windows!
#ifndef RELEASE_VER
	if (show_demo_window && show_main_window)
	{
		ImGui::SetNextWindowPos(ImVec2(550, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&show_demo_window);
	}
	if(show_debug_window && show_main_window)
		ShowDebugWindow(&show_debug_window);
#endif
	if (show_ggpo_host_join_window)
		ShowGgpoHostJoinWindow(&show_ggpo_host_join_window);

	if (show_ggpo_spectate_window)
		ShowGgpoSpectateWindow(&show_ggpo_spectate_window);

	if (show_save_load_state_window)
		ShowSaveLoadStateWindow(&show_save_load_state_window);
	
	if (show_palette_editor_window && show_main_window)
		ShowPaletteEditorWindow(&show_palette_editor_window);

	if (show_notification)
		HandleNotification();

	if (show_log_window && show_main_window) //show_main_window to make it togglable along with the main window
		ShowLogWindow(&show_log_window);

	if (IsUpdateAvailable)
		ShowUpdateWindow();

	// Rendering
	ImGui::EndFrame();
}

void ImGuiSystem::ShowDebugWindow(bool* p_open)
{
	ImGui::Begin("Debug", p_open);

	ImGui::Checkbox("Show hitbox", &DrawHitbox);

	if (ImGui::CollapsingHeader("ImGui frame values"))
	{
		ImGui::Text("\tApplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("\tDeltaTime: %f", ImGui::GetIO().DeltaTime);
		ImGui::Text("\tFrameCount: %d", ImGui::GetFrameCount());
	}
	if (ImGui::CollapsingHeader("Game Values"))
	{
		ImGui::Text("\tGameState: %d", *Containers::gameVals.pGameState);
		ImGui::Text("\tGameMode: %d", Containers::gameVals.pGameMode == 0 ? -1 : *Containers::gameVals.pGameMode);

		if (Containers::gameVals.ownSteamID != 0)
			ImGui::Text("\townSteamID: 0x%x", *Containers::gameVals.ownSteamID);
		else
			ImGui::Text("\townSteamID: 0x%x", 0);

		if (Containers::gameVals.playersInMatch.size() > 0)
		{
			for (int i = 0; i < Containers::gameVals.playersInMatch.size(); i++)
			{
				ImGui::Text("\tOpponent SteamID: 0x%x", Containers::gameVals.playersInMatch[i]);
			}
		}
		else
			ImGui::Text("\tOpponent SteamID: 0");

		//if (Containers::gameVals.opponentSteamID != 0)
		//	ImGui::Text("\tOpponent SteamID: 0x%x", *Containers::gameVals.opponentSteamID);
		//else
		//	ImGui::Text("\tOpponent SteamID: 0x%x", 0);

		ImGui::Text("\tthisPlayerNum: %d", Containers::gameVals.thisPlayerNum);

		ImGui::Text("\ttotalReadPackets: %d", Containers::gameVals.totalReadPackets);

		ImGui::Text("\tpMatchState: 0x%x : %d", Containers::gameVals.pMatchState, *Containers::gameVals.pMatchState);
		ImGui::Text("\tpMatchTimer: 0x%x : %d", Containers::gameVals.pMatchTimer, *Containers::gameVals.pMatchTimer);
		ImGui::Text("\tpMatchRounds: 0x%x : %d", Containers::gameVals.pMatchRounds, Containers::gameVals.pMatchRounds == 0 ? -1 : *Containers::gameVals.pMatchRounds);
		ImGui::Text("\tisP1CPU: %d", Containers::gameVals.isP1CPU);
		ImGui::Text("\tstageListMemory: 0x%x", Containers::gameVals.stageListMemory);
	}
	if (ImGui::CollapsingHeader("Player Objects"))
	{
		ImGui::Text("\tP1CharObjAddr: 0x%x", Containers::gameVals.P1CharObjPointer);
		ImGui::Text("\tP2CharObjAddr: 0x%x", Containers::gameVals.P2CharObjPointer);
		ImGui::Text("\tP1SelectedCharID: %d", Containers::gameVals.P1_selectedCharID);
		ImGui::Text("\tP2SelectedCharID: %d", Containers::gameVals.P2_selectedCharID);
		ImGui::Text("\tP1_is_doing_distortion: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->is_doing_distortion);
		ImGui::Text("\tP2_is_doing_distortion: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->is_doing_distortion);
		ImGui::Text("\tP1_curHP: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->currentHP);
		ImGui::Text("\tP1_maxHP: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->maxHP);
		ImGui::Text("\tP2_curHP: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->currentHP);
		ImGui::Text("\tP2_maxHP: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->maxHP);
		ImGui::Text("\tP1_xPos: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->xPos);
		ImGui::Text("\tP1_yPos: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->yPos);
		ImGui::Text("\tP2_xPos: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->xPos);
		ImGui::Text("\tP2_yPos: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->yPos);
		ImGui::Text("\tP1_effectPointer: %d", Containers::gameVals.P1CharObjPointer == 0 ? -1 : Containers::gameVals.P1CharObjPointer->effectPointer);
		ImGui::Text("\tP2_effectPointer: %d", Containers::gameVals.P2CharObjPointer == 0 ? -1 : Containers::gameVals.P2CharObjPointer->effectPointer);
		ImGui::Text("\tP1_selected_custom_pal: %d", Containers::gameVals.P1_selected_custom_pal);
		ImGui::Text("\tP2_selected_custom_pal: %d", Containers::gameVals.P2_selected_custom_pal);
		ImGui::Text("\tP1PaletteIndex:");
		if (Containers::gameVals.P1PaletteIndex)
		{
			ImGui::SameLine();
			ImGui::InputInt("##P1PaletteIndex", *&Containers::gameVals.P1PaletteIndex);
		}
		ImGui::Text("\tP2PaletteIndex:");
		if (Containers::gameVals.P2PaletteIndex)
		{
			ImGui::SameLine();
			ImGui::InputInt("##P2PaletteIndex", *&Containers::gameVals.P2PaletteIndex);
		}
		ImGui::Text("\torigP1PaletteIndex:%d", Containers::gameVals.origP1PaletteIndex);
		ImGui::Text("\torigP2PaletteIndex:%d", Containers::gameVals.origP2PaletteIndex);
		ImGui::Text("\tP1ScreenPosX: 0x%x : %.2f", Containers::gameVals.P1ScreenPosX, *Containers::gameVals.P1ScreenPosX);
	}

	if (ImGui::CollapsingHeader("Notifications"))
	{
		//use this only to align the button horizontally
		ImGui::Text("\t"); ImGui::SameLine();
		if (ImGui::Button("Show example notification"))
		{
			SetNotification("TEST TEST", 10);
		}
		ImGui::Text("\t"); ImGui::SameLine();
		if (ImGui::Button("Show example notification with window"))
		{
			SetNotification("TEST TEST", 10, true);
		}

		ImGui::Text("\t"); ImGui::SameLine();
		if (ImGui::Button("Show update notification"))
		{
			CheckUpdate();
		}

		ImGui::Text("\t"); ImGui::SameLine();
		if (ImGui::Button("Editor")) // Use buttons to toggle our bools. We could use Checkbox() as well.
			show_demo_window ^= 1;
	}

	bool pressed = ImGui::Button("Open Save/Load state window");
	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("NOT IN MATCH");
	}
	if (pressed)
		show_save_load_state_window ^= 1;

	if (ImGui::Button("Force VS mode"))
	{
		game::ForceVersusMode();
	}

	ImGui::End();
}

void ImGuiSystem::ShowSaveLoadStateWindow(bool* p_open)
{
	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH)
		return;

	static game::SavedGameState savedState;
	static int nFramesToSkipRender = 0;
	static int nMinSkip = 0;
	static int nMaxSkip = 60;
	static int nFrameStep = 1;

	ImGui::Begin("Save/Load State", p_open);
	
	if (nFramesToSkipRender < 0) {
		nFramesToSkipRender = 0;
	}
	else if (nFramesToSkipRender > 60) {
		nFramesToSkipRender = 60;
	}

	ImGui::InputInt("Num of frames to skip", &nFramesToSkipRender);

	if (ImGui::Button("Save")) {

		savedState = game::SaveGameState();

	}

	if (ImGui::Button("Load")) {
		game::LoadGameState(savedState);
		/*
		lpGameState->nFramesSkipped = 0;
		lpGameState->nFramesToSkipRender = nFramesToSkipRender;
		LoadGameState(lpGameState, &savedState);
		*/
	}

	ImGui::EndPopup();

	/*This will be the workspace for getting the save/load state functioning,
	Which in turn proves that game loop and visual loop can be separated*/
}

void ImGuiSystem::ShowCustomPalettesMenu()
{
	// 6 = character selection screen, 14 = versus screen, 15 = in match
	if (*Containers::gameVals.pGameState != GAME_STATE_CHARACTER_SELECTION_SCREEN && 
		*Containers::gameVals.pGameState != GAME_STATE_VERSUS_SCREEN &&
		*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH)
	{
		ImGui::Text("\t"); ImGui::SameLine();
		ImGui::Button("P1 palette"); ImGui::SameLine(); ImGui::TextDisabled("NOT IN MATCH");

		ImGui::Text("\t"); ImGui::SameLine();
		ImGui::Button("P2 palette"); ImGui::SameLine(); ImGui::TextDisabled("NOT IN MATCH");
	}
	else
	{
		//prevent array out of bounds
		if (Containers::gameVals.P1_selectedCharID != -1)
		{
			if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID].size() <= Containers::gameVals.P1_selected_custom_pal)
				Containers::gameVals.P1_selected_custom_pal = 0;
		}
		if (Containers::gameVals.P2_selectedCharID != -1)
		{
			if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID].size() <= Containers::gameVals.P2_selected_custom_pal)
				Containers::gameVals.P2_selected_custom_pal = 0;
		}


		if (Containers::gameVals.opponentSteamID == 0 || Containers::gameVals.thisPlayerNum == 1 || *Containers::gameVals.pGameState == GAME_STATE_CHARACTER_SELECTION_SCREEN)
		{
			if (Containers::gameVals.P1_selectedCharID == -1)
			{
				ImGui::Text("\t"); ImGui::SameLine();
				ImGui::Button("P1 palette"); ImGui::SameLine(); ImGui::TextDisabled("NO CHARACTER SELECTED");
			}
			else
			{
				ImGui::Text("\t"); ImGui::SameLine();
				if (ImGui::Button("?##P1"))
				{
					if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID].size() > 2)
					{
						int orig_pal = Containers::gameVals.P1_selected_custom_pal;
						while (orig_pal == Containers::gameVals.P1_selected_custom_pal)
						{
							Containers::gameVals.P1_selected_custom_pal = rand() % Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID].size();
							if (Containers::gameVals.P1_selected_custom_pal == 0)
								Containers::gameVals.P1_selected_custom_pal = 1;
							LOG(2, "%d - %d\n", orig_pal, Containers::gameVals.P1_selected_custom_pal);
						}
						ReplaceP1Palette();
					}
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Pick randomly from the list");
					ImGui::EndTooltip();
				}
				ImGui::SameLine();
				if (ImGui::Button("P1 palette"))
					ImGui::OpenPopup("select1");
				ImGui::SameLine();
				ImGui::TextUnformatted(Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][0].c_str());
				if (ImGui::BeginPopup("select1"))
				{
					std::wstring ws(ingame_chars[Containers::gameVals.P1_selectedCharID]);
					std::string charName(ws.begin(), ws.end());
					ImGui::Text(charName.c_str());
					ImGui::Separator();
					for (int i = 0; i < Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID].size(); i++)
					{
						bool clicked = ImGui::Selectable(Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][0].c_str());
						if (ImGui::IsItemHovered() && i != 0)
						{
							//preventing null exceptions
							if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][9][0] == 1 ||
								Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][10] != "" ||
								Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][11] != "")
							{
								//control size of tooltip:
								//ImGui::PushItemWidth();
								//ImGui::PopItemWidth();
								ImGui::BeginTooltip();
								if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][10] != "")
									ImGui::Text("Creator: %s", Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][10].c_str());
								if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][11] != "")
									ImGui::Text("Description: %s", Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][11].c_str());
								if (Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][i][9][0] == 1)
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Has bloom effect");
								ImGui::EndTooltip();
							}
						}
						if (clicked)
						{
							Containers::gameVals.P1_selected_custom_pal = i;
							ReplaceP1Palette();
						}
					}
					ImGui::EndPopup();
				}
			}
		}

		if (Containers::gameVals.opponentSteamID == 0 || Containers::gameVals.thisPlayerNum == 2 || *Containers::gameVals.pGameState == GAME_STATE_CHARACTER_SELECTION_SCREEN)
		{
			if (Containers::gameVals.P2_selectedCharID == -1)
			{
				ImGui::Text("\t"); ImGui::SameLine();
				ImGui::Button("P2 palette"); ImGui::SameLine(); ImGui::TextDisabled("NO CHARACTER SELECTED");
			}
			else
			{
				ImGui::Text("\t"); ImGui::SameLine();
				if (ImGui::Button("?##P2"))
				{
					if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID].size() > 2)
					{
						int orig_pal = Containers::gameVals.P2_selected_custom_pal;
						while (orig_pal == Containers::gameVals.P2_selected_custom_pal)
						{
							Containers::gameVals.P2_selected_custom_pal = rand() % Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID].size();
							if (Containers::gameVals.P2_selected_custom_pal == 0)
								Containers::gameVals.P2_selected_custom_pal = 1;
							LOG(2, "%d - %d\n", orig_pal, Containers::gameVals.P2_selected_custom_pal);
						}
						ReplaceP2Palette();
					}
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Pick randomly from the list");
					ImGui::EndTooltip();
				}
				ImGui::SameLine();
				if (ImGui::Button("P2 palette"))
					ImGui::OpenPopup("select2");
				ImGui::SameLine();
				ImGui::TextUnformatted(Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][0].c_str());
				if (ImGui::BeginPopup("select2"))
				{
					std::wstring ws(ingame_chars[Containers::gameVals.P2_selectedCharID]);
					std::string charName(ws.begin(), ws.end());
					ImGui::Text(charName.c_str());
					ImGui::Separator();
					for (int i = 0; i < Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID].size(); i++)
					{
						bool clicked = ImGui::Selectable(Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][0].c_str());
						if (ImGui::IsItemHovered() && i != 0)
						{
							//preventing null exceptions
							if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][9][0] == 1 ||
								Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][10] != "" ||
								Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][11] != "")
							{
								//control size of tooltip:
								//ImGui::PushItemWidth();
								//ImGui::PopItemWidth();
								ImGui::BeginTooltip();
								if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][10] != "")
									ImGui::Text("Creator: %s", Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][10].c_str());
								if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][11] != "")
									ImGui::Text("Description: %s", Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][11].c_str());
								if (Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][i][9][0] == 1)
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Has bloom effect");
								ImGui::EndTooltip();
							}
						}
						if (clicked)
						{
							Containers::gameVals.P2_selected_custom_pal = i;
							ReplaceP2Palette();
						}
					}
					ImGui::EndPopup();
				}
			}
		}
	}

	ImGui::Text("");
	ImGui::Text("\t"); ImGui::SameLine();
	bool pressed = ImGui::Button("Palette editor");
	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH || *Containers::gameVals.pGameMode != GAME_MODE_TRAINING)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("NOT IN TRAINING MODE");
	}
	if (pressed)
		show_palette_editor_window ^= 1;

	ImGui::Text("\t"); ImGui::SameLine();
	if (ImGui::Button("Reload custom palettes"))
	{
		ReloadCustomPalettes();
	}
}

void ImGuiSystem::ShowGGPONetplayMenu()
{
	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE || *Containers::gameVals.pGameMode == GAME_STATE_LOBBY)
	{
		ImGui::Button("Host/Join"); ImGui::SameLine(); ImGui::TextDisabled("CANNOUT USE DURING ONLINE MODES");

		ImGui::Button("Spectate"); ImGui::SameLine(); ImGui::TextDisabled("CANNOUT USE DURING ONLINE MODES");
	}
	else
	{
		if (ImGui::Button("Host/Join"))
		{
			show_ggpo_host_join_window ^= 1;
		}

		if (ImGui::Button("Spectate"))
		{
			show_ggpo_spectate_window ^= 1;
		}

	}
}

void ImGuiSystem::ShowGgpoHostJoinWindow(bool* p_open)
{
	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE || *Containers::gameVals.pGameMode == GAME_STATE_LOBBY)
		return;

	ImGui::Begin("Host/Join", p_open);

	ImGui::Text("Example text");

	ImGui::EndPopup();
}

void ImGuiSystem::ShowGgpoSpectateWindow(bool* p_open)
{
	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE || *Containers::gameVals.pGameMode == GAME_STATE_LOBBY)
		return;

	ImGui::Begin("Spectate", p_open);

	ImGui::Text("Example text");

	ImGui::EndPopup();
}

void ImGuiSystem::SetNotification(const char *text, float timeToShowInSec, bool showNotificationWindow)
{
	if (!Initialized)
		return;

	notificationText = text;
	notificationTimer = timeToShowInSec;
	show_notification = true;
	show_notification_window = showNotificationWindow & Settings::settingsIni.notificationpopups;
}

void ImGuiSystem::HandleNotification()
{
	std::ostringstream stringBuf;
	stringBuf << notificationText << " (" << round(ceil(notificationTimer)) << ")";
	SetMainWindowTitle(stringBuf.str().c_str());

	if (notificationTimer < 0.0)
	{
		show_notification_window = false;
		show_notification = false;
		SetMainWindowTitle(); // reset title to default
	}

	if (show_notification_window)
		ShowNotificationWindow();

	notificationTimer -= ImGui::GetIO().DeltaTime;
}

void ImGuiSystem::ShowNotificationWindow()
{
	//middle of screen
	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, 50), ImVec2(500, 500));
	ImVec2 OK_btn_size = ImVec2(100, 30);

	ImGui::Begin("Notification", NO_CLOSE_FLAG, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text(notificationText.c_str());
	//ImGui::CalcTextSize(notificationText.c_str()); ImGui::SetCursorPosX(ImGui::CalcTextSize(notificationText.c_str()).x);

	std::ostringstream stringBuf;
	stringBuf << "OK (" << round(ceil(notificationTimer)) << ")";
	std::string timeLeft(stringBuf.str());

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - (OK_btn_size.x / 2));

	if (ImGui::Button(timeLeft.c_str(), OK_btn_size))
	{
		show_notification_window = false;
		notificationTimer = -0.2; //set it below 0, but close to 0
	}

	ImGui::End();
}

// start with type a of message: "[system]", "[info]", "[warning]", "[error]", "[fatal]", "[notice]", "[log]"
void ImGuiSystem::AddLog(const char* message, ...)
{
	if (!Initialized) { return; }
	if (!message) { return; }
	if (!DoLogging) { return; }

	//Get current time into a string
	//////
	time_t current_time;
	struct tm * time_info;
	char timeString[9];  // space for "HH:MM:SS\0"

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
	/////

	if (strlen(message) > MAX_LOG_MSG_LEN)
	{
		LOG(2, "AddLog error: message too long!\nmessage: %s", message);
		ImGuiSystem::Log._AddLog("%s [error] Log message too long.", timeString);
		return;
	}

	std::string fullMessage(timeString);

	char buf[MAX_LOG_MSG_LEN];
	va_list args;
	va_start(args, message);
	vsprintf(buf, message, args);
	va_end(args);

	fullMessage += " ";
	fullMessage += buf;

	ImGuiSystem::Log._AddLog(fullMessage.c_str());
}

void ImGuiSystem::AddLogSeparator()
{
	if(DoLogging)
		ImGuiSystem::Log._AddLog("------------------------------------------------------------------\n");
}

void ImGuiSystem::WriteLogToFile()
{
	if (!Initialized)
		return;

	FILE *file = fopen("BBCF_IM\\log.txt", "a");

	if (!file)
	{
		LOG(2, "ImGuiSystem::WriteLogToFile file opening failed!!\n");
		return;
	}

	char* time = getFullDate();

	if (time)
	{
		fprintf(file, "BBCFIM %s -- %s\n", version_num, time);
		free(time);
	}
	else
	{
		fprintf(file, "{Couldn't get the current time}\n");
	}
	
	fprintf(file, "-------------------------------------\n\n");
	fprintf(file, "settings.ini config:\n");
	fprintf(file, "\t- ToggleButton: %s\n", Settings::settingsIni.togglebutton.c_str());
	fprintf(file, "\t- NotificationPopups: %d\n", Settings::settingsIni.notificationpopups);
	fprintf(file, "\t- RenderingWidth: %d\n", Settings::settingsIni.renderwidth);
	fprintf(file, "\t- RenderingHeight: %d\n", Settings::settingsIni.renderheight);
	fprintf(file, "\t- Viewport: %d\n", Settings::settingsIni.viewport);
	fprintf(file, "\t- AntiAliasing: %d\n", Settings::settingsIni.antialiasing);
	fprintf(file, "\t- V-Sync: %d\n", Settings::settingsIni.vsync);
	fprintf(file, "\t- Regionlock: %d\n", Settings::settingsIni.regionlock);
	fprintf(file, "\t- CpuUsageFix: %d\n", Settings::settingsIni.cpuusagefix);
	fprintf(file, "\t- CheckUpdates: %d\n\n", Settings::settingsIni.checkupdates);
	ImGuiSystem::Log._ToFile(file);
	fprintf(file, "\n#####################################\n\n\n");

	fclose(file);
}

void ImGuiSystem::ShowLogWindow(bool* p_open)
{
	// Demo: add random items (if Ctrl is held)
//#ifndef RELEASE_VER
//	static float last_time = -1.0f;
//	float time = ImGui::GetTime();
//	if (time - last_time >= 0.20f && ImGui::GetIO().KeyCtrl)
//	{
//		const char* random_words[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
//		ImGuiSystem::AddLog("[%s] Hello, time is %.1f, frame count is %d\n", random_words[rand() % IM_ARRAYSIZE(random_words)], time, ImGui::GetFrameCount());
//		last_time = time;
//	}
//#endif
	ImGuiSystem::Log._Draw("Log", p_open);
}

void ImGuiSystem::ShowUpdateWindow()
{
	//middle of screen
	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, 50), ImVec2(500, 500));
	ImVec2 OK_btn_size = ImVec2(100, 30);

	ImGui::Begin("Update available", &IsUpdateAvailable, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("BBCF Improvement Mod %s has been released!", GetNewVersionNum().c_str());
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - (OK_btn_size.x / 2));

	if (ImGui::Button("OK", OK_btn_size))
	{
		IsUpdateAvailable = false;
	}
	ImGui::End();
}

void ImGuiSystem::ShowPaletteEditorWindow(bool* p_open)
{
	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH || *Containers::gameVals.pGameMode != GAME_MODE_TRAINING)
		return;

	ImGui::Begin("Palette editor", p_open);

	static int selected_character = Containers::gameVals.P1_selectedCharID;
	static int prev_selectedP1Char = Containers::gameVals.P1_selectedCharID;
	static int prev_selectedP2Char = Containers::gameVals.P2_selectedCharID;
	static std::string files[] = { "Character", "Effect01", "Effect02", "Effect03", "Effect04", "Effect05", "Effect06", "Effect07" };
	static char* pBackupPal = Containers::tempVals.PaletteEditorP1PalBackup;
	static bool show_alpha = false;
	static int selected_box = 0;
	static int prev_selected_box = 0;
	//char highlight_arr[PALETTE_DATALEN];
	static char highlight_orig_backup[PALETTE_DATALEN] = {};
	static int color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha;

	//reset variables upon switching characters via training's quick char selection
	if (prev_selectedP1Char != Containers::gameVals.P1_selectedCharID || prev_selectedP2Char != Containers::gameVals.P2_selectedCharID)
	{
		selected_character = Containers::gameVals.P1_selectedCharID;
		Containers::gameVals.paletteEditor_selectedFile = 0;
		Containers::gameVals.paletteEditor_selectedPlayer = 0;
		pBackupPal = Containers::tempVals.PaletteEditorP1PalBackup;
		prev_selectedP1Char = Containers::gameVals.P1_selectedCharID;
		prev_selectedP2Char = Containers::gameVals.P2_selectedCharID;
	}

	//////////////////////////////// SELECT CHARACTER

	if (ImGui::Button("Select character"))
		ImGui::OpenPopup("select_char_pal");
	ImGui::SameLine();

	std::wstring ws(ingame_chars[selected_character]);
	std::string charName(ws.begin(), ws.end());
	ImGui::Text(charName.c_str());

	if (ImGui::BeginPopup("select_char_pal"))
	{
		static std::vector<std::string> chars(2);
		std::wstring wsChar1(ingame_chars[Containers::gameVals.P1_selectedCharID]);
		std::wstring wsChar2(ingame_chars[Containers::gameVals.P2_selectedCharID]);
		chars[0] = std::string(wsChar1.begin(), wsChar1.end());
		chars[1] = std::string(wsChar2.begin(), wsChar2.end());
		for (int i = 0; i < 2; i++)
		{
			ImGui::PushID(i);
			if(ImGui::Selectable(chars[i].c_str()))
			{
				//restore original palette
				if (Containers::tempVals.paledit_show_sel_by_highlight || Containers::tempVals.paledit_show_placeholder)
				{
					Containers::tempVals.paledit_show_sel_by_highlight = false;
					Containers::tempVals.paledit_show_placeholder = false;
					memcpy(pBackupPal, highlight_orig_backup, PALETTE_DATALEN);
					if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
						ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
					else
						ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
				}

				if (i == 0)
				{
					selected_character = Containers::gameVals.P1_selectedCharID;
					memcpy(Containers::tempVals.PaletteEditorP1PalBackup, getPaletteArray(Containers::gameVals.P1PaletteBase, 
						*Containers::gameVals.P1PaletteIndex, Containers::gameVals.paletteEditor_selectedFile), PALETTE_DATALEN);
					pBackupPal = Containers::tempVals.PaletteEditorP1PalBackup;
				}
				else
				{
					selected_character = Containers::gameVals.P2_selectedCharID;
					memcpy(Containers::tempVals.PaletteEditorP2PalBackup, getPaletteArray(Containers::gameVals.P2PaletteBase,
						*Containers::gameVals.P2PaletteIndex, Containers::gameVals.paletteEditor_selectedFile), PALETTE_DATALEN);
					pBackupPal = Containers::tempVals.PaletteEditorP2PalBackup;
				}
				Containers::gameVals.paletteEditor_selectedPlayer = i;
				ImGui::PopID();
			}
		}
		ImGui::EndPopup();
	}

	//////////////////////////////// SELECT PALETTE

	if (ImGui::Button("Select palette  "))
		ImGui::OpenPopup("select_custom_pal");
	ImGui::SameLine();
	if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
		ImGui::Text(Containers::gameVals.customPalettes[selected_character][Containers::gameVals.P1_selected_custom_pal][0].c_str());
	else
		ImGui::Text(Containers::gameVals.customPalettes[selected_character][Containers::gameVals.P2_selected_custom_pal][0].c_str());

	if (ImGui::BeginPopup("select_custom_pal"))
	{
		for (int i = 0; i < Containers::gameVals.customPalettes[selected_character].size(); i++)
		{
			bool clicked = ImGui::Selectable(Containers::gameVals.customPalettes[selected_character][i][0].c_str());
			if (ImGui::IsItemHovered() && i != 0)
			{
				//preventing null exceptions
				if (Containers::gameVals.customPalettes[selected_character][i][9][0] == 1 ||
					Containers::gameVals.customPalettes[selected_character][i][10] != "" ||
					Containers::gameVals.customPalettes[selected_character][i][11] != "")
				{
					//control size of tooltip:
					//ImGui::PushItemWidth();
					//ImGui::PopItemWidth();
					ImGui::BeginTooltip();
					if (Containers::gameVals.customPalettes[selected_character][i][10] != "")
						ImGui::Text("Creator: %s", Containers::gameVals.customPalettes[selected_character][i][10].c_str());
					if (Containers::gameVals.customPalettes[selected_character][i][11] != "")
						ImGui::Text("Description: %s", Containers::gameVals.customPalettes[selected_character][i][11].c_str());
					if (Containers::gameVals.customPalettes[selected_character][i][9][0] == 1)
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Has bloom effect");
					ImGui::EndTooltip();
				}
			}
			if (clicked)
			{
				if (Containers::tempVals.paledit_show_sel_by_highlight || Containers::tempVals.paledit_show_placeholder)
				{
					Containers::tempVals.paledit_show_sel_by_highlight = false;
					Containers::tempVals.paledit_show_placeholder = false;
				}

				if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				{
					Containers::gameVals.P1_selected_custom_pal = i;
					ReplaceP1Palette();
				}
				else
				{
					Containers::gameVals.P2_selected_custom_pal = i;
					ReplaceP2Palette();
				}
			}
		}
		ImGui::EndPopup();
	}

	//////////////////////////////// SELECT FILE

	if (ImGui::Button("Select file     "))
		ImGui::OpenPopup("select_file_pal");
	ImGui::SameLine();

	ImGui::Text(files[Containers::gameVals.paletteEditor_selectedFile].c_str());

	if (ImGui::BeginPopup("select_file_pal"))
	{
		for (int i = 0; i < 8; i++)
		{
			if (ImGui::Selectable(files[i].c_str()))
			{
				//restore original palette
				if (Containers::tempVals.paledit_show_sel_by_highlight || Containers::tempVals.paledit_show_placeholder)
				{
					Containers::tempVals.paledit_show_sel_by_highlight = false;
					Containers::tempVals.paledit_show_placeholder = false;
					memcpy(pBackupPal, highlight_orig_backup, PALETTE_DATALEN);
					if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
						ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
					else
						ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
				}

				Containers::gameVals.paletteEditor_selectedFile = i;
				if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				{
					memcpy(Containers::tempVals.PaletteEditorP1PalBackup, getPaletteArray(Containers::gameVals.P1PaletteBase,
						*Containers::gameVals.P1PaletteIndex, Containers::gameVals.paletteEditor_selectedFile), PALETTE_DATALEN);
					pBackupPal = Containers::tempVals.PaletteEditorP1PalBackup;
				}
				else
				{
					memcpy(Containers::tempVals.PaletteEditorP2PalBackup, getPaletteArray(Containers::gameVals.P2PaletteBase,
						*Containers::gameVals.P2PaletteIndex, Containers::gameVals.paletteEditor_selectedFile), PALETTE_DATALEN);
					pBackupPal = Containers::tempVals.PaletteEditorP2PalBackup;
				}
			}
		}
		ImGui::EndPopup();
	}

	if (ImGui::Checkbox("Show transparency values", &show_alpha))
	{
		if(show_alpha)
			color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar;
		else
			color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha;
	}

	//if (show_placeholder)
	//{
	//	//forcing checkbox to be false constantly
	//	ImGui::Button("", ImVec2(19, 19));
	//	ImGui::SameLine();
	//	ImGui::TextDisabled("Highlight mode");
	//}
	//else 
	if (ImGui::Checkbox("Highlight mode", &Containers::tempVals.paledit_show_sel_by_highlight))
	{
		if (Containers::tempVals.paledit_show_sel_by_highlight)
		{
			if (Containers::tempVals.paledit_show_placeholder)
				Containers::tempVals.paledit_show_placeholder = false;
			else
				memcpy(highlight_orig_backup, pBackupPal, PALETTE_DATALEN); //save original into backup
			//fill the array with black
			int step = 1;
			for (int i = 0; i < PALETTE_DATALEN; i++, step++)
			{
				if (step % 4 == 0)
				{
					pBackupPal[i] = 0xFF;
					step = 0;
				}
				else
					pBackupPal[i] = 0;
			}
			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
			else
				ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
		}
		else
		{
			memcpy(pBackupPal, highlight_orig_backup, PALETTE_DATALEN);
			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
			else
				ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
		}
	}

	if (Containers::gameVals.paletteEditor_selectedFile == 0)
	{
		//if (show_sel_by_highlight)
		//{
		//	//forcing checkbox to be false constantly
		//	ImGui::Button("", ImVec2(19, 19));
		//	ImGui::SameLine();
		//	ImGui::TextDisabled("Placeholder mode");
		//	//ImGui::ColorButtonOn32Bit("##PalColorButton", (unsigned char*)pBackupPal + (i * 4), color_edit_flags); 
		//}
		//else 
		if (ImGui::Checkbox("Placeholder mode", &Containers::tempVals.paledit_show_placeholder))
		{
			if (Containers::tempVals.paledit_show_placeholder)
			{
				if (Containers::tempVals.paledit_show_sel_by_highlight)
					Containers::tempVals.paledit_show_sel_by_highlight = false;
				else
					memcpy(highlight_orig_backup, pBackupPal, PALETTE_DATALEN); //save original into backup
																			//fill the array with black
				if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				{
					memcpy(pBackupPal, placeholder_palettes[Containers::gameVals.P1_selectedCharID], PALETTE_DATALEN);
					ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
				}
				else
				{
					memcpy(pBackupPal, placeholder_palettes[Containers::gameVals.P2_selectedCharID], PALETTE_DATALEN);
					ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
				}
			}
			else
			{
				memcpy(pBackupPal, highlight_orig_backup, PALETTE_DATALEN);
				if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
					ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
				else
					ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
			}
		}
	}

	////////////////////////////////// COLOR PALETTE ARRAY
	ImGui::Text("");
	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

	for (int i = 0, col = 0; i < 255; i++)
	{
		ImGui::PushID(i);

		if(i == 0) //for spacing stuff
		{
			ImGui::Text("");
			col++;
			ImGui::SameLine(29);
		}

		bool changed = false;
		bool pressed = false;

		if (Containers::tempVals.paledit_show_sel_by_highlight)
			pressed = ImGui::ColorButtonOn32Bit("##PalColorButton", (unsigned char*)pBackupPal + (i * 4), color_edit_flags);
		else
			changed = ImGui::ColorEdit4On32Bit("##PalColorEdit", (unsigned char*)pBackupPal + (i * 4), color_edit_flags);
		if (changed || pressed)
		{
			if (Containers::tempVals.paledit_show_sel_by_highlight)
			{
				prev_selected_box = selected_box;
				selected_box = i * 4;
				pBackupPal[prev_selected_box] = 0;
				pBackupPal[prev_selected_box+1] = 0;
				pBackupPal[prev_selected_box+2] = 0;
				pBackupPal[prev_selected_box+3] = 0xFF;
				pBackupPal[selected_box] = 0xFF;
				pBackupPal[selected_box+1] = 0xFF;
				pBackupPal[selected_box+2] = 0xFF;
				pBackupPal[selected_box+3] = 0xFF;
				//selected_box = i;
				//((int*)pBackupPal)[prev_selected_box] = 0x000000FF; //doesnt work, for some reason it becomes 0x0000FF00
				//((int*)pBackupPal)[selected_box] = 0xFFFFFFFF;
			}
			
			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				ReplaceP1Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
			else
				ReplaceP2Palette_PaletteEditor(pBackupPal, Containers::gameVals.paletteEditor_selectedFile);
		}
		if (col < 15)
		{
			ImGui::SameLine();
			col++;
		}
		else
			col = 0;
		ImGui::PopID();
	}
	ImGui::PopStyleVar();

	//////////////////////////////////// SAVING

	static char buf1[MAX_PATH] = "";
	static char message[200] = "";
	static bool save_bloom = false;

	ImGui::Text("");
	ImGui::Separator();

	if (Containers::tempVals.paledit_show_sel_by_highlight)
	{
		ImGui::TextDisabled("Cannot save with Highlight mode on!");
		ImGui::End();
		return;
	}

	if (Containers::gameVals.paletteEditor_selectedFile == 0)
	{
		ImGui::Checkbox("Save with bloom effect", &save_bloom);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Bloom effects cannot be changed until a new round is started");
			ImGui::EndTooltip();
		}
	}

	ImGui::PushItemWidth(250);
	struct TextFilters { static int FilterAllowedChars(ImGuiTextEditCallbackData* data) { if (data->EventChar < 256 && strchr(" qwertzuiopasdfghjklyxcvbnmQWERTZUIOPASDFGHJKLYXCVBNM0123456789_.()[]!@&+-'^,;{}$=", (char)data->EventChar)) return 0; return 1; } };
	//ImGui::InputText("##savepal", buf1, MAX_PATH); ImGui::SameLine();
	ImGui::InputText("##savepal", buf1, MAX_PATH, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterAllowedChars); ImGui::SameLine();
	ImGui::PopItemWidth();

	bool pressed = ImGui::Button("Save palette");
	ImGui::Text(message);

	if (pressed || show_overwrite_popup)
	{
		if (strcmp(buf1, "") == 0)
		{
			memcpy(message, "Error, no filename given", 25);
			ImGuiSystem::AddLog("[error] Could not save custom palette, no filename was given\n");
			ImGui::End();
			return;
		}

		TCHAR pathBuf[MAX_PATH];
		GetModuleFileName(NULL, pathBuf, MAX_PATH);
		std::wstring::size_type pos = std::wstring(pathBuf).find_last_of(L"\\");
		std::wstring fullPath = std::wstring(pathBuf).substr(0, pos);

		fullPath += L"\\BBCF_IM\\Palettes\\";
		fullPath += ingame_chars[selected_character];
		fullPath += L"\\";

		std::string filenameTemp(buf1);
		std::wstring filename(filenameTemp.begin(), filenameTemp.end());
		fullPath += filename;

		std::wstring bloomfilefullpath = fullPath;
		bloomfilefullpath += L"_effectbloom.hpl";

		if (Containers::gameVals.paletteEditor_selectedFile > 0 && filename.find(L"_effect0") == std::wstring::npos)
		{
			fullPath += L"_effect0";
			fullPath += std::to_wstring(Containers::gameVals.paletteEditor_selectedFile);
			filenameTemp += "_effect0";
			filenameTemp += std::to_string(Containers::gameVals.paletteEditor_selectedFile);
		}

		if (filename.find(L".hpl") == std::wstring::npos)
		{
			fullPath += L".hpl";
			filenameTemp += ".hpl";
		}

		std::string path(fullPath.begin(), fullPath.end());

		//hpal header
		std::string content(HPAL_header, HPAL_HEADER_LEN);
		content.append(pBackupPal, PALETTE_DATALEN);

		//overwrite popup code start
		if (PathFileExists(fullPath.c_str()))
		{
			ImGui::OpenPopup("Overwrite?");
			show_overwrite_popup = true;
		}
		if (ImGui::BeginPopupModal("Overwrite?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static bool overwrite = false;
			ImGui::Text("'%s' already exists.\nAre you sure you want to overwrite it?\n\n", filenameTemp.c_str());
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0))) 
			{ 
				ImGui::CloseCurrentPopup();
				show_overwrite_popup = false;
				overwrite = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) 
			{ 
				ImGui::CloseCurrentPopup(); 
				show_overwrite_popup = false;
				overwrite = false;
			}
			ImGui::EndPopup();
			if(!overwrite)
				return;
			overwrite = false;
		}
		//overwrite popup code end

		bool isFileClosed = false;
		HANDLE file = CreateFile(fullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE) 
		{
			ImGuiSystem::AddLog("[error] File %s could not be created. Error: %u\n", path.c_str(), GetLastError());
			memcpy(message, "Error, check log", 17);
			ImGui::End();
			return;
		}
		DWORD dwBytesWritten;
		if (!WriteFile(file, content.c_str(), content.length(), &dwBytesWritten, NULL))
		{
			ImGuiSystem::AddLog("[error] File %s was not written to. Error: %u\n", path.c_str(), GetLastError());
			memcpy(message, "Error, check log", 17);
		}
		else
		{
			ImGuiSystem::AddLog("[system] Custom palette '%s' saved successfully to:\n%s\n", filenameTemp.c_str(), path.c_str());
			std::string messageText = "'";
			messageText += filenameTemp.c_str();
			messageText += "' saved successfully";
			memcpy(message, messageText.c_str(), messageText.length() + 1);

			if (save_bloom)
			{
				HANDLE bloomfile = CreateFile(bloomfilefullpath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (bloomfile == INVALID_HANDLE_VALUE)
				{
					ImGuiSystem::AddLog("[error] Bloom effect file could not be created. Error: %u\n", GetLastError());
					memcpy(message, "Error, check log", 17);
				}
				ImGuiSystem::AddLog("[system] Bloom effect file for '%s' saved successfully\n", filenameTemp.c_str());
				CloseHandle(bloomfile);
			}

			if (file != INVALID_HANDLE_VALUE)
				isFileClosed = CloseHandle(file);

			int prevSelectedPal = 0;
			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				prevSelectedPal = Containers::gameVals.P1_selected_custom_pal;
			else
				prevSelectedPal = Containers::gameVals.P2_selected_custom_pal;

			DoLogging = false; //dont log
			ReloadCustomPalettes();
			DoLogging = true;

			//find loaded file
			int selectedCharID = 0;
			int foundPaletteIndex = 0;
			bool isEffectFile = false;

			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				selectedCharID = Containers::gameVals.P1_selectedCharID;
			else
				selectedCharID = Containers::gameVals.P2_selectedCharID;

			if (filenameTemp.find("_effect0") != std::string::npos)
				isEffectFile = true;

				if (isEffectFile)
					foundPaletteIndex = FindCustomPaletteIndex(filenameTemp.substr(0, filenameTemp.rfind("_effect0")), selectedCharID);
				else
					foundPaletteIndex = FindCustomPaletteIndex(filenameTemp.substr(0, filenameTemp.rfind('.')), selectedCharID);

			if (!foundPaletteIndex)
			{
				if (isEffectFile)
					ImGuiSystem::AddLog("[error] '%s' has no custom character palette to match with! Create a character palette named '%s' to load this effect file on!\n",
						filenameTemp.c_str(), (filenameTemp.substr(0, filenameTemp.rfind("_effect0")) + ".hpl").c_str());
				else
					ImGuiSystem::AddLog("[error] '%s' wasn't loaded properly, try pressing the 'Reload custom palettes' button!\n", filenameTemp.c_str());

				DoLogging = false;
				if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
				{
					if(foundPaletteIndex < Containers::gameVals.customPalettes[selectedCharID].size())
						Containers::gameVals.P1_selected_custom_pal = prevSelectedPal;

					ReplaceP1Palette();
					ReplaceP1Palette();//doing it twice to not set it back to the original index
				}
				else
				{
					if (foundPaletteIndex < Containers::gameVals.customPalettes[selectedCharID].size())
						Containers::gameVals.P2_selected_custom_pal = prevSelectedPal;

					ReplaceP2Palette();
					ReplaceP2Palette();//doing it twice to not set it back to the original index
				}
				DoLogging = true;
				memcpy(message, "Saved successfully, error loading! Check log", 45);
				return;
			}

			DoLogging = false; //dont log
			if (Containers::gameVals.paletteEditor_selectedPlayer == 0)
			{
				Containers::gameVals.P1_selected_custom_pal = foundPaletteIndex;
				ReplaceP1Palette();
				ReplaceP1Palette(); //doing it twice to not set it back to the original index
			}
			else
			{
				Containers::gameVals.P2_selected_custom_pal = foundPaletteIndex;
				ReplaceP2Palette();
				ReplaceP2Palette(); //doing it twice to not set it back to the original index
			}
			DoLogging = true;

			ImGuiSystem::AddLog("[system] Custom palette '%s' successfully added to the custom palettes menu\n", filenameTemp.c_str());
		}

		if(file != INVALID_HANDLE_VALUE && isFileClosed == false)
			CloseHandle(file);
	}

	ImGui::End();
}

void ImGuiSystem::ShowGameplaySettingsMenu()
{
	static unsigned char selectedStage = 0;
	ImGui::Text("\t"); ImGui::SameLine();

	if (Containers::gameVals.pGameMode == 0 || Containers::gameVals.pGameState == 0)
	{
		ImGui::TextDisabled("Not available");
		return;
	}

	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE ||
		*Containers::gameVals.pGameMode == GAME_MODE_TRAINING ||
		*Containers::gameVals.pGameMode == GAME_MODE_VERSUS)
	{
		if (ImGui::SliderByte("Stage", &selectedStage, 0, 78))
		{
			*Containers::gameVals.stageSelect_X = stages[selectedStage][0];
			*Containers::gameVals.stageSelect_Y = stages[selectedStage][1];
		}
	}
	else
		ImGui::TextDisabled("Stage selection not available");

	std::string selectedGameMode = GameModes[activatedGameMode].name;
	selectedGameMode += " Mode";

	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE)
	{
		//we are spectators
		if (Containers::gameVals.thisPlayerNum == 0)
		{
			std::string P1SelectedGameMode = "";
			if (P1_activatedGameMode < GameModes.size())
				P1SelectedGameMode = GameModes[P1_activatedGameMode].name;
			else
				P1SelectedGameMode = "<UNKNOWN>";
			P1SelectedGameMode += " Mode";

			std::string P2SelectedGameMode = "";
			if (P2_activatedGameMode < GameModes.size())
				P2SelectedGameMode = GameModes[P2_activatedGameMode].name;
			else
				P2SelectedGameMode = "<UNKNOWN>";
			P2SelectedGameMode += " Mode";

			ImGui::Text("\t"); ImGui::SameLine(); ImGui::Text("Player1's selection: ");
			ImGui::PushID("135"); //random
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::RadioButton(P1SelectedGameMode.c_str(), true);
			ImGui::PopID();
			ImGui::Text("");

			ImGui::Text("\t"); ImGui::SameLine(); ImGui::Text("Player2's selection: ");
			ImGui::PushID("169"); //random
			ImGui::Text("\t"); ImGui::SameLine(); ImGui::RadioButton(P2SelectedGameMode.c_str(), true);
			ImGui::PopID();

			return;
		}

		//we are the one playing
		std::string opSelectedGameMode = "";
		if (opponent_activatedGameMode < GameModes.size())
			opSelectedGameMode = GameModes[opponent_activatedGameMode].name;
		else
			opSelectedGameMode = "<UNKNOWN>";
		opSelectedGameMode += " Mode";
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::Text("Opponent's selection: ");
		ImGui::PushID("123"); //random
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::RadioButton(opSelectedGameMode.c_str(), true);
		ImGui::PopID();
		ImGui::Separator();
		ImGui::Text("");
	}

	if (*Containers::gameVals.pGameState != GAME_STATE_CHARACTER_SELECTION_SCREEN && *Containers::gameVals.pGameState != GAME_STATE_REPLAY_MENU)
	{
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::RadioButton(selectedGameMode.c_str(), true);
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::TextDisabled("NOT ON CHARACTER SELECTION SCREEN");
		//ImGui::Text("\t"); ImGui::SameLine(); ImGui::TextDisabled("NOT ON REPLAY MENU SCREEN");
	}
	else if (*Containers::gameVals.pGameMode != GAME_MODE_VERSUS && *Containers::gameVals.pGameMode != GAME_MODE_ONLINE && 
		*Containers::gameVals.pGameMode != GAME_MODE_TRAINING && *Containers::gameVals.pGameState != GAME_STATE_REPLAY_MENU)//we use gamestate for replay menu, since gamemode sets to 0, if we return to the replay menu via the pause menu
	{
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::RadioButton(selectedGameMode.c_str(), true);
		ImGui::Text("\t"); ImGui::SameLine(); ImGui::TextDisabled("NOT IN ONLINE, TRAINING, OR VERSUS MODES");
	}
	else
	{
		for (int i = 0; i < GameModes.size(); i++)
		{
			ImGui::Text("\t"); ImGui::SameLine();
			selectedGameMode = GameModes[i].name;
			selectedGameMode += " Mode";
			//prevent some game modes being played in training, due to instadeath on match start
			if ((i == customGameMode_onepunch || i == customGameMode_twopunch || i == customGameMode_fivepunch || i == customGameMode_tugofwar) &&
				*Containers::gameVals.pGameMode == GAME_MODE_TRAINING)
			{
				ImGui::RadioButton(selectedGameMode.c_str(), false);
			}
			else
			{
				if (ImGui::RadioButton(selectedGameMode.c_str(), &activatedGameMode, GameModes[i].id) && Containers::gameVals.opponentSteamID != 0)
					Send_customGamemode_request(*Containers::gameVals.opponentSteamID);
			}
			if (ImGui::IsItemHovered() && GameModes[i].desc)
			{
				ImGui::BeginTooltip();
				ImGui::Text(GameModes[i].desc);
				ImGui::EndTooltip();
			}
		}
	}
}
