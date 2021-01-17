#pragma once

enum JonbEntryType_
{
	JonbChunkType_Hurtbox,
	JonbChunkType_Hitbox
};

class JonbEntry
{
public:
	JonbEntryType_ type;
	float offsetX;
	float offsetY;
	float width;
	float height;
};