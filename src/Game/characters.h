#pragma once
#include <string>

enum CharIndex
{
	CharIndex_Ragna = 0,
	CharIndex_Jin,
	CharIndex_Noel,
	CharIndex_Rachel,
	CharIndex_Taokaka,
	CharIndex_Tager,
	CharIndex_Litchi,
	CharIndex_Arakune,
	CharIndex_Bang8,
	CharIndex_Carl,
	CharIndex_Hakumen,
	CharIndex_Nu,
	CharIndex_Tsubaki,
	CharIndex_Hazama,
	CharIndex_Mu,
	CharIndex_Makoto,
	CharIndex_Valkenhayn,
	CharIndex_Platinum,
	CharIndex_Relius,
	CharIndex_Izayoi,
	CharIndex_Amane,
	CharIndex_Bullet,
	CharIndex_Azrael,
	CharIndex_Kagura,
	CharIndex_Kokonoe,
	CharIndex_Terumi,
	CharIndex_Celica,
	CharIndex_Lambda,
	CharIndex_Hibiki,
	CharIndex_Nine,
	CharIndex_Naoto,
	CharIndex_Izanami,
	CharIndex_Susanoo,
	CharIndex_Es,
	CharIndex_Mai,
	CharIndex_Jubei
};

int getCharactersCount();

const std::string& getCharacterNameByIndexA(int charIndex);

std::wstring getCharacterNameByIndexW(int charIndex);

bool isCharacterIndexOutOfBound(int charIndex);
