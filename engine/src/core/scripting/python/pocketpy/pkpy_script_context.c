#include "pkpy_script_context.h"

#include <pocketpy.h>

#include <seika/assert.h>
#include <seika/logger.h>
#include <seika/asset/asset_file_loader.h>

#include "seika/memory.h"

static CREScriptContext* scriptContext = NULL;
static py_Name startFunctionName;
static py_Name processFunctionName;
static py_Name fixedProcessFunctionName;
static py_Name endFunctionName;

static char* pkpy_import_file(const char* path);

//--- Script Context Interface ---//
static void pkpy_init(CREScriptContext* context);
static void pkpy_finalize(CREScriptContext* context);
static void pkpy_create_instance(SkaEntity entity, const char* classPath, const char* className);
static void pkpy_delete_instance(SkaEntity entity);
static void pkpy_on_start(SkaEntity entity);
static void pkpy_on_end(SkaEntity entity);
static void pkpy_on_update(SkaEntity entity, f32 deltaTime);
static void pkpy_on_fixed_update(SkaEntity entity, f32 deltaTime);
static void pkpy_network_callback(const char* message);


CREScriptContextTemplate cre_pkpy_get_script_context_template() {
    return (CREScriptContextTemplate){
        .contextType = CreScriptContextType_PYTHON,
        .on_script_context_init = pkpy_init,
        .on_script_context_finalize = pkpy_finalize,
        .on_create_instance = pkpy_create_instance,
        .on_delete_instance = pkpy_delete_instance,
        .on_start = pkpy_on_start,
        .on_pre_update_all = NULL,
        .on_post_update_all = NULL,
        .on_update_instance = pkpy_on_update,
        .on_fixed_update_instance = pkpy_on_fixed_update,
        .on_end = pkpy_on_end,
        .on_network_callback = pkpy_network_callback,
    };
}

void pkpy_init(CREScriptContext* context) {
    py_initialize();
    // Cache function names
    startFunctionName = py_name("_start");
    processFunctionName = py_name("_process");
    fixedProcessFunctionName = py_name("_fixed_process");
    endFunctionName = py_name("_end");
    // Setup callbacks
    py_Callbacks* callbacks = py_callbacks();
    callbacks->importfile = pkpy_import_file;

    scriptContext = context;
}

void pkpy_finalize(CREScriptContext* context) {
    scriptContext = NULL;
    py_finalize();
}

void pkpy_create_instance(SkaEntity entity, const char* classPath, const char* className) {}

void pkpy_delete_instance(SkaEntity entity) {}

void pkpy_on_start(SkaEntity entity) {}

void pkpy_on_end(SkaEntity entity) {}

void pkpy_on_update(SkaEntity entity, f32 deltaTime) {}

void pkpy_on_fixed_update(SkaEntity entity, f32 deltaTime) {}

void pkpy_network_callback(const char* message) {}

char* pkpy_import_file(const char* path) {
    // Use built in asset loader to load script instead of pkpy's default
    char* moduleString = ska_asset_file_loader_read_file_contents_as_string(path, NULL);
    if (!moduleString) {
        ska_logger_error("Failed to load pkpy module at path'%s'", path);
        return NULL;
    }
    return moduleString;
}
