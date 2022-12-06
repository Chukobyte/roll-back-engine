#include "native_script_context.h"

#include "../seika/src/data_structures/se_hash_map.h"
#include "../seika/src/data_structures/se_hash_map_string.h"
#include "../seika/src/data_structures/se_static_array.h"
#include "../seika/src/utils/se_assert.h"
#include "../seika/src/memory/se_mem.h"

#include "../script_context.h"

#define MAX_NATIVE_CLASSES 4
#define MAX_NATIVE_CLASS_ENTITIES 8

// --- Script Context Interface --- //
void native_on_create_instance(Entity entity, const char* classPath, const char* className);
void native_on_delete_instance(Entity entity);
void native_on_start(Entity entity);
void native_on_update_instance(Entity entity, float deltaTime);
void native_on_physics_update_instance(Entity entity, float deltaTime);
void native_on_end(Entity entity);

// Script Cache
SEStringHashMap* classCache = NULL;
SEHashMap* entityToClassName = NULL;

CREScriptContext* native_script_context = NULL;

CREScriptContext* cre_native_create_script_context() {
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_create_instance = native_on_create_instance;
    scriptContext->on_delete_instance = native_on_delete_instance;
    scriptContext->on_start = native_on_start;
    scriptContext->on_update_instance = native_on_update_instance;
    scriptContext->on_physics_update_instance = native_on_physics_update_instance;
    scriptContext->on_end = native_on_end;

    SE_ASSERT(classCache == NULL);
    classCache = se_string_hash_map_create(MAX_NATIVE_CLASSES);

    SE_ASSERT(entityToClassName == NULL);
    entityToClassName = se_hash_map_create(sizeof(Entity), sizeof(CRENativeScriptClass **), MAX_NATIVE_CLASS_ENTITIES);

    native_script_context = scriptContext;

    return scriptContext;
}

void cre_native_class_register_new_class(CRENativeScriptClass* scriptClass) {
    if (se_string_hash_map_has(classCache, scriptClass->name)) {
        se_logger_warn("Already have script class registered!\nname: '%s', path: '%s'", scriptClass->name,
                       scriptClass->path);
        return;
    }
    se_logger_debug("register native c/c++ class, name: %s, path: %s", scriptClass->name, scriptClass->path);
    se_string_hash_map_add(classCache, scriptClass->name, &scriptClass, scriptClass->class_instance_size);
}

void native_on_create_instance(Entity entity, const char* classPath, const char* className) {
    SE_ASSERT_FMT(se_string_hash_map_has(classCache, className), "Class ref not cached!  entity: '%d', class_path: '%s', class_name: '%s'", entity, classPath, className);
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_string_hash_map_get(
            classCache, className);
    SE_ASSERT(scriptClassRef != NULL);
    SE_ASSERT(scriptClassRef->create_new_instance_func != NULL);
    CRENativeScriptClass* newScriptClass = scriptClassRef->create_new_instance_func(entity);
    se_hash_map_add(entityToClassName, &entity, &newScriptClass);
    if (newScriptClass->update_func != NULL) {
        native_script_context->updateEntities[native_script_context->updateEntityCount++] = entity;
    }
    if (newScriptClass->physics_update_func != NULL) {
        native_script_context->physicsUpdateEntities[native_script_context->physicsUpdateEntityCount++] = entity;
    }
}

void native_on_delete_instance(Entity entity) {
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_hash_map_get(
            entityToClassName, &entity);

    if (scriptClassRef->update_func != NULL) {
        se_array_utils_remove_item_uint32(
            native_script_context->updateEntities,
            &native_script_context->updateEntityCount,
            entity,
            NULL_ENTITY
        );
    }
    if (scriptClassRef->physics_update_func != NULL) {
        se_array_utils_remove_item_uint32(
            native_script_context->physicsUpdateEntities,
            &native_script_context->physicsUpdateEntityCount,
            entity,
            NULL_ENTITY
        );
    }

    SE_MEM_FREE(scriptClassRef);
    se_hash_map_erase(entityToClassName, &entity);
}

void native_on_start(Entity entity) {
    SE_ASSERT(se_hash_map_has(entityToClassName, &entity));
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_hash_map_get(
            entityToClassName, &entity);
    scriptClassRef->on_start_func(scriptClassRef);
}

void native_on_update_instance(Entity entity, float deltaTime) {
    CRENativeScriptClass* scriptClass = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_hash_map_get(entityToClassName, &entity);
    scriptClass->update_func(scriptClass, deltaTime);
}

void native_on_physics_update_instance(Entity entity, float deltaTime) {
    CRENativeScriptClass* scriptClass = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_hash_map_get(entityToClassName, &entity);
    scriptClass->physics_update_func(scriptClass, deltaTime);
}

void native_on_end(Entity entity) {
    SE_ASSERT(se_hash_map_has(entityToClassName, &entity));
    CRENativeScriptClass* scriptClassRef = (CRENativeScriptClass*) *(CRENativeScriptClass**) se_hash_map_get(
            entityToClassName, &entity);
    scriptClassRef->on_end_func(scriptClassRef);
}
