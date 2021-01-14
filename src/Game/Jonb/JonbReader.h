#pragma once
#include "../CharData.h"
#include "JonbEntry.h"

#include <vector>

class JonbReader
{
public:
	static std::vector<JonbEntry> getJonbEntries(const CharData* charObj);
};
