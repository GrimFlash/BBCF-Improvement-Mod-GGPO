#include "Player.h"

void Player::SetCharDataPtr(const void* addr)
{
	m_charData = (CharData**)addr;
}

bool Player::IsCharDataNullPtr() const
{
	if (m_charData == 0)
		return true;

	return *m_charData == 0;
}

CharData* Player::GetData() const
{
	return *m_charData;
}

CharPaletteHandle& Player::GetPalHandle()
{
	return m_charPalHandle;
}
