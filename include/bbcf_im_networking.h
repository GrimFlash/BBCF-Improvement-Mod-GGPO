#pragma once
#include "stdafx.h"
#include "../include/network_manager.h"
#include <steam_api.h>

void Send_BBCFIM_ID(CSteamID opponentSteamID);
void Receive_BBCFIM_ID(im_packet_internal_t *packet);

void Send_customGamemode_request(CSteamID opponentSteamID);
void Receive_customGamemode_request(im_packet_internal_t *packet);