#pragma once
#include "CharData.h"

#include "Palette/CharPaletteHandle.h"

class Player
{
public:
	CharData* GetData() const;
	CharPaletteHandle& GetPalHandle();

	void SetCharDataPtr(const void* addr);
	bool IsCharDataNullPtr() const;

private:
	CharData** m_charData;
	CharPaletteHandle m_charPalHandle;
};
