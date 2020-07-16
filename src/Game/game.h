#pragma once

#include <windows.h>
#include <d3d9.h>
#include <dinput.h>
#include <ggponet.h>
#include <memory>
#include <array>

#include "../../include/containers.h"
#include "../../include/logger.h"

namespace game {

/*
const unsigned short MIN_HEAT = 0;
const unsigned short MAX_HEAT = <put value here>;
const unsigned short MIN_BURST = 0;
const unsigned short MAX_BURST = <put value here>;
const unsigned short MIN_HEALTH = <put value here>;
const unsigned short MAX_HEALTH = <put value here>;
const unsigned short MAX_BARRIER = <put value here>;
const unsigned short MIN_BARRIER = <put value here>;
const short MIN_NEGATIVE_PENALTY = 0;
const short MAX_NEGATIVE_PENALTY = <put value here> ;

For the burst and barrier values, i'm not 100% sure if they are signed or not. If they are, just remove "unsigned" next to them - Grim
(Feel Free to delete this comment when we confirm it)
*/

/*
enum gameinputs : unsigned short {
    Up = 
	Right = 
	Down = 
	Left = 
	Taunt = 
	A = 
	B = 
	C = 
	D =  
	Reset = 
	Pause = 
};
*/

/*
typedef enum HitboxTypes {
  

} HitboxTypes;
*/

/*
typedef struct SpriteInfo {
    

} SpriteInfo;
*/

/*
typedef enum PlayerDataAllowedNormals {
    ALLNORMALS = 

    NORMAL_2A = 
    NORMAL_4A = 
    NORMAL_5A = 
    NORMAL_6A = 
    NORMAL_2B = 
    NORMAL_4B = 
    NORMAL_5B = 
    NORMAL_6B = 
    NORMAL_1C = 
    NORMAL_2C = 
    NORMAL_3C = 
    NORMAL_4C = 
    NORMAL_5C = 
    NORMAL_6C = 
    NORMAL_1D =
    NORMAL_2D = 
    NORMAL_3D =
    NORMAL_4D =
    NORMAL_5D =
    NORMAL_6D =
    NORMAL_7D =
    NORMAL_8D =
    NORMAL_9D =
    NORMAL_J2A =
    NORMAL_J5A =
    NORMAL_J2B =
    NORMAL_J5B =
    NORMAL_J6B =
    NORMAL_J2C =
    NORMAL_J5C =
    NORMAL_J2D =
    NORMAL_J4D =
    NORMAL_J5D =
    NORMAL_J6D =
    NORMAL_J7D =
    NORMAL_J8D =
    NORMAL_J9D =

} PlayerDataAllowedNormals;
*/

typedef struct GameObjectData GameObjectData;
typedef struct SavedGameState SavedGameState;

typedef struct PlayerData {

    /*
    Here we will need to put in stuff related to characters, ranging
    from current heat to character specific stuff (i.e Susan seals)
    */

    int* health;

    int* x_pos;
    int* y_pos;

    //enum PlayerDataAllowedNormals allowedNormals;

} PlayerData;

/*
struct RandomNumberGenerator {
   
};
*/


typedef struct ClientSynchronizationRequest {

    unsigned short nPort;
    int nSelectedCharacter;

} ClientSynchronizationRequest;



typedef struct ServerSynchronizationResponse {

    unsigned short nPort;
    int nSelectedCharacter;
    //Reminder to make sure to put RNG structs in here

} ServerSynchronizationResponse;



typedef struct CharacterSelection {
    char* name;
    int value;
} CharacterSelection;

/*
static CharacterSelection CHARACTERS[] = {
    {"Charactername here", <hexadecimal # here>},

    repeat for every character
 
};
*/

typedef struct StageSelection {
    char* name;
    int value;
} StageSelection;

/*
static StageSelection STAGES[] = {
    {"Stage name", hexidecimal #},

    repeat for every character
};
*/

struct InputRewriteStruct {
    bool left;
    bool down;
    bool up;
    bool right;
    bool a;
    bool b;
    bool c;
    bool d;
    bool taunt;
};

/*
typedef struct GameMethods {


} GameMethods;
*/

typedef struct GGPOState {
  
    GGPOSession* ggpo;
    GGPOPlayer p1;
    GGPOPlayer p2;
    GGPOPlayer* localPlayer;
    GGPOPlayer* remotePlayer;
    GGPOPlayerHandle player_handles[2];
    GGPOSessionCallbacks cb;
    GGPOErrorCode lastResult;
    int localPlayerIndex;
    int characters[2];
    char bIsSynchronized;
    int nFramesAhead;

} GGPOState;



typedef struct SessionInitiationState {

    ClientSynchronizationRequest request;
    ServerSynchronizationResponse response;
    bool bHasRequest;
    bool bHasResponse;
    bool bIsHost;
   

} SessionInitializationState;


/*
typedef struct PlayData {
 

} PlayData;
*/

/*
typedef struct CharacterConstants {


} CharacterConstants;
*/

template <std::size_t T>
static uintptr_t FindAddress(uintptr_t base, std::array<unsigned int, T> const& offsets)
{
    if (offsets.size() == 0) {
        return base;
    }

    for (std::size_t i = 0; i < offsets.size(); ++i) {
        base = *(uintptr_t*)base;

        if (base == NULL) {
            return NULL;
        }

        base += offsets[i];
    }

    return base;
}

namespace pointer_offsets {
    static const unsigned int time      = 0xDA0CE8;
    static const unsigned int player1   = 0x819DF0;
    static const unsigned int player2   = 0xDC204C;

    static const unsigned int MainMenuCategory = 0xE19EB0;
    static const unsigned int BattleHighlightedMode = 0xE14158;
    static const unsigned int ModeSelection = 0xE133A0;
    static const unsigned int SelectedPlayer1 = 0xE19F3C;
    static const unsigned int SelectedPlayer2 = 0xE19F40;

    namespace player_common {
        static const std::array<unsigned int, 1> health = { 0x9D4 };
        static const std::array<unsigned int, 1> xpos   = { 0x268 };
        static const std::array<unsigned int, 1> ypos   = { 0x26C };
    }
}

typedef struct GameState {

    //int nFramesToSkipRender;
    //int nFramesSkipped;
    //int lastSecondNumFramesSimulated;

   //GGPOState ggpoState;
   //SessionInitiationState sessionInitState;

   //Struct definitons for GameObjectData

   //Camera values

   int* time;
   PlayerData player1;
   PlayerData player2;

   //Will update this more as I get a clearer idea on what exactly we'll need

 

} GameState;

extern std::unique_ptr<GameState> gGameState;

void InitGameStatePointers();

extern std::unique_ptr<std::array<unsigned char, 0x214C4 >> gP1Data;
extern std::unique_ptr<std::array<unsigned char, 0x214C4 >> gP2Data;

typedef struct SavedGameState {

    struct Player {
        int health;
        int x_pos;
        int y_pos;
    };

    int time;
    Player p1;
    Player p2;

} SavedGameState;

static SavedGameState SaveGameState()
{
    SavedGameState game_state;

    if (gGameState) {
        game_state.time = *gGameState->time;
        /*
        game_state.p1.health = *gGameState->player1.health;
        game_state.p1.x_pos = *gGameState->player1.x_pos;
        game_state.p1.y_pos = *gGameState->player1.y_pos;

        game_state.p2.health = *gGameState->player2.health;
        game_state.p2.x_pos = *gGameState->player2.x_pos;
        game_state.p2.y_pos = *gGameState->player2.y_pos;*/
    }
    
    auto base = (uintptr_t)Containers::gameProc.hBBCFGameModule;
    auto p1_ref= (uintptr_t*)(base + pointer_offsets::player1);
    auto p1_dref = *p1_ref;
    auto p2_ref= (uintptr_t*)(base + pointer_offsets::player2);
    auto p2_dref = *p2_ref;
    logGameState((uintptr_t*)(base + pointer_offsets::time),p1_ref,p2_ref);
    std::memcpy(gP1Data->data(), (unsigned char*)(p1_dref), 0x214C4);
    std::memcpy(gP2Data->data(), (unsigned char*)(p2_dref), 0x214C4);
    return game_state;
}

static void LoadGameState(SavedGameState const& game_state)
{
    if (gGameState) {
        *gGameState->time = game_state.time;

        /**gGameState->player1.health = game_state.p1.health;
        *gGameState->player1.x_pos = game_state.p1.x_pos;
        *gGameState->player1.y_pos = game_state.p1.y_pos;

        *gGameState->player2.health = game_state.p2.health;
        *gGameState->player2.x_pos = game_state.p2.x_pos;
        *gGameState->player2.y_pos = game_state.p2.y_pos;*/
    }
    auto base = (uintptr_t)Containers::gameProc.hBBCFGameModule;
    auto p1_dref = *(uintptr_t*)(base + pointer_offsets::player1);
    auto p2_dref = *(uintptr_t*)(base + pointer_offsets::player2);
    std::memcpy((unsigned char*)p1_dref, gP1Data->data(), 0x214C4);
    std::memcpy((unsigned char*)p2_dref, gP2Data->data(), 0x214C4);
}

static void ForceVersusMode()
{
    DWORD pid;
    auto base = (uintptr_t)Containers::gameProc.hBBCFGameModule;
    DWORD MainMenuCategory = (base + pointer_offsets::MainMenuCategory);
    DWORD ModeSelection = (base + pointer_offsets::ModeSelection);
    DWORD BattleHighlightedMode = (base + pointer_offsets::BattleHighlightedMode);
    DWORD SelectedPlayer1 = (base + pointer_offsets::SelectedPlayer1);
    DWORD SelectedPlayer2 = (base + pointer_offsets::SelectedPlayer2);
  
    int MainMenuCategoryValue = 2;
    int BattleHighlightedModeValue = 1;
    int ModeSelectionValue = 19;

    int SelectedPlayer1Value = 0;
    int SelectedPlayer2Value = 1;
    //Reminder to self - When you start working on online connectivity, you'll need to put these two ints in a conditional

    HWND hWnd = FindWindowA(0, ("BLAZBLUE CENTRALFICTION"));

    GetWindowThreadProcessId(hWnd, &pid);
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    WriteProcessMemory(pHandle, (LPVOID)MainMenuCategory, &MainMenuCategoryValue, sizeof(MainMenuCategoryValue), 0);
    WriteProcessMemory(pHandle, (LPVOID)BattleHighlightedMode, &BattleHighlightedModeValue, sizeof(BattleHighlightedModeValue), 0);
    WriteProcessMemory(pHandle, (LPVOID)ModeSelection, &ModeSelectionValue, sizeof(ModeSelectionValue), 0);
    WriteProcessMemory(pHandle, (LPVOID)SelectedPlayer1, &SelectedPlayer1Value, sizeof(SelectedPlayer1Value), 0);
    WriteProcessMemory(pHandle, (LPVOID)SelectedPlayer2, &SelectedPlayer2Value, sizeof(SelectedPlayer2Value), 0);
}

/*
struct GameObjectScriptingStruct {



};
*/

/*
typedef enum GameObjectID {

<List game objects and their IDs here>

} GameObjectID;
*/

/*
typedef enum GameObjectStateFlags {

<List game object state flags and their IDs here (i.e Blockstun)>

} GameObjectStateFlags;
*/

/*
typedef enum GameObjectGraphicalEffectsFlags {



} GameObjectGraphicalEffectsFlags;
*/

/*
typedef enum GameObjectAttackInformationFlags {



} GameObjectAttackInformationFlags;
*/

/*
typedef struct GameObjectAttackInformation {
	


} GameObjectAttackInformation;
*/

/*
struct GameObjectData {

};
*/


}