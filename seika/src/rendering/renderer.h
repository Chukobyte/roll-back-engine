#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "texture.h"
#include "font.h"
#include "shader/shader_instance.h"
#include "../math/se_math.h"

void se_renderer_initialize(int inWindowWidth, int inWindowHeight, int inResolutionWidth, int inResolutionHeight);
void se_renderer_finalize();
void se_renderer_update_window_size(float windowWidth, float windowHeight);
void se_renderer_set_sprite_shader_default_params(SEShader* shader);
void se_renderer_queue_sprite_draw_call(Texture* texture, SERect2 sourceRect, SESize2D destSize, SEColor color, bool flipX, bool flipY, SETransformModel2D* globalTransform, int zIndex, SEShaderInstance* shaderInstance);
void se_renderer_queue_font_draw_call(SEFont* font, const char* text, float x, float y, float scale, SEColor color, int zIndex);
void se_renderer_process_and_flush_batches(const SEColor* backgroundColor);
void se_renderer_process_and_flush_batches_just_framebuffer(const SEColor* backgroundColor);

#ifdef __cplusplus
}
#endif
