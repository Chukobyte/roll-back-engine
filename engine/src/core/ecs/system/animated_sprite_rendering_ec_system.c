#include "animated_sprite_rendering_ec_system.h"

#include <seika/seika.h>
#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/ecs/ecs.h>
#include <seika/utils/se_assert.h>

#include "../ecs_globals.h"

#include "ec_system.h"
#include "../component/animated_sprite_component.h"
#include "../component/transform2d_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

static void on_entity_registered(SkaECSSystem* system, SkaEntity entity);
static void animated_sprite_render(SkaECSSystem* system);

void cre_animated_sprite_rendering_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Animated Sprite Rendering");
    systemTemplate.on_entity_registered_func = on_entity_registered;
    systemTemplate.render_func = animated_sprite_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, AnimatedSpriteComponent);
}

void on_entity_registered(SkaECSSystem* system, SkaEntity entity) {
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    SE_ASSERT(animatedSpriteComponent != NULL);
    animated_sprite_component_refresh_random_stagger_animation_time(animatedSpriteComponent);
    if (animatedSpriteComponent->isPlaying) {
        animatedSpriteComponent->startAnimationTickTime = sf_get_ticks() + animatedSpriteComponent->randomStaggerTime;
    }
}

void animated_sprite_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    const int currentTickTime = (int) sf_get_ticks();
    for (size_t i = 0; i < system->entity_count; i++) {
        const CreEntity entity = system->entities[i];
        Transform2DComponent* spriteTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
        CreAnimationFrame currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[animatedSpriteComponent->currentAnimation.currentFrame];
        if (animatedSpriteComponent->isPlaying) {
            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            const float spriteCurrentTickTime = (float) currentTickTime + (float) animatedSpriteComponent->randomStaggerTime;
            const int tickRate = (int) (((spriteCurrentTickTime - (float) animatedSpriteComponent->startAnimationTickTime) / (float) animatedSpriteComponent->currentAnimation.speed) * entityTimeDilation);
            const int newIndex = tickRate % animatedSpriteComponent->currentAnimation.frameCount;
            if (newIndex != animatedSpriteComponent->currentAnimation.currentFrame) {
                // Notify observers that frame has changed
                se_event_notify_observers(&animatedSpriteComponent->onFrameChanged, &(SESubjectNotifyPayload){
                        .data = &(AnimatedSpriteFrameChangedPayload){ .entity = entity, .newFrame = newIndex }
                });

                currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[newIndex];
                if (newIndex + 1 == animatedSpriteComponent->currentAnimation.frameCount) {
                    // Notify the observers that the animation has finished
                    se_event_notify_observers(&animatedSpriteComponent->onFrameChanged, &(SESubjectNotifyPayload){
                            .data = &(AnimatedSpriteAnimationFinishedPayload){ .entity = entity, .animation = &animatedSpriteComponent->currentAnimation }
                    });
                    if (!animatedSpriteComponent->currentAnimation.doesLoop) {
                        animatedSpriteComponent->isPlaying = false;
                    }
                }
                animatedSpriteComponent->currentAnimation.currentFrame = newIndex;
            }
        }
        const CRECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, spriteTransformComp, &animatedSpriteComponent->origin);
        const SKASize2D destinationSize = {
                currentFrame.drawSource.w * renderCamera->zoom.x,
                currentFrame.drawSource.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
                currentFrame.texture,
                currentFrame.drawSource,
                destinationSize,
                animatedSpriteComponent->modulate,
                animatedSpriteComponent->flipH,
                animatedSpriteComponent->flipV,
                &renderResource.transform2D,
                renderResource.globalZIndex,
                se_shader_cache_get_instance_checked(animatedSpriteComponent->shaderInstanceId)
        );
    }
}

//CreEntitySystem* animatedSpriteRenderingSystem = NULL;
//
//static void animated_sprite_rendering_system_on_entity_registered(CreEntity entity);
//static void animated_sprite_rendering_system_render();
//static void animated_sprite_rendering_system_on_ec_system_destroy();
//
//CreEntitySystem* cre_animated_sprite_rendering_ec_system_create() {
//    SE_ASSERT(animatedSpriteRenderingSystem == NULL);
//    animatedSpriteRenderingSystem = cre_ec_system_create();
//    animatedSpriteRenderingSystem->name = se_strdup("Animated Sprite Rendering");
//    animatedSpriteRenderingSystem->on_entity_registered_func = animated_sprite_rendering_system_on_entity_registered;
//    animatedSpriteRenderingSystem->render_func = animated_sprite_rendering_system_render;
//    animatedSpriteRenderingSystem->on_ec_system_destroy = animated_sprite_rendering_system_on_ec_system_destroy;
//    animatedSpriteRenderingSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_ANIMATED_SPRITE;
//    return animatedSpriteRenderingSystem;
//}
//
//void animated_sprite_rendering_system_on_entity_registered(CreEntity entity) {
//    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
//    SE_ASSERT(animatedSpriteComponent != NULL);
//    animated_sprite_component_refresh_random_stagger_animation_time(animatedSpriteComponent);
//    if (animatedSpriteComponent->isPlaying) {
//        animatedSpriteComponent->startAnimationTickTime = sf_get_ticks() + animatedSpriteComponent->randomStaggerTime;
//    }
//}
//
//void animated_sprite_rendering_system_render() {
//    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
//    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
//    const int currentTickTime = (int) sf_get_ticks();
//    for (size_t i = 0; i < animatedSpriteRenderingSystem->entity_count; i++) {
//        const CreEntity entity = animatedSpriteRenderingSystem->entities[i];
//        Transform2DComponent* spriteTransformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
//        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
//        CreAnimationFrame currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[animatedSpriteComponent->currentAnimation.currentFrame];
//        if (animatedSpriteComponent->isPlaying) {
//            const float entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
//            const float spriteCurrentTickTime = (float) currentTickTime + (float) animatedSpriteComponent->randomStaggerTime;
//            const int tickRate = (int) (((spriteCurrentTickTime - (float) animatedSpriteComponent->startAnimationTickTime) / (float) animatedSpriteComponent->currentAnimation.speed) * entityTimeDilation);
//            const int newIndex = tickRate % animatedSpriteComponent->currentAnimation.frameCount;
//            if (newIndex != animatedSpriteComponent->currentAnimation.currentFrame) {
//                // Notify observers that frame has changed
//                se_event_notify_observers(&animatedSpriteComponent->onFrameChanged, &(SESubjectNotifyPayload){
//                    .data = &(AnimatedSpriteFrameChangedPayload){ .entity = entity, .newFrame = newIndex }
//                });
//
//                currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[newIndex];
//                if (newIndex + 1 == animatedSpriteComponent->currentAnimation.frameCount) {
//                    // Notify the observers that the animation has finished
//                    se_event_notify_observers(&animatedSpriteComponent->onFrameChanged, &(SESubjectNotifyPayload){
//                        .data = &(AnimatedSpriteAnimationFinishedPayload){ .entity = entity, .animation = &animatedSpriteComponent->currentAnimation }
//                    });
//                    if (!animatedSpriteComponent->currentAnimation.doesLoop) {
//                        animatedSpriteComponent->isPlaying = false;
//                    }
//                }
//                animatedSpriteComponent->currentAnimation.currentFrame = newIndex;
//            }
//        }
//        const CRECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
//        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, spriteTransformComp, &animatedSpriteComponent->origin);
//        const SKASize2D destinationSize = {
//            currentFrame.drawSource.w * renderCamera->zoom.x,
//            currentFrame.drawSource.h * renderCamera->zoom.y
//        };
//
//        ska_renderer_queue_sprite_draw(
//            currentFrame.texture,
//            currentFrame.drawSource,
//            destinationSize,
//            animatedSpriteComponent->modulate,
//            animatedSpriteComponent->flipH,
//            animatedSpriteComponent->flipV,
//            &renderResource.transform2D,
//            renderResource.globalZIndex,
//            se_shader_cache_get_instance_checked(animatedSpriteComponent->shaderInstanceId)
//        );
//    }
//}
//
//void animated_sprite_rendering_system_on_ec_system_destroy() {
//    SE_ASSERT(animatedSpriteRenderingSystem != NULL);
//    animatedSpriteRenderingSystem = NULL;
//}
