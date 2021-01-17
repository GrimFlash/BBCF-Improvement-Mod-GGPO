#include "HitboxOverlay.h"

#include "Core/interfaces.h"
#include "Game/gamestates.h"
#include "Game/Jonb/JonbReader.h"
#include "imgui_internal.h"

void HitboxOverlay::Update()
{
	if (HasNullptrInData() || !m_windowOpen)
	{
		return;
	}

	if (!isHitboxOverlayEnabledInCurrentState())
	{
		return;
	}

	BeforeDraw();

	ImGui::Begin("##HitboxOverlay", nullptr, m_overlayWindowFlags);

	Draw();

	ImGui::End();

	AfterDraw();
}

void HitboxOverlay::BeforeDraw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
}

void HitboxOverlay::Draw()
{
	for (int i = 0; i < g_gameVals.entityCount; i++)
	{
		CharData* pEntity = (CharData*)g_gameVals.pEntityList[i];
		const bool isCharacter = i < 2;
		const bool isEntityActive = pEntity->unknownStatus1 == 1 && pEntity->pJonbEntryBegin;

		if (isCharacter || isEntityActive)
		{
			if (!IsOwnerEnabled(pEntity->ownerEntity))
			{
				continue;
			}

			const ImVec2 entityWorldPos = CalculateObjWorldPosition(pEntity);
			DrawCollisionAreas(pEntity, entityWorldPos);
		}
	}
}

void HitboxOverlay::AfterDraw()
{
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

bool HitboxOverlay::IsOwnerEnabled(CharData* ownerCharInfo)
{
	for (int i = 0; i < 2; i++)
	{
		if (ownerCharInfo == (CharData*)g_gameVals.pEntityList[i])
		{
			return drawCharacterHitbox[i];
		}
	}

	return false;
}

bool HitboxOverlay::HasNullptrInData()
{
	return !g_gameVals.pEntityList;
}

ImVec2 HitboxOverlay::CalculateObjWorldPosition(const CharData* charObj)
{
	float posX = charObj->position_x_dupe - charObj->offsetX_1 + charObj->offsetX_2;
	float posY = charObj->position_y_dupe + charObj->offsetY_2;

	return ImVec2(
		floor(posX / 1000 * m_scale),
		floor(posY / 1000 * m_scale)
	);
}

ImVec2 HitboxOverlay::CalculateScreenPosition(ImVec2 worldPos)
{
	D3DXVECTOR3 result;
	D3DXVECTOR3 vec3WorldPos(worldPos.x, worldPos.y, 0.0f);
	WorldToScreen(g_interfaces.pD3D9ExWrapper, g_gameVals.viewMatrix, g_gameVals.projMatrix, &vec3WorldPos, &result);

	return ImVec2(floor(result.x), floor(result.y));
}

ImVec2 HitboxOverlay::RotatePoint(ImVec2 center, float angleInRad, ImVec2 point)
{
	if (!angleInRad)
	{
		return point;
	}

	// translate point back to origin:
	point.x -= center.x;
	point.y -= center.y;

	float s = sin(angleInRad);
	float c = cos(angleInRad);

	// rotate point
	float xNew = point.x * c - point.y * s;
	float yNew = point.x * s + point.y * c;

	// translate point back:
	point.x = xNew + center.x;
	point.y = yNew + center.y;
	return point;
}

void HitboxOverlay::DrawOriginLine(ImVec2 worldPos, float rotationRad)
{
	const unsigned int colorOrange = 0xFFFF9900;
	const int horizontalLength = 20;
	const int verticalLength = 50;

	ImVec2 horizontalFrom = RotatePoint(worldPos, rotationRad, ImVec2(worldPos.x - horizontalLength / 2, worldPos.y));
	ImVec2 horizontalTo = RotatePoint(worldPos, rotationRad, ImVec2(worldPos.x + horizontalLength / 2, worldPos.y));
	horizontalFrom = CalculateScreenPosition(horizontalFrom);
	horizontalTo = CalculateScreenPosition(horizontalTo);
	RenderLine(horizontalFrom, horizontalTo, colorOrange, 3);

	ImVec2 verticalFrom = worldPos;
	ImVec2 verticalTo = RotatePoint(verticalFrom, rotationRad, ImVec2(verticalFrom.x, verticalFrom.y + verticalLength));
	verticalFrom = CalculateScreenPosition(verticalFrom);
	verticalTo = CalculateScreenPosition(verticalTo);
	RenderLine(verticalFrom, verticalTo, colorOrange, 3);
}

void HitboxOverlay::DrawCollisionAreas(const CharData* charObj, const ImVec2 playerWorldPos)
{
	std::vector<JonbEntry> entries = JonbReader::getJonbEntries(charObj);

	for (const JonbEntry &entry : entries)
	{
		float scaleX = charObj->scaleX / 1000.0f;
		float scaleY = charObj->scaleY / 1000.0f;
		float offsetX =  floor(entry.offsetX * m_scale * scaleX);
		float offsetY = -floor(entry.offsetY * m_scale * scaleY);
		float width =    floor(entry.width * m_scale * scaleX);
		float height =  -floor(entry.height * m_scale * scaleY);

		float rotationDeg = charObj->rotationDegrees / 1000.0f;

		if (!charObj->facingLeft)
		{
			offsetX = -offsetX;
			width = -width;
			if (rotationDeg)
			{
				rotationDeg = 360.0f - rotationDeg;
			}
		}

		ImVec2 pointA(playerWorldPos.x + offsetX, playerWorldPos.y + offsetY);
		ImVec2 pointB(playerWorldPos.x + offsetX + width, playerWorldPos.y + offsetY);
		ImVec2 pointC(playerWorldPos.x + offsetX + width, playerWorldPos.y + offsetY + height);
		ImVec2 pointD(playerWorldPos.x + offsetX, playerWorldPos.y + offsetY + height);

		float rotationRad = D3DXToRadian(rotationDeg);

		pointA = RotatePoint(playerWorldPos, rotationRad, pointA);
		pointB = RotatePoint(playerWorldPos, rotationRad, pointB);
		pointC = RotatePoint(playerWorldPos, rotationRad, pointC);
		pointD = RotatePoint(playerWorldPos, rotationRad, pointD);
		
		pointA = CalculateScreenPosition(pointA);
		pointB = CalculateScreenPosition(pointB);
		pointC = CalculateScreenPosition(pointC);
		pointD = CalculateScreenPosition(pointD);

		const unsigned int colorBlue = 0xFF0033CC;
		const unsigned int colorRed = 0xFFFF0000;
		const unsigned int rectBorderColor = entry.type == JonbChunkType_Hurtbox ? colorBlue : colorRed;

		RenderRect(pointA, pointB, pointC, pointD, rectBorderColor, m_rectThickness);

		const unsigned char transparency = 0xFF * m_rectFillTransparency;
		unsigned int clearedTransparencyBits = (rectBorderColor & ~0xFF000000);
		unsigned int transparencyPercentage = ((int)transparency << 24) & 0xFF000000;
		const unsigned int rectFillColor = clearedTransparencyBits | transparencyPercentage;
		RenderRectFilled(pointA, pointB, pointC, pointD, rectFillColor);

		if (drawOriginLine)
		{
			DrawOriginLine(playerWorldPos, rotationRad);
		}
	}
}

float& HitboxOverlay::GetScale()
{
	return m_scale;
}

void HitboxOverlay::DrawRectThicknessSlider()
{
	ImGui::SliderFloat("Border thickness", &m_rectThickness, 0.0f, 5.0f, "%.1f");
}

void HitboxOverlay::DrawRectFillTransparencySlider()
{
	ImGui::SliderFloat("Fill transparency", &m_rectFillTransparency, 0.0f, 1.0f, "%.2f");
}

void HitboxOverlay::RenderLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	window->DrawList->AddLine(from, to, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), thickness);
}

void HitboxOverlay::RenderCircle(const ImVec2& position, float radius, uint32_t color, float thickness, uint32_t segments)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	window->DrawList->AddCircle(position, radius, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), segments, thickness);
}

void HitboxOverlay::RenderCircleFilled(const ImVec2& position, float radius, uint32_t color, uint32_t segments)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	window->DrawList->AddCircleFilled(position, radius, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), segments);
}

void HitboxOverlay::RenderRect(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xFF;
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = (color) & 0xFF;

	window->DrawList->AddRect(from, to, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), rounding, roundingCornersFlags, thickness);
}

void HitboxOverlay::RenderRect(const ImVec2& pointA, const ImVec2& pointB, const ImVec2& pointC, const ImVec2& pointD, uint32_t color, float thickness)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xFF;
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = (color) & 0xFF;

	window->DrawList->AddQuad(pointA, pointB, pointC, pointD, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), thickness);
}

void HitboxOverlay::RenderRectFilled(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xFF;
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = (color) & 0xFF;

	window->DrawList->AddRectFilled(from, to, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), rounding, roundingCornersFlags);
}

void HitboxOverlay::RenderRectFilled(const ImVec2& pointA, const ImVec2& pointB, const ImVec2& pointC, const ImVec2& pointD, uint32_t color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = (color >> 24) & 0xFF;
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = (color) & 0xFF;

	window->DrawList->AddQuadFilled(pointA, pointB, pointC, pointD, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }));
}

bool HitboxOverlay::WorldToScreen(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* view, D3DXMATRIX* proj, D3DXVECTOR3* pos, D3DXVECTOR3* out)
{
	D3DVIEWPORT9 viewPort;
	D3DXMATRIX world;

	pDevice->GetViewport(&viewPort);
	D3DXMatrixIdentity(&world);

	D3DXVec3Project(out, pos, &viewPort, proj, view, &world);
	if (out->z < 1) {
		return true;
	}
	return false;
}
