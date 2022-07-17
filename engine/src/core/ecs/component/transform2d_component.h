#pragma once

#include <stdbool.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "../../math/rbe_math.h"

typedef struct Transform2DComponent {
    Vector2 position;
    Vector2 scale;
    float rotation; // Degrees
    int zIndex;
    bool isZIndexRelativeToParent;
    bool ignoreCamera;
    mat4 model;
} Transform2DComponent;

Transform2DComponent* transform2d_component_create();
void transform2d_component_get_local_model_matrix(mat4 model, Transform2DComponent* transform2DComponent);
Vector2 transform2d_component_get_position_from_model(mat4 model);
Vector2 transform2d_component_get_scale_from_model(mat4 model);
float transform2d_component_get_rotation_deg_from_model(mat4 model);
void transform2d_component_print(Transform2DComponent* transform2DComponent);
