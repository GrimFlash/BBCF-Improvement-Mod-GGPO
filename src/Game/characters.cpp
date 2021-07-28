#include "characters.h"

#include <vector>

const std::vector<std::string> charNames
{
	"Ragna",		//00
	"Jin",			//01
	"Noel",			//02
	"Rachel",		//03
	"Taokaka",		//04
	"Tager",		//05
	"Litchi",		//06
	"Arakune",		//07
	"Bang",			//08
	"Carl",			//09
	"Hakumen",		//10
	"Nu",			//11
	"Tsubaki",		//12
	"Hazama",		//13
	"Mu",			//14
	"Makoto",		//15
	"Valkenhayn",	//16
	"Platinum",		//17
	"Relius",		//18
	"Izayoi",		//19
	"Amane",		//20
	"Bullet",		//21
	"Azrael",		//22
	"Kagura",		//23
	"Kokonoe",		//24
	"Terumi",		//25
	"Celica",		//26
	"Lambda",		//27
	"Hibiki",		//28
	"Nine",			//29
	"Naoto",		//30
	"Izanami",		//31
	"Susanoo",		//32
	"Es",			//33
	"Mai",			//34
	"Jubei"			//35
};

int getCharactersCount()
{
	return charNames.size();
}

const std::string& getCharacterNameByIndexA(int charIndex)
{
	const static std::string UNKNOWN = "Unknown";

	if (charIndex < getCharactersCount())
		return charNames[charIndex];

	return UNKNOWN;
}

std::wstring getCharacterNameByIndexW(int charIndex)
{
	const std::string& charName = getCharacterNameByIndexA(charIndex);
	return std::wstring(charName.begin(), charName.end());
}

bool isCharacterIndexOutOfBound(int charIndex)
{
	return getCharactersCount() < charIndex;
}
