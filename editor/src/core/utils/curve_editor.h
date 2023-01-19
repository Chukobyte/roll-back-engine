#pragma once

#include "../ui/imgui/imgui_helper.h"
#include "imgui_internal.h"
#include "enum_class_utils.h"

enum class CurveEditorFlags {
    NO_TANGENTS = 1 << 0,
    SHOW_GRID = 1 << 1,
    ALL = NO_TANGENTS | SHOW_GRID,
};

GENERATE_ENUM_CLASS_OPERATORS(CurveEditorFlags)

int BeginCurveEditor(const char* label, float* values, int points_count, const ImVec2& editor_size, CurveEditorFlags flags, int* new_count);

class CurveEditor {
  public:
    int Begin(int* newCount = nullptr);

    std::string label;
    CurveEditorFlags flags = CurveEditorFlags::ALL;
    float* values = nullptr;
    int valueCount = 0;
    ImVec2 editorSize;
  private:
};
