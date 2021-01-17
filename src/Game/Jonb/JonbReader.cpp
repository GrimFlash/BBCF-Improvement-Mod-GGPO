#include "JonbReader.h"

std::vector<JonbEntry> JonbReader::getJonbEntries(const CharData * charObj)
{
	std::vector<JonbEntry> jonbEntries;

	const int entriesCount = charObj->hurtboxCount + charObj->hitboxCount;
	JonbEntry* pEntry = charObj->pJonbEntryBegin;

	for (int i = 0; i < entriesCount; i++)
	{
		jonbEntries.push_back(*pEntry);
		pEntry++;
	}

	return jonbEntries;
}
