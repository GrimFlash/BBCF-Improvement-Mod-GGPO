#pragma once

#include <windows.h>
#include <d3d9.h>
#include <dinput.h>
#include <ggponet.h>

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
typedef struct PlayerData PlayerData;
typedef struct SavedGameState SavedGameState;

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


typedef struct GameState {

    int nFramesToSkipRender;
    int nFramesSkipped;
    int lastSecondNumFramesSimulated;

   GGPOState ggpoState;
   SessionInitiationState sessionInitState;

   //Struct definitons for GameObjectData

   //Camera values

   PlayerData* arrPlayerData;
   int* nRoundTimeRemaining;

   //Will update this more as I get a clearer idea on what exactly we'll need

 

} GameState;

void SaveGameState(GameState* gameState, SavedGameState* dest);
void LoadGameState(GameState* gameState, SavedGameState* src);

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


struct PlayerData {

    /*
    Here we will need to put in stuff related to characters, ranging
    from current heat to character specific stuff (i.e Susan seals)
    */

    enum PlayerDataAllowedNormals allowedNormals;

};



typedef struct SavedGameState {


} SavedGameState;