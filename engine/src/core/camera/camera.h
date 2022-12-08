#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../seika/src/utils/observer.h"
#include "../seika/src/math/se_math.h"

#include "../ecs/entity/entity.h"

typedef enum CreCameraMode {
    CreCameraMode_MANUAL = 0,
    CreCameraMode_FOLLOW_ENTITY = 1,
} CreCameraMode;

typedef enum CreCameraArchorMode {
    CreCameraArchorMode_FIXED_TOP_LEFT = 0,
    CreCameraArchorMode_DRAG_CENTER = 1,
} CreCameraArchorMode;

typedef struct CRECamera2D {
    Rect2 boundary;
    Vector2 viewport;
    Vector2 offset;
    Vector2 zoom;
    CreCameraMode mode;
    CreCameraArchorMode archorMode;
    Entity entityFollowing;
    SEObserver onEntityTransformChangeObserver;
    SEObserver onEntityExitSceneObserver;
} CRECamera2D;

void cre_camera2d_clamp_viewport_to_boundary(CRECamera2D* camera2D);
void cre_camera2d_follow_entity(CRECamera2D* camera2D, Entity entity);
void cre_camera2d_unfollow_entity(CRECamera2D* camera2D, Entity entity);

#ifdef __cplusplus
}
#endif
