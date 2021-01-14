#include "imgui_utils.h"

#include "imgui_internal.h"

#include <windows.h>

///////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////

// Helpers copied from imgui_internal
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

// Helpers copied from imgui.cpp
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose 
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255

static void ColorPickerOptionsPopup(ImGuiColorEditFlags flags, const float* ref_col)
{
	bool allow_opt_picker = !(flags & ImGuiColorEditFlags__PickerMask);
	bool allow_opt_alpha_bar = !(flags & ImGuiColorEditFlags_NoAlpha) && !(flags & ImGuiColorEditFlags_AlphaBar);
	if ((!allow_opt_picker && !allow_opt_alpha_bar) || !ImGui::BeginPopup("context"))
		return;
	ImGuiContext& g = *GImGui;
	if (allow_opt_picker)
	{
		ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (ImGui::GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
		ImGui::PushItemWidth(picker_size.x);
		for (int picker_type = 0; picker_type < 2; picker_type++)
		{
			// Draw small/thumbnail version of each picker type (over an invisible button for selection)
			if (picker_type > 0) ImGui::Separator();
			ImGui::PushID(picker_type);
			ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | (flags & ImGuiColorEditFlags_NoAlpha);
			if (picker_type == 0) picker_flags |= ImGuiColorEditFlags_PickerHueBar;
			if (picker_type == 1) picker_flags |= ImGuiColorEditFlags_PickerHueWheel;
			ImVec2 backup_pos = ImGui::GetCursorScreenPos();
			if (ImGui::Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
				g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags__PickerMask) | (picker_flags & ImGuiColorEditFlags__PickerMask);
			ImGui::SetCursorScreenPos(backup_pos);
			ImVec4 dummy_ref_col;
			memcpy(&dummy_ref_col.x, ref_col, sizeof(float) * (picker_flags & ImGuiColorEditFlags_NoAlpha ? 3 : 4));
			ImGui::ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
			ImGui::PopID();
		}
		ImGui::PopItemWidth();
	}
	if (allow_opt_alpha_bar)
	{
		if (allow_opt_picker) ImGui::Separator();
		ImGui::CheckboxFlags("Alpha Bar", (unsigned int*)&g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar);
	}
	ImGui::EndPopup();
}

// 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
static void RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, ImGuiDir direction, ImU32 col)
{
	switch (direction)
	{
	case ImGuiDir_Left:  draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
	case ImGuiDir_Right: draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
	case ImGuiDir_Up:    draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
	case ImGuiDir_Down:  draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
	case ImGuiDir_None: case ImGuiDir_Count_: break; // Fix warnings
	}
}

static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w)
{
	RenderArrow(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32_BLACK);
	RenderArrow(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32_WHITE);
	RenderArrow(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32_BLACK);
	RenderArrow(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32_WHITE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui::HoverTooltip(const char* text)
{
	if (IsItemHovered())
	{
		BeginTooltip();
		TextUnformatted(text);
		EndTooltip();
	}
}

bool ImGui::ButtonUrl(const std::string& label, const wchar_t* url, const ImVec2& btnSize)
{
	if (Button(label.c_str(), btnSize))
	{
		ShellExecute(nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
		return true;
	}

	return false;
}

void ImGui::TextAlignedHorizontalCenter(const char* text, ...)
{
	const ImVec4 COLOR_TRANSPARENT(0.000f, 0.000f, 0.000f, 0.000f);

	va_list args;
	va_start(args, text);

	TextColoredV(COLOR_TRANSPARENT, text, args);
	float width = GetItemRectSize().x;
	float height = GetItemRectSize().y;
	AlignItemHorizontalCenter(width);
	SetCursorPosY(GetCursorPosY() - height);

	TextV(text, args);
	va_end(args);
}

void ImGui::TextColoredAlignedHorizontalCenter(const ImVec4 color, const char* text)
{
	const ImVec4 COLOR_TRANSPARENT(0.000f, 0.000f, 0.000f, 0.000f);
	TextColored(COLOR_TRANSPARENT, text);

	float width = GetItemRectSize().x;
	float height = GetItemRectSize().y;
	AlignItemHorizontalCenter(width);
	SetCursorPosY(GetCursorPosY() - height);

	TextColored(color, text);
}

void ImGui::AlignItemHorizontalCenter(float itemWidth)
{
	SetCursorPosX(GetWindowSize().x / 2 - (itemWidth / 2));
}

void ImGui::Spacing(ImVec2& size)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ItemSize(size);
	PopStyleVar();
}

void ImGui::VerticalSpacing(float height)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ItemSize(ImVec2(0, height));
	PopStyleVar();
}

void ImGui::HorizontalSpacing(float width)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ItemSize(ImVec2(width, 0));
	PopStyleVar();

	SameLine();
}

bool ImGui::SliderByte(const char* label, unsigned char* v, unsigned char v_min, unsigned char v_max, const char* display_format)
{
    if (!display_format)
        display_format = "%.0f";
    float v_f = (float)*v;
    bool value_changed = SliderFloat(label, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
    *v = (unsigned char)v_f;
    return value_changed;
}

bool ImGui::DragByte(const char* label, unsigned char* v, float v_speed, unsigned char v_min, unsigned char v_max, const char* display_format)
{
    if (!display_format)
        display_format = "%.0f";
    float v_f = (float)*v;
    bool value_changed = DragFloat(label, &v_f, v_speed, (float)v_min, (float)v_max, display_format);
    *v = (unsigned char)v_f;
    return value_changed;
}

void ImGui::ColorTooltipIndexed(const char* text, int idx, const float* col, ImGuiColorEditFlags flags)
{
    ImGuiContext& g = *GImGui;

    int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & ImGuiColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
    BeginTooltipEx(0, true);

    const char* text_end = text ? FindRenderedTextEnd(text, NULL) : text;
    if (text_end > text)
    {
        TextUnformatted(text, text_end);
        Separator();
    }

    ImVec2 sz(g.FontSize * 4 + g.Style.FramePadding.y * 2, g.FontSize * 4 + g.Style.FramePadding.y * 2);
    ColorButton("##preview", ImVec4(col[0], col[1], col[2], col[3]), (flags & (ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf)) | ImGuiColorEditFlags_NoTooltip, sz);
    SameLine();
	if (flags & ImGuiColorEditFlags_NoAlpha)
		idx ? Text("%.3d\n#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", idx, cr, cg, cb, cr, cg, cb, col[0], col[1], col[2])
		: Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
	else
		idx ? Text("%.3d\n#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", idx, cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3])
		: Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
    EndTooltip();
}

bool ImGui::ColorButtonIndexed(const char* desc_id, int idx, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    float default_size = GetFrameHeight();
    if (size.x == 0.0f)
        size.x = default_size;
    if (size.y == 0.0f)
        size.y = default_size;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_without_alpha(col.x, col.y, col.z, 1.0f);
    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
    ImRect bb_inner = bb;
    float off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
    bb_inner.Expand(off);
    if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col.w < 1.0f)
    {
        float mid_x = (float)(int)((bb_inner.Min.x + bb_inner.Max.x) * 0.5f + 0.5f);
        RenderColorRectWithAlphaCheckerboard(ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight);
        window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_without_alpha), rounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
    }
    else
    {
        // Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
        ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col : col_without_alpha;
        if (col_source.w < 1.0f)
            RenderColorRectWithAlphaCheckerboard(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
        else
            window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding, ImDrawCornerFlags_All);
    }
    if (g.Style.FrameBorderSize > 0.0f)
        RenderFrameBorder(bb.Min, bb.Max, rounding);
    else
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border

    // Drag and Drop Source
    if (g.ActiveId == id && BeginDragDropSource()) // NB: The ActiveId test is merely an optional micro-optimization
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col, sizeof(float) * 3, ImGuiCond_Once);
        else
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col, sizeof(float) * 4, ImGuiCond_Once);
        ColorButton(desc_id, col, flags);
        SameLine();
        TextUnformatted("Color");
        EndDragDropSource();
        hovered = false;
    }

    // Tooltip
    if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
        ColorTooltipIndexed(desc_id, idx, &col.x, flags & (ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

    return pressed;
}

bool ImGui::ColorButtonOn32Bit(const char* desc_id, int idx, unsigned char val[4], ImGuiColorEditFlags flags, ImVec2 size)
{
	ImVec4 col = { val[2] == 0 ? 0 : val[2] / 255.0f,
		val[1] == 0 ? 0 : val[1] / 255.0f,
		val[0] == 0 ? 0 : val[0] / 255.0f,
		val[3] == 0 ? 0 : val[3] / 255.0f };

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(desc_id);
	float default_size = GetFrameHeight();
	if (size.x == 0.0f)
		size.x = default_size;
	if (size.y == 0.0f)
		size.y = default_size;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (flags & ImGuiColorEditFlags_NoAlpha)
		flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImVec4 col_without_alpha(col.x, col.y, col.z, 1.0f);
	float grid_step = ImMin(size.x, size.y) / 2.99f;
	float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
	ImRect bb_inner = bb;
	float off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
	bb_inner.Expand(off);
	if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col.w < 1.0f)
	{
		float mid_x = (float)(int)((bb_inner.Min.x + bb_inner.Max.x) * 0.5f + 0.5f);
		RenderColorRectWithAlphaCheckerboard(ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight);
		window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_without_alpha), rounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
	}
	else
	{
		// Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
		ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col : col_without_alpha;
		if (col_source.w < 1.0f)
			RenderColorRectWithAlphaCheckerboard(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
		else
			window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding, ImDrawCornerFlags_All);
	}
	if (g.Style.FrameBorderSize > 0.0f)
		RenderFrameBorder(bb.Min, bb.Max, rounding);
	else
		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border

																							// Drag and Drop Source
	if (g.ActiveId == id && BeginDragDropSource()) // NB: The ActiveId test is merely an optional micro-optimization
	{
		if (flags & ImGuiColorEditFlags_NoAlpha)
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col, sizeof(float) * 3, ImGuiCond_Once);
		else
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col, sizeof(float) * 4, ImGuiCond_Once);
		ColorButton(desc_id, col, flags);
		SameLine();
		TextUnformatted("Color");
		EndDragDropSource();
		hovered = false;
	}

	// Tooltip
	if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
		ColorTooltipIndexed(desc_id, idx, &col.x, flags & (ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

	return pressed;
}

bool ImGui::ColorEdit4On32Bit(const char* label, int idx, unsigned char val[4], ImGuiColorEditFlags flags)
{
	float col[] = { val[2] == 0 ? 0 : val[2] / 255.0f,
					val[1] == 0 ? 0 : val[1] / 255.0f,
					val[0] == 0 ? 0 : val[0] / 255.0f,
					val[3] == 0 ? 0 : val[3] / 255.0f };
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float square_sz = GetFrameHeight();
	const float w_extra = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
	const float w_items_all = CalcItemWidth() - w_extra;
	const char* label_display_end = FindRenderedTextEnd(label);

	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
	const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
	const int components = alpha ? 4 : 3;
	const ImGuiColorEditFlags flags_untouched = flags;

	BeginGroup();
	PushID(label);

	// If we're not showing any slider there's no point in doing any HSV conversions
	if (flags & ImGuiColorEditFlags_NoInputs)
		flags = (flags & (~ImGuiColorEditFlags__InputsMask)) | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_NoOptions;

	// Context menu: display and modify options (before defaults are applied)
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorEditOptionsPopup(col, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags__InputsMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__InputsMask);
	if (!(flags & ImGuiColorEditFlags__DataTypeMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);
	if (!(flags & ImGuiColorEditFlags__PickerMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);
	flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__InputsMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask));

	// Convert to the formats we need
	float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
	if (flags & ImGuiColorEditFlags_HSV)
		ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
	unsigned char i[4] = {
		(unsigned char)IM_F32_TO_INT8_UNBOUND(f[0]),
		(unsigned char)IM_F32_TO_INT8_UNBOUND(f[1]),
		(unsigned char)IM_F32_TO_INT8_UNBOUND(f[2]),
		(unsigned char)IM_F32_TO_INT8_UNBOUND(f[3]) };

	bool value_changed = false;
	bool value_changed_as_float = false;

	if ((flags & (ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB/HSV 0..255 Sliders
		const float w_item_one = ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
		const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

		const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
		const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		const char* fmt_table_int[3][4] =
		{
			{ "%3.0f",   "%3.0f",   "%3.0f",   "%3.0f" }, // Short display
			{ "R:%3.0f", "G:%3.0f", "B:%3.0f", "A:%3.0f" }, // Long display for RGBA
			{ "H:%3.0f", "S:%3.0f", "V:%3.0f", "A:%3.0f" }  // Long display for HSVA
		};
		const char* fmt_table_float[3][4] =
		{
			{ "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
			{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
			{ "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
		};
		const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_HSV) ? 2 : 1;

		PushItemWidth(w_item_one);
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			if (n + 1 == components)
				PushItemWidth(w_item_last);
			if (flags & ImGuiColorEditFlags_Float)
				value_changed = value_changed_as_float = value_changed | DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
			else
				value_changed |= DragByte(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context");
		}
		PopItemWidth();
		PopItemWidth();
	}
	else if ((flags & ImGuiColorEditFlags_HEX) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB Hexadecimal Input
		char buf[64];
		if (alpha)
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
		PushItemWidth(w_items_all);
		if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
		{
			value_changed = true;
			char* p = buf;
			while (*p == '#' || ImCharIsSpace(*p))
				p++;
			i[0] = i[1] = i[2] = i[3] = 0;
			if (alpha)
				sscanf(p, "%02X%02X%02X%02X", (unsigned char*)&i[0], (unsigned char*)&i[1], (unsigned char*)&i[2], (unsigned char*)&i[3]); // Treat at unsigned (%X is unsigned)
			else
				sscanf(p, "%02X%02X%02X", (unsigned char*)&i[0], (unsigned char*)&i[1], (unsigned char*)&i[2]);
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");
		PopItemWidth();
	}

	ImGuiWindow* picker_active_window = NULL;
	if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
	{
		if (!(flags & ImGuiColorEditFlags_NoInputs))
			SameLine(0, style.ItemInnerSpacing.x);

		const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
		if (ColorButtonIndexed("##ColorButton", idx, col_v4, flags))
		{
			if (!(flags & ImGuiColorEditFlags_NoPicker))
			{
				// Store current color and open a picker
				g.ColorPickerRef = col_v4;
				OpenPopup("picker");
				SetNextWindowPos(window->DC.LastItemRect.GetBL() + ImVec2(-1, style.ItemSpacing.y));
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");

		if (BeginPopup("picker"))
		{
			picker_active_window = g.CurrentWindow;
			if (label != label_display_end)
			{
				TextUnformatted(label, label_display_end);
				Separator();
			}
			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__InputsMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
			value_changed |= ColorPicker4On32Bit("##picker", val, picker_flags, &g.ColorPickerRef.x);
			PopItemWidth();
			EndPopup();
		}
	}

	if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
	{
		SameLine(0, style.ItemInnerSpacing.x);
		TextUnformatted(label, label_display_end);
	}

	// Convert back
	if (picker_active_window == NULL)
	{
		if (!value_changed_as_float)
			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		if (flags & ImGuiColorEditFlags_HSV)
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		if (value_changed)
		{
			unsigned char temp[] = {
				(unsigned char)IM_F32_TO_INT8_UNBOUND(f[2]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(f[1]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(f[0]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(f[3]) };
			val[0] = temp[0];
			val[1] = temp[1];
			val[2] = temp[2];
			if (alpha)
				val[3] = temp[3];
		}
	}

	PopID();
	EndGroup();

	// Drag and Drop Target
	if (window->DC.LastItemRectHoveredRect && BeginDragDropTarget()) // NB: The LastItemRectHoveredRect test is merely an optional micro-optimization
	{
		float plData[4];
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
		{
			memcpy((float*)plData, payload->Data, sizeof(float) * 3);
			unsigned char temp[] = {
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[2]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[1]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[0]) };
			val[0] = temp[0];
			val[1] = temp[1];
			val[2] = temp[2];
			value_changed = true;
		}
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			memcpy((float*)plData, payload->Data, sizeof(float) * components);
			unsigned char temp[] = {
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[2]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[1]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[0]),
				(unsigned char)IM_F32_TO_INT8_UNBOUND(plData[3]) };
			val[0] = temp[0];
			val[1] = temp[1];
			val[2] = temp[2];
			val[3] = temp[3];
			value_changed = true;
		}
		EndDragDropTarget();
	}

	// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
	if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
		window->DC.LastItemId = g.ActiveId;

	return value_changed;
}

bool ImGui::ColorPicker4On32Bit(const char* label, unsigned char val[4], ImGuiColorEditFlags flags, const float* ref_col)
{
	float col[] = {
		val[2] == 0 ? 0 : val[2] / 255.0f,
		val[1] == 0 ? 0 : val[1] / 255.0f,
		val[0] == 0 ? 0 : val[0] / 255.0f,
		val[3] == 0 ? 0 : val[3] / 255.0f };

	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	ImDrawList* draw_list = window->DrawList;

	ImGuiStyle& style = g.Style;
	ImGuiIO& io = g.IO;

	PushID(label);
	BeginGroup();

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
		flags |= ImGuiColorEditFlags_NoSmallPreview;

	// Context menu: display and store options.
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorPickerOptionsPopup(flags, col);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags__PickerMask))
		flags |= ((g.ColorEditOptions & ImGuiColorEditFlags__PickerMask) ? g.ColorEditOptions : ImGuiColorEditFlags__OptionsDefault) & ImGuiColorEditFlags__PickerMask;
	IM_ASSERT(ImIsPowerOfTwo((int)(flags & ImGuiColorEditFlags__PickerMask))); // Check that only 1 is selected
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

	// Setup
	int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
	bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
	ImVec2 picker_pos = window->DC.CursorPos;
	float square_sz = GetFrameHeight();
	float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
	float sv_picker_size = ImMax(bars_width * 1, CalcItemWidth() - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
	float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
	float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
	float bars_triangles_half_sz = (float)(int)(bars_width * 0.20f);

	float backup_initial_col[4];
	memcpy(backup_initial_col, col, components * sizeof(float));

	float wheel_thickness = sv_picker_size * 0.08f;
	float wheel_r_outer = sv_picker_size * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

	// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
	float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
	ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
	ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
	ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

	float H, S, V;
	ColorConvertRGBtoHSV(col[0], col[1], col[2], H, S, V);

	bool value_changed = false, value_changed_h = false, value_changed_sv = false;

	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Hue wheel + SV triangle logic
		InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
		if (IsItemActive())
		{
			ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
			ImVec2 current_off = g.IO.MousePos - wheel_center;
			float initial_dist2 = ImLengthSqr(initial_off);
			if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
			{
				// Interactive with Hue wheel
				H = atan2f(current_off.y, current_off.x) / IM_PI * 0.5f;
				if (H < 0.0f)
					H += 1.0f;
				value_changed = value_changed_h = true;
			}
			float cos_hue_angle = cosf(-H * 2.0f * IM_PI);
			float sin_hue_angle = sinf(-H * 2.0f * IM_PI);
			if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
			{
				// Interacting with SV triangle
				ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
				if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
					current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
				float uu, vv, ww;
				ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
				V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
				S = ImClamp(uu / V, 0.0001f, 1.0f);
				value_changed = value_changed_sv = true;
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{
		// SV rectangle logic
		InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
		if (IsItemActive())
		{
			S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
			V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			value_changed = value_changed_sv = true;
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");

		// Hue bar logic
		SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
		InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
		if (IsItemActive())
		{
			H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			value_changed = value_changed_h = true;
		}
	}

	// Alpha bar logic
	if (alpha_bar)
	{
		SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
		InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
		if (IsItemActive())
		{
			col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			val[3] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[3]);
			value_changed = true;
		}
	}

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
	{
		SameLine(0, style.ItemInnerSpacing.x);
		BeginGroup();
	}

	if (!(flags & ImGuiColorEditFlags_NoLabel))
	{
		const char* label_display_end = FindRenderedTextEnd(label);
		if (label != label_display_end)
		{
			if ((flags & ImGuiColorEditFlags_NoSidePreview))
				SameLine(0, style.ItemInnerSpacing.x);
			TextUnformatted(label, label_display_end);
		}
	}

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
	{
		ImVec4 col_v4(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
		if ((flags & ImGuiColorEditFlags_NoLabel))
			Text("Current");
		ColorButton("##current", col_v4, (flags & (ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip)), ImVec2(square_sz * 3, square_sz * 2));
		if (ref_col != NULL)
		{
			Text("Original");
			ImVec4 ref_col_v4(ref_col[0], ref_col[1], ref_col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : ref_col[3]);
			if (ColorButton("##original", ref_col_v4, (flags & (ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip)), ImVec2(square_sz * 3, square_sz * 2)))
			{
				memcpy(col, ref_col, components * sizeof(float));
				val[0] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[2]);
				val[1] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[1]);
				val[2] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[0]);
				val[3] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[3]);
				value_changed = true;
			}
		}
		EndGroup();
	}

	// Convert back color to RGB
	if (value_changed_h || value_changed_sv)
	{
		ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f, col[0], col[1], col[2]);
		val[0] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[2]);
		val[1] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[1]);
		val[2] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[0]);
	}

	// R,G,B and H,S,V slider color editor
	if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
		ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
		ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
		if (flags & ImGuiColorEditFlags_RGB || (flags & ImGuiColorEditFlags__InputsMask) == 0)
			value_changed |= ColorEdit4On32Bit("##rgb", NULL, val, sub_flags | ImGuiColorEditFlags_RGB);
		if (flags & ImGuiColorEditFlags_HSV || (flags & ImGuiColorEditFlags__InputsMask) == 0)
			value_changed |= ColorEdit4On32Bit("##hsv", NULL, val, sub_flags | ImGuiColorEditFlags_HSV);
		if (flags & ImGuiColorEditFlags_HEX || (flags & ImGuiColorEditFlags__InputsMask) == 0)
			value_changed |= ColorEdit4On32Bit("##hex", NULL, val, sub_flags | ImGuiColorEditFlags_HEX);
		PopItemWidth();
	}

	// Try to cancel hue wrap (after ColorEdit), if any
	if (value_changed)
	{
		float new_H, new_S, new_V;
		ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0)
		{
			if (new_V <= 0 && V != new_V)
			{
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
				val[0] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[2]);
				val[1] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[1]);
				val[2] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[0]);
			}
			else if (new_S <= 0)
			{
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
				val[0] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[2]);
				val[1] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[1]);
				val[2] = (unsigned char)IM_F32_TO_INT8_UNBOUND(col[0]);
			}
		}
	}

	ImVec4 hue_color_f(1, 1, 1, 1); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
	ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
	ImU32 col32_no_alpha = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 1.0f));

	const ImU32 hue_colors[6 + 1] = { IM_COL32(255,0,0,255), IM_COL32(255,255,0,255), IM_COL32(0,255,0,255), IM_COL32(0,255,255,255), IM_COL32(0,0,255,255), IM_COL32(255,0,255,255), IM_COL32(255,0,0,255) };
	ImVec2 sv_cursor_pos;

	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Render Hue Wheel
		const float aeps = 1.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
		const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
		for (int n = 0; n < 6; n++)
		{
			const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
			const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
			const int vert_start_idx = draw_list->VtxBuffer.Size;
			draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
			draw_list->PathStroke(IM_COL32_WHITE, false, wheel_thickness);
			const int vert_end_idx = draw_list->VtxBuffer.Size;

			// Paint colors over existing vertices
			ImVec2 gradient_p0(wheel_center.x + cosf(a0) * wheel_r_inner, wheel_center.y + sinf(a0) * wheel_r_inner);
			ImVec2 gradient_p1(wheel_center.x + cosf(a1) * wheel_r_inner, wheel_center.y + sinf(a1) * wheel_r_inner);
			ShadeVertsLinearColorGradientKeepAlpha(draw_list->VtxBuffer.Data + vert_start_idx, draw_list->VtxBuffer.Data + vert_end_idx, gradient_p0, gradient_p1, hue_colors[n], hue_colors[n + 1]);
		}

		// Render Cursor + preview on Hue Wheel
		float cos_hue_angle = cosf(H * 2.0f * IM_PI);
		float sin_hue_angle = sinf(H * 2.0f * IM_PI);
		ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
		float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
		int hue_cursor_segments = ImClamp((int)(hue_cursor_rad / 1.4f), 9, 32);
		draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, IM_COL32(128, 128, 128, 255), hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, IM_COL32_WHITE, hue_cursor_segments);

		// Render SV triangle (rotated according to hue)
		ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
		ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
		ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
		ImVec2 uv_white = GetFontTexUvWhitePixel();
		draw_list->PrimReserve(6, 6);
		draw_list->PrimVtx(tra, uv_white, hue_color32);
		draw_list->PrimVtx(trb, uv_white, hue_color32);
		draw_list->PrimVtx(trc, uv_white, IM_COL32_WHITE);
		draw_list->PrimVtx(tra, uv_white, IM_COL32_BLACK_TRANS);
		draw_list->PrimVtx(trb, uv_white, IM_COL32_BLACK);
		draw_list->PrimVtx(trc, uv_white, IM_COL32_BLACK_TRANS);
		draw_list->AddTriangle(tra, trb, trc, IM_COL32(128, 128, 128, 255), 1.5f);
		sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{
		// Render SV Square
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), IM_COL32_WHITE, hue_color32, hue_color32, IM_COL32_WHITE);
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), IM_COL32_BLACK_TRANS, IM_COL32_BLACK_TRANS, IM_COL32_BLACK, IM_COL32_BLACK);
		RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
		sv_cursor_pos.x = ImClamp((float)(int)(picker_pos.x + ImSaturate(S) * sv_picker_size + 0.5f), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
		sv_cursor_pos.y = ImClamp((float)(int)(picker_pos.y + ImSaturate(1 - V) * sv_picker_size + 0.5f), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

		// Render Hue Bar
		for (int i = 0; i < 6; ++i)
			draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), hue_colors[i], hue_colors[i], hue_colors[i + 1], hue_colors[i + 1]);
		float bar0_line_y = (float)(int)(picker_pos.y + H * sv_picker_size + 0.5f);
		RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
		RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f);
	}

	// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
	float sv_cursor_rad = value_changed_sv ? 10.0f : 6.0f;
	draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, col32_no_alpha, 12);
	draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, IM_COL32(128, 128, 128, 255), 12);
	draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, IM_COL32_WHITE, 12);

	// Render alpha bar
	if (alpha_bar)
	{
		float alpha = ImSaturate(col[3]);
		ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
		RenderColorRectWithAlphaCheckerboard(bar1_bb.Min, bar1_bb.Max, IM_COL32(0, 0, 0, 0), bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
		draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, col32_no_alpha, col32_no_alpha, col32_no_alpha & ~IM_COL32_A_MASK, col32_no_alpha & ~IM_COL32_A_MASK);
		float bar1_line_y = (float)(int)(picker_pos.y + (1.0f - alpha) * sv_picker_size + 0.5f);
		RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
		RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f);
	}

	EndGroup();
	PopID();

	float col2[] = {
		val[2] == 0 ? 0 : val[2] / 255.0f,
		val[1] == 0 ? 0 : val[1] / 255.0f,
		val[0] == 0 ? 0 : val[0] / 255.0f,
		val[3] == 0 ? 0 : val[3] / 255.0f };

	return value_changed && memcmp(backup_initial_col, col2, components * sizeof(float));
}
