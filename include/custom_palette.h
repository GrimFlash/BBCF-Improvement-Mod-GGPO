#pragma once
#include "../include/network_manager.h"

#define PALETTE_DATALEN 1020
#define HPAL_HEADER_LEN 0x24

bool HookPaletteFunctions();
DWORD* getPaletteArray(DWORD* base, DWORD palIndex, DWORD fileID);
void LoadPalettePacket(im_packet_internal_t *packet);
void LoadEffectPacket(im_packet_internal_t *packet);
void LoadBloomPacket(im_packet_internal_t *packet);
void CreateCustomFolders();
void LoadPaletteFiles();
void ReloadCustomPalettes();
void ReplaceP1Palette(bool sendToOpponent = true);
void ReplaceP2Palette(bool sendToOpponent = true);
void ResettingDefaultPalettes();
void ReplaceP1Palette_PaletteEditor(char* pPaletteData, int file);
void ReplaceP2Palette_PaletteEditor(char* pPaletteData, int file);
int FindCustomPaletteIndex(std::string paletteName, int charID);

extern char HPAL_header[HPAL_HEADER_LEN];

extern LPCWSTR ingame_chars[36];