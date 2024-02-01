#include "color_rect_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/color_rect_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_utils.h"

CreEntitySystem* colorRectSystem = NULL;
SETexture* colorRectTexture = NULL;
SKARect2 colorRectDrawSource = { 0.0f, 0.0f, 1.0f, 1.0f };

void color_rect_system_render();
void color_rect_system_on_ec_system_destroy();

CreEntitySystem* cre_color_rect_ec_system_create() {
    return cre_color_rect_ec_system_create_ex(NULL);
}

struct CreEntitySystem* cre_color_rect_ec_system_create_ex(struct SETexture* rectTexture) {
    SE_ASSERT(colorRectSystem == NULL);
    colorRectSystem = cre_ec_system_create();
    colorRectSystem->name = se_strdup("Color Square");
    colorRectSystem->render_func = color_rect_system_render;
    colorRectSystem->on_ec_system_destroy = color_rect_system_on_ec_system_destroy;
    colorRectSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_COLOR_RECT;

    colorRectTexture = rectTexture != NULL ? rectTexture : se_texture_create_solid_colored_texture(1, 1, 255);
    return colorRectSystem;
}

void color_rect_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < colorRectSystem->entity_count; i++) {
        const CreEntity entity = colorRectSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
        const ColorRectComponent* colorRectComponent = (ColorRectComponent *) cre_component_manager_get_component(entity, CreComponentDataIndex_COLOR_RECT);
        const CRECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, transformComp, &SKA_VECTOR2_ZERO);
        const SKASize2D destinationSize = {
            colorRectComponent->size.w * renderCamera->zoom.x,
            colorRectComponent->size.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
            colorRectTexture,
            colorRectDrawSource,
            destinationSize,
            colorRectComponent->color,
            false,
            false,
            &renderResource.transform2D,
            renderResource.globalZIndex,
            NULL
        );
    }
}

void color_rect_system_on_ec_system_destroy() {
    SE_ASSERT(colorRectSystem != NULL);
    colorRectSystem = NULL;
    if (colorRectTexture) {
        se_texture_delete(colorRectTexture); // TODO: Properly delete texture (te
        colorRectTexture = NULL;
    }
}
