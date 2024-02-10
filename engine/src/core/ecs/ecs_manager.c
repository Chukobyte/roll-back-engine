#include "ecs_manager.h"

#define SKA_MAX_ENTITIES 1000

#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>
#include <seika/asset/asset_manager.h>

#include "ecs_globals.h"

#include "component.h"
#include "component/animated_sprite_component.h"
#include "component/collider2d_component.h"
#include "component/color_rect_component.h"
#include "component/parallax_component.h"
#include "component/particles2d_component.h"
#include "component/sprite_component.h"
#include "component/script_component.h"
#include "component/text_label_component.h"
#include "component/transform2d_component.h"
#include "systems/animated_sprite_rendering_ec_system.h"
#include "systems/collision_ec_system.h"
#include "systems/color_rect_ec_system.h"
#include "systems/font_rendering_ec_system.h"
#include "systems/parallax_ec_system.h"
#include "systems/particle_emitter_ec_system.h"
#include "systems/script_ec_system.h"
#include "systems/sprite_rendering_ec_system.h"
#include "../scene/scene_manager.h"
#include "../game_properties.h"

static SceneTreeNode* fpsDisplayNode = NULL;

static void register_components() {
    const SkaComponentTypeInfo* animSpriteTypeInfo = SKA_ECS_REGISTER_COMPONENT(AnimatedSpriteComponent);
    const SkaComponentTypeInfo* collider2dTypeInfo = SKA_ECS_REGISTER_COMPONENT(Collider2DComponent);
    const SkaComponentTypeInfo* colorRectTypeInfo = SKA_ECS_REGISTER_COMPONENT(ColorRectComponent);
    const SkaComponentTypeInfo* nodeTypeInfo = SKA_ECS_REGISTER_COMPONENT(NodeComponent);
    const SkaComponentTypeInfo* parallaxTypeInfo = SKA_ECS_REGISTER_COMPONENT(ParallaxComponent);
    const SkaComponentTypeInfo* particles2dTypeInfo = SKA_ECS_REGISTER_COMPONENT(Particles2DComponent);
    const SkaComponentTypeInfo* scriptTypeInfo = SKA_ECS_REGISTER_COMPONENT(ScriptComponent);
    const SkaComponentTypeInfo* spriteTypeInfo = SKA_ECS_REGISTER_COMPONENT(SpriteComponent);
    const SkaComponentTypeInfo* textLabelTypeInfo = SKA_ECS_REGISTER_COMPONENT(TextLabelComponent);
    const SkaComponentTypeInfo* transform2dTypeInfo = SKA_ECS_REGISTER_COMPONENT(Transform2DComponent);
    // Update globals
    ANIMATED_SPRITE_COMPONENT_INDEX = animSpriteTypeInfo->index;
    ANIMATED_SPRITE_COMPONENT_TYPE = animSpriteTypeInfo->type;
    COLLIDER2D_COMPONENT_INDEX = collider2dTypeInfo->index;
    COLLIDER2D_COMPONENT_TYPE = collider2dTypeInfo->type;
    COLOR_RECT_COMPONENT_INDEX = colorRectTypeInfo->index;
    COLOR_RECT_COMPONENT_TYPE = colorRectTypeInfo->type;
    NODE_COMPONENT_INDEX = nodeTypeInfo->index;
    NODE_COMPONENT_TYPE = nodeTypeInfo->type;
    PARALLAX_COMPONENT_INDEX = parallaxTypeInfo->index;
    PARALLAX_COMPONENT_TYPE = parallaxTypeInfo->type;
    PARTICLES2D_COMPONENT_INDEX = particles2dTypeInfo->index;
    PARTICLES2D_COMPONENT_TYPE = particles2dTypeInfo->type;
    SCRIPT_COMPONENT_INDEX = scriptTypeInfo->index;
    SCRIPT_COMPONENT_TYPE = scriptTypeInfo->type;
    SPRITE_COMPONENT_INDEX = spriteTypeInfo->index;
    SPRITE_COMPONENT_TYPE = spriteTypeInfo->type;
    TEXT_LABEL_COMPONENT_INDEX = textLabelTypeInfo->index;
    TEXT_LABEL_COMPONENT_TYPE = textLabelTypeInfo->type;
    TRANSFORM2D_COMPONENT_INDEX = transform2dTypeInfo->index;
    TRANSFORM2D_COMPONENT_TYPE = transform2dTypeInfo->type;
}

void cre_ecs_manager_initialize() {
    ska_ecs_initialize();

    register_components();

    // Create and Register Systems
    cre_sprite_rendering_ec_system_create_and_register();
    cre_animated_sprite_rendering_ec_system_create_and_register();
    cre_collision_ec_system_create_and_register();
    cre_color_rect_ec_system_create_and_register();
    cre_font_rendering_ec_system_create_and_register();
    cre_parallax_ec_system_create_and_register();
    cre_particle_ec_system_create_and_register();
    cre_script_ec_system_create_and_register();

//    ska_ecs_system_register(cre_sprite_rendering_ec_system_create());
//    ska_ecs_system_register(cre_animated_sprite_rendering_ec_system_create());
//    ska_ecs_system_register(cre_font_rendering_ec_system_create());
//    ska_ecs_system_register(cre_script_ec_system_create());
//    ska_ecs_system_register(cre_collision_ec_system_create());
//    ska_ecs_system_register(cre_color_rect_ec_system_create());
//    ska_ecs_system_register(cre_parallax_ec_system_create());
//    ska_ecs_system_register(cre_particle_emitter_ec_system_create());

//    cre_component_manager_initialize();
//    cre_ec_system_initialize();
    // Initialize and register ec systems
//    cre_ec_system_register(cre_sprite_rendering_ec_system_create());
//    cre_ec_system_register(cre_animated_sprite_rendering_ec_system_create());
//    cre_ec_system_register(cre_font_rendering_ec_system_create());
//    cre_ec_system_register(cre_script_ec_system_create());
//    cre_ec_system_register(cre_collision_ec_system_create());
//    cre_ec_system_register(cre_color_rect_ec_system_create());
//    cre_ec_system_register(cre_parallax_ec_system_create());
//    cre_ec_system_register(cre_particle_emitter_ec_system_create());
}

void cre_ecs_manager_initialize_ex(SETexture* colorRectTexture, SETexture* particle2DSquareTexture) {
    ska_ecs_initialize();

    register_components();

    cre_sprite_rendering_ec_system_create_and_register();
    cre_animated_sprite_rendering_ec_system_create_and_register();
    cre_collision_ec_system_create_and_register();
    cre_color_rect_ec_system_create_and_register_ex(colorRectTexture);
    cre_font_rendering_ec_system_create_and_register();
    cre_parallax_ec_system_create_and_register();
    cre_particle_ec_system_create_and_register_ex(particle2DSquareTexture);
    cre_script_ec_system_create_and_register();

//    cre_component_manager_initialize();
//    cre_ec_system_initialize();
    // Initialize and register ec systems
//    cre_ec_system_register(cre_sprite_rendering_ec_system_create());
//    cre_ec_system_register(cre_animated_sprite_rendering_ec_system_create());
//    cre_ec_system_register(cre_font_rendering_ec_system_create());
//    cre_ec_system_register(cre_script_ec_system_create());
//    cre_ec_system_register(cre_collision_ec_system_create());
//    cre_ec_system_register(cre_color_rect_ec_system_create_ex(colorRectTexture));
//    cre_ec_system_register(cre_parallax_ec_system_create());
//    cre_ec_system_register(cre_particle_emitter_ec_system_create_ex(particle2DSquareTexture));
}

void cre_ecs_manager_enable_fps_display_entity(bool enabled, const char* fontUID, float positionX, float positionY) {
    static bool isEnabled = false;
    static SkaEntity currentFpsEntity = SKA_NULL_ENTITY;
    // Create temp entity
    if (!isEnabled && enabled) {
        fpsDisplayNode = cre_scene_tree_create_tree_node(ska_ecs_entity_create(), NULL);
        currentFpsEntity = fpsDisplayNode->entity;
        // Transform 2D
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position.x = positionX;
        transform2DComponent->localTransform.position.y = positionY;
        transform2DComponent->ignoreCamera = true;
        transform2DComponent->zIndex = SE_RENDERER_MAX_Z_INDEX;
        ska_ecs_component_manager_set_component(currentFpsEntity, TRANSFORM2D_COMPONENT_INDEX, transform2DComponent);
        // Text Label Component
        TextLabelComponent* textLabelComponent = text_label_component_create();
        textLabelComponent->font = se_asset_manager_get_font(fontUID != NULL ? fontUID : CRE_DEFAULT_FONT_ASSET.uid);
        se_strcpy(textLabelComponent->text, "FPS: ");
        ska_ecs_component_manager_set_component(currentFpsEntity, TEXT_LABEL_COMPONENT_INDEX, textLabelComponent);
        // Script Component
        ScriptComponent* scriptComponent = script_component_create("main", "FpsDisplay");
        scriptComponent->contextType = ScriptContextType_NATIVE;
        ska_ecs_component_manager_set_component(currentFpsEntity, SCRIPT_COMPONENT_INDEX, scriptComponent);
        // Update systems
        ska_ecs_system_update_entity_signature_with_systems(currentFpsEntity);
        cre_scene_manager_queue_node_for_creation(fpsDisplayNode);
    } else if (isEnabled && !enabled) {
        SE_ASSERT_FMT(currentFpsEntity != SKA_NULL_ENTITY, "Current fps entity is a null entity!?");
        SE_ASSERT_FMT(fpsDisplayNode != NULL, "FPS Display Node is NULL!?");
        cre_queue_destroy_tree_node_entity_all(fpsDisplayNode);
        currentFpsEntity = SKA_NULL_ENTITY;
        fpsDisplayNode = NULL;
    }
    isEnabled = enabled;
}

void cre_ecs_manager_finalize() {
    ska_ecs_finalize();

//    cre_ec_system_finalize();
//    cre_component_manager_finalize();
}
