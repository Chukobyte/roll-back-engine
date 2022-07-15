#pragma once

#define RBE_PI 3.14159265358979323846f
#define RBE_RAD_2_DEG (180.0f / RBE_PI)

// --- Vector2 --- //
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// --- Size2D --- //
typedef struct Size2D {
    float w;
    float h;
} Size2D;

// --- Rect2 --- //
typedef struct Rect2 {
    float x;
    float y;
    float w;
    float h;
} Rect2;

// --- Color --- //
typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color;

Color rbe_color_get_normalized_color_default_alpha(unsigned int r, unsigned int g, unsigned int b);
Color rbe_color_get_normalized_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
Color rbe_color_get_white();

// --- Misc --- //
float rbe_math_map_to_range(float input, float inputLow, float inputHigh, float outputLow, float outputHigh);
float rbe_math_map_to_unit(float input, float inputLow, float inputHigh);
