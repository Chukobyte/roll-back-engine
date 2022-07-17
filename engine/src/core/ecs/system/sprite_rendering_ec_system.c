#include "sprite_rendering_ec_system.h"

#include <string.h>

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/sprite_component.h"
#include "../../rendering/renderer.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

EntitySystem* spriteRenderingSystem = NULL;

void sprite_rendering_system_render();

EntitySystem* sprite_rendering_ec_system_create() {
    RBE_ASSERT(spriteRenderingSystem == NULL);
    spriteRenderingSystem = rbe_ec_system_create();
    spriteRenderingSystem->name = rbe_strdup("Sprite Rendering");
    spriteRenderingSystem->render_func = sprite_rendering_system_render;
    spriteRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_SPRITE;
    return spriteRenderingSystem;
}

void sprite_rendering_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();

    for (size_t i = 0; i < spriteRenderingSystem->entity_count; i++) {
        const Entity entity = spriteRenderingSystem->entities[i];
        const Transform2DComponent* spriteTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        const SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        const RBECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_graph_transform(entity);
        const Rect2 destinationRectangle = {
            (globalTransform->position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
            (globalTransform->position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
            spriteComponent->drawSource.w * globalTransform->scale.x * renderCamera->zoom.x,
            spriteComponent->drawSource.h * globalTransform->scale.y * renderCamera->zoom.y
        };
        rbe_renderer_queue_sprite_draw_call(
            spriteComponent->texture,
            spriteComponent->drawSource,
            destinationRectangle,
            spriteComponent->modulate,
            spriteComponent->flipX,
            spriteComponent->flipY,
            globalTransform
        );
    }
}
