#include "sprite_rendering_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/ecs/ecs.h>

#include "../ecs_globals.h"

//#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/sprite_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

static void sprite_render(SkaECSSystem* system);

void cre_sprite_rendering_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Sprite Rendering");
    systemTemplate.render_func = sprite_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, SpriteComponent);
}

void sprite_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < system->entity_count; i++) {
        const CreEntity entity = system->entities[i];
        Transform2DComponent* spriteTransformComp = (Transform2DComponent*) ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        SpriteComponent* spriteComponent = (SpriteComponent*) ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
        const CRECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, spriteTransformComp, &spriteComponent->origin);
        const SKASize2D destinationSize = {
                spriteComponent->drawSource.w * renderCamera->zoom.x,
                spriteComponent->drawSource.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
                spriteComponent->texture,
                spriteComponent->drawSource,
                destinationSize,
                spriteComponent->modulate,
                spriteComponent->flipH,
                spriteComponent->flipV,
                &renderResource.transform2D,
                renderResource.globalZIndex,
                se_shader_cache_get_instance_checked(spriteComponent->shaderInstanceId)
        );
    }
}

//CreEntitySystem* spriteRenderingSystem = NULL;

//void sprite_rendering_system_render();
//void sprite_rendering_on_ec_system_destroy();
//
//CreEntitySystem* cre_sprite_rendering_ec_system_create() {
//    SE_ASSERT(spriteRenderingSystem == NULL);
//    spriteRenderingSystem = cre_ec_system_create();
//    spriteRenderingSystem->name = se_strdup("Sprite Rendering");
//    spriteRenderingSystem->render_func = sprite_rendering_system_render;
//    spriteRenderingSystem->on_ec_system_destroy = sprite_rendering_on_ec_system_destroy;
//    spriteRenderingSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_SPRITE;
//    return spriteRenderingSystem;
//}
//
//void sprite_rendering_system_render() {
//    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
//    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
//
//    for (size_t i = 0; i < spriteRenderingSystem->entity_count; i++) {
//        const CreEntity entity = spriteRenderingSystem->entities[i];
//        Transform2DComponent* spriteTransformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
//        SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
//        const CRECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
//        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, spriteTransformComp, &spriteComponent->origin);
//        const SKASize2D destinationSize = {
//            spriteComponent->drawSource.w * renderCamera->zoom.x,
//            spriteComponent->drawSource.h * renderCamera->zoom.y
//        };
//
//        ska_renderer_queue_sprite_draw(
//                spriteComponent->texture,
//                spriteComponent->drawSource,
//                destinationSize,
//                spriteComponent->modulate,
//                spriteComponent->flipH,
//                spriteComponent->flipV,
//                &renderResource.transform2D,
//                renderResource.globalZIndex,
//                se_shader_cache_get_instance_checked(spriteComponent->shaderInstanceId)
//        );
//    }
//}
//
//void sprite_rendering_on_ec_system_destroy() {
//    SE_ASSERT(spriteRenderingSystem != NULL);
//    spriteRenderingSystem = NULL;
//}
