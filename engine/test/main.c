#include "unity.h"

#include <stdbool.h>

#include <SDL3/SDL_main.h>

#include <seika/memory.h>
#include <seika/file_system.h>
#include <seika/string.h>
#include <seika/asset/asset_manager.h>
#include <seika/rendering/texture.h>

#include "core/node_event.h"
#include "core/ecs/ecs_globals.h"
#include "core/ecs/components/collider2d_component.h"
#include "core/ecs/components/text_label_component.h"
#include "core/ecs/components/transform2d_component.h"
#include "core/ecs/ecs_manager.h"
#include "core/json/json_file_loader.h"
#include "core/game_properties.h"
#include "core/engine_context.h"
#include "core/scene/scene_manager.h"
#include "core/tilemap/tilemap.h"
#include "core/scripting/python/pocketpy/pkpy_util.h"

inline static SkaTexture* create_mock_texture() {
    SkaTexture* texture = SKA_ALLOC_ZEROED(SkaTexture);
    return texture;
}

void setUp() {
    cre_game_props_initialize(cre_game_props_create());

    cre_ecs_manager_initialize_ex(create_mock_texture(), create_mock_texture());

    CREEngineContext* engineContext = cre_engine_context_initialize();
    engineContext->engineRootDir = ska_fs_get_cwd();
    engineContext->internalAssetsDir = ska_fs_get_cwd();
}
void tearDown() {
    cre_ecs_manager_finalize();
    cre_game_props_finalize();
    cre_engine_context_finalize();
}

void cre_node_event_test(void);
void cre_json_file_loader_scene_test(void);
void cre_pocketpy_api_test(void);
void cre_tilemap_test(void);

int32 main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(cre_node_event_test);
    RUN_TEST(cre_json_file_loader_scene_test);
    RUN_TEST(cre_pocketpy_api_test);
    RUN_TEST(cre_tilemap_test);
    return UNITY_END();
}

//--- Node event test ---//
static bool hasBeenNotified = false;
static int NODE_EVENT_TEST_NUMBER = 345;

void node_event_callback(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    hasBeenNotified = true;
}

void node_event_callback2(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    const int number = *(int*) notifyPayload->data;
    if (number == NODE_EVENT_TEST_NUMBER) {
        hasBeenNotified = true;
    }
}

void cre_node_event_test(void) {
    const SkaEntity eventEntity = 1;
    const SkaEntity observerEntity = 2;
    const char* eventId = "walk";

    // Test Empty
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_count(eventEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));

    node_event_create_event(eventEntity, eventId);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_count(eventEntity));

    node_event_subscribe_to_event(eventEntity, eventId, observerEntity, node_event_callback, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(observerEntity));

    // Test notify
    node_event_notify_observers(eventEntity, eventId, &(NodeEventNotifyPayload){ .data = NULL });
    TEST_ASSERT_TRUE(hasBeenNotified);
    hasBeenNotified = false;

    const SkaEntity anotherObserverEntity = 3;
    node_event_subscribe_to_event(eventEntity, eventId, anotherObserverEntity, node_event_callback2, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(2, node_event_get_event_observer_count(eventEntity, eventId));
    node_event_notify_observers(eventEntity, eventId, &(NodeEventNotifyPayload){ .data = &NODE_EVENT_TEST_NUMBER });
    TEST_ASSERT_TRUE(hasBeenNotified);
    node_event_destroy_all_entity_events_and_observers(anotherObserverEntity);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_observer_count(eventEntity, eventId));

    node_event_destroy_all_entity_events_and_observers(eventEntity);
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_count(eventEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));

    // Test to make sure entity event removal also removes observer entries
    node_event_create_event(eventEntity, eventId);
    node_event_subscribe_to_event(eventEntity, eventId, observerEntity, node_event_callback, NULL, NULL);
    node_event_subscribe_to_event(eventEntity, eventId, anotherObserverEntity, node_event_callback, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(2, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(observerEntity));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(anotherObserverEntity));
    node_event_destroy_all_entity_events_and_observers(eventEntity);
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(anotherObserverEntity));
}

//--- Json File Loader Tests ---//
// Note: If making changes to scene file make sure cmake runs steps to copy test dependency resources

#define TEST_SCENE_1_PATH "engine/test/resources/test_scene1.cscn"

void cre_json_file_loader_scene_test(void) {
    // ROOT NODE
    JsonSceneNode* rootNode = cre_json_load_scene_file(TEST_SCENE_1_PATH);
    TEST_ASSERT_NOT_NULL(rootNode);
    TEST_ASSERT_EQUAL_STRING("Main", rootNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, rootNode->type);
    TEST_ASSERT_NULL(rootNode->tags);
    TEST_ASSERT_NULL(rootNode->externalNodeSource);
    TEST_ASSERT_EQUAL_UINT(2, rootNode->childrenCount);
    // Root components
    TEST_ASSERT_NOT_NULL(rootNode->components[TRANSFORM2D_COMPONENT_INDEX]);
    Transform2DComponent* rootTransformComp = (Transform2DComponent*) rootNode->components[TRANSFORM2D_COMPONENT_INDEX];
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, rootTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, rootTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(0, rootTransformComp->zIndex);
    TEST_ASSERT_TRUE(rootTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(rootTransformComp->ignoreCamera);

    // PLAYER NODE (Child)
    JsonSceneNode* playerNode = rootNode->children[0];
    TEST_ASSERT_NOT_NULL(playerNode);
    TEST_ASSERT_EQUAL_STRING("Player", playerNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, playerNode->type);
    TEST_ASSERT_NULL(playerNode->tags);
    TEST_ASSERT_NULL(playerNode->externalNodeSource);
    TEST_ASSERT_EQUAL_INT(0, playerNode->childrenCount);
    // Player components
    TEST_ASSERT_NOT_NULL(playerNode->components[TRANSFORM2D_COMPONENT_INDEX]);
    Transform2DComponent* playerTransformComp = (Transform2DComponent*) playerNode->components[TRANSFORM2D_COMPONENT_INDEX];
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(1, playerTransformComp->zIndex);
    TEST_ASSERT_TRUE(playerTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(playerTransformComp->ignoreCamera);

    // BALL NODE (External scene child)
    JsonSceneNode* ballNode = rootNode->children[1];
    TEST_ASSERT_NOT_NULL(ballNode);
    TEST_ASSERT_EQUAL_STRING("TestBall", ballNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, ballNode->type);
    TEST_ASSERT_NULL(ballNode->tags);
    TEST_ASSERT_EQUAL_STRING("engine/test/resources/ball.cscn", ballNode->externalNodeSource);
    TEST_ASSERT_EQUAL_INT(2, ballNode->childrenCount);
    // Ball components
    TEST_ASSERT_NOT_NULL(ballNode->components[TRANSFORM2D_COMPONENT_INDEX]);
    Transform2DComponent* ballTransformComp = (Transform2DComponent*) ballNode->components[TRANSFORM2D_COMPONENT_INDEX];
    // Testing to make sure position in current scene file overrides the default position in the 'ball.cscn'
    TEST_ASSERT_EQUAL_FLOAT(100.0f, ballTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(110.0f, ballTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, ballTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, ballTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, ballTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(0, ballTransformComp->zIndex);
    TEST_ASSERT_TRUE(ballTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(ballTransformComp->ignoreCamera);
    // BALL NODE CHILD COLLIDER
    JsonSceneNode* ballColliderNode = ballNode->children[0];
    TEST_ASSERT_NOT_NULL(ballColliderNode);
    Transform2DComponent* ballColliderTransformComp = (Transform2DComponent*) ballColliderNode->components[TRANSFORM2D_COMPONENT_INDEX];
    TEST_ASSERT_NOT_NULL(ballColliderTransformComp);
    Collider2DComponent* ballColliderCollider2DComp = (Collider2DComponent*) ballColliderNode->components[COLLIDER2D_COMPONENT_INDEX];
    TEST_ASSERT_NOT_NULL(ballColliderCollider2DComp);
    // BALL TEXT LABEL (Not in external scene but added as a child to the local scene)
    JsonSceneNode* ballTextLabel = ballNode->children[1];
    TEST_ASSERT_NOT_NULL(ballTextLabel);
    Transform2DComponent * ballTextTransform2DComp = (Transform2DComponent*) ballTextLabel->components[TRANSFORM2D_COMPONENT_INDEX];
    TEST_ASSERT_NOT_NULL(ballTextTransform2DComp);
    TextLabelComponent* ballTextLabelComp = (TextLabelComponent*) ballTextLabel->components[TEXT_LABEL_COMPONENT_INDEX];
    TEST_ASSERT_NOT_NULL(ballTextLabelComp);

    cre_json_delete_json_scene_node(rootNode);
}

//--- Pocketpy API Test ---//
void cre_pocketpy_api_test(void) {
    ska_asset_manager_initialize();
    cre_scene_manager_initialize();
    cre_scene_manager_queue_scene_change("engine/test/resources/test_scene1.cscn");
    cre_scene_manager_process_queued_scene_change();

    CREGameProperties* testGameProps = cre_game_props_create();
    testGameProps->gameTitle = ska_strdup("Test Game");
    testGameProps->resolutionWidth = 400;
    testGameProps->resolutionHeight = 300;
    testGameProps->initialScenePath = ska_strdup("main.cscn");
    cre_game_props_initialize(testGameProps);

    TEST_MESSAGE("Testing python api");

    // Load test node
    char* testCustomNodesSource = ska_fs_read_file_contents("engine/test/resources/test_custom_nodes.py", NULL);
    cre_pkpy_util_create_from_string("test_custom_nodes", testCustomNodesSource);
    // Load test file
    char* pythonText = ska_fs_read_file_contents("engine/test/resources/crescent_api_test.py", NULL);
    TEST_ASSERT_NOT_NULL(pythonText);
    py_exec(pythonText, "crescent_api_test.py", EXEC_MODE, NULL);
    SKA_FREE(testCustomNodesSource);
    SKA_FREE(pythonText);
    if (py_checkexc(false)) { printf("PKPY Error:\n%s", py_formatexc()); }
    TEST_ASSERT_FALSE(py_checkexc(false));

    cre_scene_manager_finalize();
    ska_asset_manager_finalize();
    cre_game_props_finalize();
}

//--- Tilemap Test ---//
void cre_tilemap_test(void) {
    CreTilemap tilemap = CRE_TILEMAP_DEFAULT_EMPTY;
    cre_tilemap_initialize(&tilemap);

    // Test setting (and unsetting) a single tile in an empty tile map
    const SkaVector2i tileOnePosition = (SkaVector2i){ .x = 5, .y = 5 };
    cre_tilemap_set_tile_active(&tilemap, &tileOnePosition, true);
    cre_tilemap_commit_active_tile_changes(&tilemap);
    TEST_ASSERT_EQUAL_INT(6, tilemap.tilesArray->size.w);
    TEST_ASSERT_EQUAL_INT(6, tilemap.tilesArray->size.h);
    TEST_ASSERT_EQUAL_INT(CreTileType_CENTER, cre_tilemap_get_tile_bitmask(&tilemap, &tileOnePosition));
    cre_tilemap_set_tile_active(&tilemap, &tileOnePosition, false);
    cre_tilemap_commit_active_tile_changes(&tilemap);
    TEST_ASSERT_EQUAL_INT(0, tilemap.tilesArray->size.w);
    TEST_ASSERT_EQUAL_INT(0, tilemap.tilesArray->size.h);

    // 3 x 3 minimal bitmask tests
    const SkaVector2i tileTwoPosition = { .x = 9, .y = 4 };
    const SkaVector2i tileThreePosition = { .x = 8, .y = 4 };

    const SkaVector2i tileFourPosition = { .x = 3, .y = 8 };
    const SkaVector2i tileFivePosition = { .x = 3, .y = 7 };

    cre_tilemap_set_tile_active(&tilemap, &tileTwoPosition, true);
    cre_tilemap_set_tile_active(&tilemap, &tileThreePosition, true);
    cre_tilemap_set_tile_active(&tilemap, &tileFourPosition, true);
    cre_tilemap_set_tile_active(&tilemap, &tileFivePosition, true);
    cre_tilemap_set_tile_render_coord(&tilemap, &tileFourPosition, &(SkaVector2i){ 1, 3 });
    cre_tilemap_commit_active_tile_changes(&tilemap);
    const CreTileData* tileFourData = cre_tilemap_get_tile_data(&tilemap, &tileFourPosition);
    TEST_ASSERT_EQUAL_INT(10, tilemap.tilesArray->size.w);
    TEST_ASSERT_EQUAL_INT(9, tilemap.tilesArray->size.h);
    TEST_ASSERT_EQUAL_INT(CreTileType_CENTER | CreTileType_LEFT, cre_tilemap_get_tile_bitmask(&tilemap, &tileTwoPosition));
    TEST_ASSERT_EQUAL_INT(CreTileType_CENTER | CreTileType_RIGHT, cre_tilemap_get_tile_bitmask(&tilemap, &tileThreePosition));
    TEST_ASSERT_EQUAL_INT(CreTileType_CENTER | CreTileType_TOP, tileFourData->bitmask);
    TEST_ASSERT_EQUAL_INT(CreTileType_CENTER | CreTileType_BOTTOM, cre_tilemap_get_tile_bitmask(&tilemap, &tileFivePosition));
    TEST_ASSERT_EQUAL_INT(1, tileFourData->renderCoords.x);
    TEST_ASSERT_EQUAL_INT(3, tileFourData->renderCoords.y);

    cre_tilemap_finalize(&tilemap);
}
