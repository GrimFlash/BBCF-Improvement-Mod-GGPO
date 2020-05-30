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
    NORMAL_J5A =
    NORMAL_J2B =
    NORMAL_J5B =
    NORMAL_J2C =
    NORMAL_J5C =
    NORMAL_J2D =
    NORMAL_J4D =
    NORMAL_J5D =
    NORMAL_J6D =
    NORMAL_J8D =

} PlayerDataAllowedNormals;
*/

/*
struct RandomNumberGenerator {
   
};
*/

/*
typedef struct ClientSynchronizationRequest {

} ClientSynchronizationRequest;

(GGPO related, don't worry about this for now)
*/

/*
typedef struct ServerSynchronizationResponse {
 
} ServerSynchronizationResponse;

(GGPO related, don't worry about this for now)
*/

typedef struct CharacterSelection {
    char* name;
    int value;
} CharacterSelection;

/*
static CharacterSelection CHARACTERS[] = {
    {"Charactername here", <hexadecimal # here},

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


  (This will be a part of making save/load state


} GameMethods;
*/

/*typedef struct GGPOState {
  
  (GGPO related, don't worry about this for now)

} GGPOState;
*/

/*
typedef struct SessionInitiationState {

(GGPO related, don't worry about this for now)

} SessionInitializationState;
*/

/*
typedef struct PlayData {
 

} PlayData;
*/

/*
typedef struct CharacterConstants {


} CharacterConstants;
*/

/*
typedef struct GameState {

} GameState;
*/

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

/*
struct PlayerData {

};
*/

/*
typedef struct SavedGameState {


} SavedGameState;
*/
