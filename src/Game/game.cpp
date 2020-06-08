#include "game.h"

namespace game {

using CharData = std::array<unsigned char, 0x214C4>;

std::unique_ptr<GameState> gGameState;
std::unique_ptr<CharData> gP1Data;
std::unique_ptr<CharData> gP2Data;

/// <summary>
/// Gets the pointer data pointers in BBCF's memory, so we can access and write to them later for
/// saving and loading state
/// </summary>
///
/// <param name="base">The pointer address to the player object (note: NOT BBCF's base address)</param>
/// <param name="player_data">The player data object to write the addresses into</param>
/// <param name="player">String representation of what player this is for logging purposes e.g. "player 1" or "player 2"</param>
static void GetPlayerPointers(uintptr_t base, PlayerData& player_data, std::string const& player)
{
    auto get_address_or_log = [](std::string const& name, uintptr_t base, auto offsets) {
        uintptr_t addr = FindAddress(base, offsets);

        if (!addr) {
            LOG(2, ("Could not find address for " + name).c_str());
        }

        return addr;
    };

    player_data.health = (int*)get_address_or_log(
        player + " health",
        base,
        pointer_offsets::player_common::health
    );

    player_data.x_pos = (int*)get_address_or_log(
        player + " xpos",
        base,
        pointer_offsets::player_common::xpos
    );

    player_data.y_pos = (int*)get_address_or_log(
        player + " ypos",
        base,
        pointer_offsets::player_common::ypos
    );
}

void InitGameStatePointers()
{
    gGameState = std::make_unique<GameState>();
    gP1Data = std::make_unique<CharData>();
    gP2Data = std::make_unique<CharData>();

    auto get_address_or_log = [](std::string const& name, uintptr_t base, auto offsets) {
        uintptr_t addr = FindAddress(base, offsets);

        if (!addr) {
            LOG(2, ("Could not find address for " + name).c_str());
        }

        return addr;
    };

    uintptr_t base = (uintptr_t)Containers::gameProc.hBBCFGameModule;
    gGameState->time = (int*)(base + pointer_offsets::time);
    GetPlayerPointers(base + pointer_offsets::player1, gGameState->player1, "Player 1");
    GetPlayerPointers(base + pointer_offsets::player2, gGameState->player2, "Player 2");
}

}

//Hold off on working on this page for now