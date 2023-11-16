#include "unity.h"

#include <stdbool.h>

#include <SDL2/SDL_main.h>

#include <seika/rendering/texture.h>

#include "../src/core/node_event.h"
#include "../src/core/ecs/component/component.h"
#include "../src/core/ecs/component/transform2d_component.h"
#include "../src/core/ecs/component/collider2d_component.h"
#include "../src/core/ecs/component/text_label_component.h"
#include "../src/core/ecs/system/ec_system.h"
#include "../src/core/ecs/ecs_manager.h"
#include "../src/core/json/json_file_loader.h"
#include "../src/core/game_properties.h"

SETexture fakeColorRectTexture = {0};

void setUp() {
    cre_game_props_initialize(cre_game_props_create());
    cre_ecs_manager_initialize_ex(&fakeColorRectTexture);
}
void tearDown() {
    cre_ecs_manager_finalize();
    cre_game_props_finalize();
}

void cre_node_event_test(void);
void cre_json_file_loader_scene_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(cre_node_event_test);
    RUN_TEST(cre_json_file_loader_scene_test);
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
    const CreEntity eventEntity = 1;
    const CreEntity observerEntity = 2;
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

    const CreEntity anotherObserverEntity = 3;
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
    TEST_ASSERT_NOT_NULL(rootNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* rootTransformComp = (Transform2DComponent*) rootNode->components[CreComponentDataIndex_TRANSFORM_2D];
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
    TEST_ASSERT_NOT_NULL(playerNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* playerTransformComp = (Transform2DComponent*) playerNode->components[CreComponentDataIndex_TRANSFORM_2D];
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
    TEST_ASSERT_NOT_NULL(ballNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* ballTransformComp = (Transform2DComponent*) ballNode->components[CreComponentDataIndex_TRANSFORM_2D];
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
    Transform2DComponent* ballColliderTransformComp = (Transform2DComponent*) ballColliderNode->components[CreComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_NOT_NULL(ballColliderTransformComp);
    Collider2DComponent* ballColliderCollider2DComp = (Collider2DComponent*) ballColliderNode->components[CreComponentDataIndex_COLLIDER_2D];
    TEST_ASSERT_NOT_NULL(ballColliderCollider2DComp);
    // BALL TEXT LABEL (Not in external scene but added as a child to the local scene)
    JsonSceneNode* ballTextLabel = ballNode->children[1];
    TEST_ASSERT_NOT_NULL(ballTextLabel);
    Transform2DComponent * ballTextTransform2DComp = (Transform2DComponent*) ballTextLabel->components[CreComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_NOT_NULL(ballTextTransform2DComp);
    TextLabelComponent* ballTextLabelComp = (TextLabelComponent*) ballTextLabel->components[CreComponentDataIndex_TEXT_LABEL];
    TEST_ASSERT_NOT_NULL(ballTextLabelComp);


    cre_json_delete_json_scene_node(rootNode);
}
