#pragma once
#include "impl_format.h"

#include "CharPaletteHandle.h"

#include "Game/characters.h"
#include "Game/Player.h"

#include <vector>

class PaletteManager
{
public:
	PaletteManager();
	~PaletteManager();
	std::vector<std::vector<IMPL_data_t>> &GetCustomPalettesVector();

	bool PushImplFileIntoVector(IMPL_t &filledPal);
	bool PushImplFileIntoVector(CharIndex charIndex, IMPL_data_t &filledPalData);
	bool WritePaletteToFile(CharIndex charIndex, IMPL_data_t *filledPalData);

	void LoadAllPalettes();
	void ReloadAllPalettes();

	int GetOnlinePalsStartIndex(CharIndex charIndex);
	void OverwriteIMPLDataPalName(std::string fileName, IMPL_data_t& palData);

	// Return values:
	// ret > 0, index found
	// ret == -1, index not found
	// ret == -2, charindex out of bound
	// ret == -3, default palette or no name given
	int FindCustomPalIndex(CharIndex charIndex, const char* palNameToFind);
	bool PaletteArchiveDownloaded();
	bool SwitchPalette(CharIndex charIndex, CharPaletteHandle& palHandle, int newCustomPalIndex);
	void ReplacePaletteFile(const char* newPalData, PaletteFile palFile, CharPaletteHandle& palHandle);
	void RestoreOrigPal(CharPaletteHandle& palHandle);
	const char* GetCurPalFileAddr(PaletteFile palFile, CharPaletteHandle& palHandle);
	const char* GetCustomPalFile(CharIndex charIndex, int palIndex, PaletteFile palFile, CharPaletteHandle& palHandle);
	int GetCurrentCustomPalIndex(CharPaletteHandle& palHandle) const;
	const IMPL_info_t& GetCurrentPalInfo(CharPaletteHandle& palHandle) const;
	void SetCurrentPalInfo(CharPaletteHandle& palHandle, IMPL_info_t& palInfo);
	const IMPL_data_t& GetCurrentPalData(CharPaletteHandle& palHandle);
	void LoadPaletteSettingsFile();

	// Call it ONCE per frame
	void OnUpdate(CharPaletteHandle& P1, CharPaletteHandle& P2);

	// Call it ONCE upon match start
	void OnMatchInit(Player& playerOne, Player& playerTwo);

	void OnMatchRematch(Player& playerOne, Player& playerTwo);
	void OnMatchEnd(CharPaletteHandle& playerOne, CharPaletteHandle& playerTwo);

private:
	std::vector<std::vector<IMPL_data_t>> m_customPalettes;
	std::vector<std::vector<std::string>> m_paletteSlots;
	std::vector<int> m_onlinePalsStartIndex;
	bool m_loadOnlinePalettes = false;
	bool m_PaletteArchiveDownloaded = false;

	void CreatePaletteFolders();
	void InitCustomPaletteVector();
	void LoadPalettesIntoVector(CharIndex charIndex, std::wstring& wFolderPath);
	void LoadPalettesFromFolder();
	void LoadImplFile(const std::string& fullPath, const std::string& fileName, CharIndex charIndex);
	void LoadHplFile(const std::string& fullPath, const std::string& fileName, CharIndex charIndex);
	void InitPaletteSlotsVector();
	void InitOnlinePalsIndexVector();
	void ApplyDefaultCustomPalette(CharIndex charIndex, CharPaletteHandle& charPalHandle);
};