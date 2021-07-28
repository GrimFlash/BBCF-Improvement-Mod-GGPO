#pragma once

#include <imgui.h>
#include <string>

namespace ImGui
{
	IMGUI_API void HoverTooltip(const char* text);
	IMGUI_API bool ButtonUrl(const std::string& label, const wchar_t* url, const ImVec2& btnSize = ImVec2(0, 0));
	IMGUI_API void TextAlignedHorizontalCenter(const char* text, ...);
	IMGUI_API void TextColoredAlignedHorizontalCenter(const ImVec4 color, const char* text);
	IMGUI_API void AlignItemHorizontalCenter(float itemWidth);
	IMGUI_API void Spacing(ImVec2& size);
	IMGUI_API void VerticalSpacing(float height = 1);
	IMGUI_API void HorizontalSpacing(float width = 1);

	IMGUI_API bool SliderByte(const char* label, unsigned char* v, unsigned char v_min, unsigned char v_max, const char* display_format = "%.0f");
	IMGUI_API bool DragByte(const char* label, unsigned char* v, float v_speed, unsigned char v_min, unsigned char v_max, const char* display_format = "%.0f");

	IMGUI_API bool ColorButtonIndexed(const char* desc_id, int idx, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));
	IMGUI_API void ColorTooltipIndexed(const char* text, int idx, const float* col, ImGuiColorEditFlags flags);
	IMGUI_API bool ColorEdit4On32Bit(const char* label, int idx, unsigned char val[4], ImGuiColorEditFlags flags = 0);
	IMGUI_API bool ColorButtonOn32Bit(const char* desc_id, int idx, unsigned char val[4], ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));
	IMGUI_API bool ColorPicker4On32Bit(const char* label, unsigned char col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
}
