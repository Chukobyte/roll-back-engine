#include "pkpy_api_impl.h"

#include <seika/assert.h>
#include <seika/input/input.h>
#include <seika/rendering/render_context.h>
#include <seika/rendering/frame_buffer.h>
#include <seika/rendering/shader/shader_instance_minimal.h>
#include <seika/rendering/shader/shader_instance.h>
#include <seika/rendering/shader/shader_cache.h>

#include "core/engine_context.h"
#include "core/game_properties.h"
#include "core/camera/camera.h"
#include "core/camera/camera_manager.h"
#include "core/ecs/ecs_manager.h"


// Helper functions
static inline SkaVector2 cre_pkpy_api_helper_mouse_get_global_position(const SkaVector2* offset) {
    SkaMouse* globalMouse = ska_input_get_mouse();
    const CRECamera2D* camera = cre_camera_manager_get_current_camera();
    CREGameProperties* gameProps = cre_game_props_get();
    SkaRenderContext* renderContext = ska_render_context_get();
    const SkaVector2 mouse_pixel_coord = {
        ska_math_map_to_range(globalMouse->position.x, 0.0f, (float) renderContext->windowWidth, 0.0f, (float) gameProps->resolutionWidth),
        ska_math_map_to_range(globalMouse->position.y, 0.0f, (float) renderContext->windowHeight, 0.0f, (float) gameProps->resolutionHeight)
};
    const SkaVector2 mouseWorldPos = {
        (camera->viewport.x + camera->offset.x + mouse_pixel_coord.x + offset->x) * camera->zoom.x,
        (camera->viewport.y + camera->offset.y + mouse_pixel_coord.y + offset->y) * camera->zoom.y
};
    return mouseWorldPos;
}

// Shader Instance
bool cre_pkpy_api_shader_instance_delete(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyShaderId = py_toint(py_arg(0));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    bool hasDeletedInstance = false;
    if (shaderInstance) {
        ska_shader_cache_remove_instance(shaderId);
        ska_shader_instance_destroy(shaderInstance);
    }
    py_newbool(py_retval(), hasDeletedInstance);
    return true;
}

bool cre_pkpy_api_shader_instance_create_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const bool value = py_tobool(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_bool(shaderInstance, paramName, value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const bool value = py_tobool(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_bool(shaderInstance, paramName, value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_bool_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const bool value = ska_shader_instance_param_get_bool(shaderInstance, paramName);
    py_newbool(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const py_i64 value = py_toint(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_int(shaderInstance, paramName, (int32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const py_i64 value = py_toint(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_int(shaderInstance, paramName, (int32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_int_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const int32 value = ska_shader_instance_param_get_int(shaderInstance, paramName);
    py_newint(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 value = py_tofloat(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_create_float(shaderInstance, paramName, (f32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 value = py_tofloat(py_arg(2));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    ska_shader_instance_param_update_float(shaderInstance, paramName, (f32)value);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const f32 value = ska_shader_instance_param_get_float(shaderInstance, paramName);
    py_newfloat(py_retval(), value);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 vecValue = (SkaVector2){ .x = (f32)valueX, .y = (f32)valueY };
    ska_shader_instance_param_create_float2(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 vecValue = (SkaVector2){ .x = (f32)valueX, .y = (f32)valueY };
    ska_shader_instance_param_update_float2(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float2_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector2 value = ska_shader_instance_param_get_float2(shaderInstance, paramName);
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 vecValue = (SkaVector3){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ };
    ska_shader_instance_param_create_float3(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 vecValue = (SkaVector3){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ };
    ska_shader_instance_param_update_float3(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float3_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector3 value = ska_shader_instance_param_get_float3(shaderInstance, paramName);
    py_newtuple(py_retval(), 3);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_Ref pyZ = py_tuple_getitem(py_retval(), 2);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    py_newfloat(pyZ, value.z);
    return true;
}

bool cre_pkpy_api_shader_instance_create_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(6);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));
    const f64 valueW = py_tofloat(py_arg(5));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 vecValue = (SkaVector4){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ, .w = (f32)valueW };
    ska_shader_instance_param_create_float4(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_set_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(6);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));
    const f64 valueX = py_tofloat(py_arg(2));
    const f64 valueY = py_tofloat(py_arg(3));
    const f64 valueZ = py_tofloat(py_arg(4));
    const f64 valueW = py_tofloat(py_arg(5));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 vecValue = (SkaVector4){ .x = (f32)valueX, .y = (f32)valueY, .z = (f32)valueZ, .w = (f32)valueW };
    ska_shader_instance_param_update_float4(shaderInstance, paramName, vecValue);
    return true;
}

bool cre_pkpy_api_shader_instance_get_float4_param(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const py_i64 pyShaderId = py_toint(py_arg(0));
    const char* paramName = py_tostr(py_arg(1));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    const SkaVector4 value = ska_shader_instance_param_get_float4(shaderInstance, paramName);
    py_newtuple(py_retval(), 4);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_Ref pyZ = py_tuple_getitem(py_retval(), 2);
    py_Ref pyW = py_tuple_getitem(py_retval(), 3);
    py_newfloat(pyX, value.x);
    py_newfloat(pyY, value.y);
    py_newfloat(pyZ, value.z);
    py_newfloat(pyW, value.w);
    return true;
}

// Shader Util
bool cre_pkpy_api_shader_util_compile_shader(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const char* shaderPath = py_tostr(py_arg(0));

    const SkaShaderInstanceId newId = ska_shader_cache_create_instance_and_add(shaderPath);
    SKA_ASSERT_FMT(newId != SKA_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from path '%s'", shaderPath);
    py_newint(py_retval(), newId);
    return true;
}

bool cre_pkpy_api_shader_util_compile_shader_raw(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(2);
    const char* vertexPath = py_tostr(py_arg(0));
    const char* fragmentPath = py_tostr(py_arg(1));

    const SkaShaderInstanceId newId = ska_shader_cache_create_instance_and_add_from_raw(vertexPath, fragmentPath);
    SKA_ASSERT_FMT(newId != SKA_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from paths: vertex = '%s', fragment = '%s'", vertexPath, fragmentPath);
    py_newint(py_retval(), newId);
    return true;
}

bool cre_pkpy_api_shader_util_set_screen_shader(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyShaderId = py_toint(py_arg(0));

    const SkaShaderInstanceId shaderId = (SkaShaderInstanceId)pyShaderId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(shaderId);
    bool hasSetShaderInstance = false;
    if (shaderInstance) {
        ska_frame_buffer_set_screen_shader(shaderInstance);
        hasSetShaderInstance = true;
    }
    py_newbool(py_retval(), hasSetShaderInstance);
    return true;
}

// TODO: Keep track of current screen shader
bool cre_pkpy_api_shader_util_get_current_screen_shader(int argc, py_StackRef argv) {
    py_newint(py_retval(), 0);
    return true;
}

bool cre_pkpy_api_shader_util_reset_screen_shader_to_default(int argc, py_StackRef argv) {
    ska_frame_buffer_reset_to_default_screen_shader();
    return true;
}

// Engine
bool cre_pkpy_api_engine_exit(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyExitCode = py_toint(py_arg(0));

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->isRunning = false;
    engineContext->exitCode = (int32)pyExitCode;
    return true;
}

bool cre_pkpy_api_engine_set_target_fps(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyTargetFPS = py_toint(py_arg(0));

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->targetFPS = (int32)pyTargetFPS;
    return true;
}

bool cre_pkpy_api_engine_get_target_fps(int argc, py_StackRef argv) {
    CREEngineContext* engineContext = cre_engine_context_get();
    py_newint(py_retval(), engineContext->targetFPS);
    return true;
}

bool cre_pkpy_api_engine_get_average_fps(int argc, py_StackRef argv) {
    CREEngineContext* engineContext = cre_engine_context_get();
    py_newint(py_retval(), (py_i64)engineContext->stats.averageFPS);
    return true;
}

bool cre_pkpy_api_engine_set_fps_display_enabled(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    const bool isEnabled = py_tobool(py_arg(0));
    const char* fontUID = py_tostr(py_arg(1));
    const py_f64 pyPosX = py_tofloat(py_arg(2));
    const py_f64 pyPosY = py_tofloat(py_arg(3));

    cre_ecs_manager_enable_fps_display_entity(isEnabled, fontUID, (f32)pyPosX, (f32)pyPosY);
    return true;
}

bool cre_pkpy_api_engine_get_global_physics_delta_time(int argc, py_StackRef argv) {
    py_newfloat(py_retval(), CRE_GLOBAL_PHYSICS_DELTA_TIME);
    return true;
}

// Input

bool cre_pkpy_api_input_is_key_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_pressed((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_key_just_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_just_pressed((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_key_just_released(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyKey = py_toint(py_arg(0));

    const bool isPressed = ska_input_is_key_just_released((SkaInputKey)pyKey, SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX);
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_add_action(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    const char* actionName = py_tostr(py_arg(0));
    const py_i64 pyValueKey = py_toint(py_arg(1));
    const py_i64 pyDeviceId = py_toint(py_arg(2));

    ska_input_add_input_action(
        actionName,
        (SkaInputActionValue[]){ { .key = (SkaInputKey)pyValueKey, .strengthThreshold = 0.5f }, { SkaInputKey_INVALID } },
        (SkaInputDeviceIndex)pyDeviceId
    );
    return true;
}

bool cre_pkpy_api_input_is_action_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const char* actionName = py_tostr(py_arg(0));

    // TODO: Probably should take device index as a param
    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isPressed = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_pressed(handle, deviceIndex) : false;
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_action_just_pressed(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const char* actionName = py_tostr(py_arg(0));

    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isPressed = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_just_pressed(handle, deviceIndex) : false;
    py_newbool(py_retval(), isPressed);
    return true;
}

bool cre_pkpy_api_input_is_action_just_released(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const char* actionName = py_tostr(py_arg(0));

    const SkaInputDeviceIndex deviceIndex = SKA_INPUT_FIRST_PLAYER_DEVICE_INDEX;
    const SkaInputActionHandle handle = ska_input_find_input_action_handle(actionName, deviceIndex);
    const bool isReleased = handle != SKA_INPUT_INVALID_INPUT_ACTION_HANDLE ? ska_input_is_input_action_just_released(handle, deviceIndex) : false;
    py_newbool(py_retval(), isReleased);
    return true;
}

// TODO: Finish implementing
bool cre_pkpy_api_input_start_gamepad_vibration(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    const py_i64 pyDeviceId = py_toint(py_arg(0));
    const py_f64 pyWeakMagnitude = py_tofloat(py_arg(1));
    const py_f64 pyStrongMagnitude = py_tofloat(py_arg(2));
    const py_f64 pyDurationSeconds = py_tofloat(py_arg(3));

    // ska_input_gamepad_start_vibration((int32)pyDeviceId, (f32)pyWeakMagnitude, (f32)pyStrongMagnitude, (f32)pyDurationSeconds);
    return true;
}

// TODO: Finish implementing
bool cre_pkpy_api_input_stop_gamepad_vibration(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    const py_i64 pyDeviceId = py_toint(py_arg(0));

    // ska_input_gamepad_stop_vibration((SkaInputDeviceIndex)pyDeviceId);
    return true;
}

bool cre_pkpy_api_input_mouse_get_position(int argc, py_StackRef argv) {
    const SkaMouse* globalMouse = ska_input_get_mouse();
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, (py_f64)globalMouse->position.x);
    py_newfloat(pyY, (py_f64)globalMouse->position.y);
    return true;
}

bool cre_pkpy_api_input_mouse_get_world_position(int argc, py_StackRef argv) {
    const SkaVector2 mouseWorldPosition = cre_pkpy_api_helper_mouse_get_global_position(&SKA_VECTOR2_ZERO);
    py_newtuple(py_retval(), 2);
    py_Ref pyX = py_tuple_getitem(py_retval(), 0);
    py_Ref pyY = py_tuple_getitem(py_retval(), 1);
    py_newfloat(pyX, (py_f64)mouseWorldPosition.x);
    py_newfloat(pyY, (py_f64)mouseWorldPosition.y);
    return true;
}

// Scene Tree
bool cre_pkpy_api_scene_tree_change_scene(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_scene_tree_get_root(int argc, py_StackRef argv) { return true; }

// Scene Manager
bool cre_pkpy_api_scene_manager_process_queued_creation_entities(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_scene_manager_process_queued_scene_change(int argc, py_StackRef argv) { return true; }

// Game Properties
bool cre_pkpy_api_game_properties_get(int argc, py_StackRef argv) { return true; }

// Camera2D
bool cre_pkpy_api_camera2d_set_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_boundary(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_boundary(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_follow_node(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_unfollow_node(int argc, py_StackRef argv) { return true; }

// World
bool cre_pkpy_api_world_set_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_delta_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_variable_delta_time(int argc, py_StackRef argv) { return true; }

// Audio Source
bool cre_pkpy_api_audio_source_set_pitch(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_audio_source_get_pitch(int argc, py_StackRef argv) { return true; }

// Audio Manager
bool cre_pkpy_api_audio_manager_play_sound(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_audio_manager_stop_sound(int argc, py_StackRef argv) { return true; }

// Game Config
bool cre_pkpy_api_game_config_save(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_game_config_load(int argc, py_StackRef argv) { return true; }

// Packed Scene
bool cre_pkpy_api_packed_scene_create_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_packed_scene_load(int argc, py_StackRef argv) { return true; }

// Collision Handler
bool cre_pkpy_api_collision_handler_process_collisions(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collision_handler_process_mouse_collisions(int argc, py_StackRef argv) { return true; }

// Network
bool cre_pkpy_api_network_is_server(int argc, py_StackRef argv) { return true; }
// Server
bool cre_pkpy_api_server_start(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_server_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_server_send(int argc, py_StackRef argv) { return true; }
// Client
bool cre_pkpy_api_client_start(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_client_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_client_send(int argc, py_StackRef argv) { return true; }

// Node
bool cre_pkpy_api_node_new(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_name(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_add_child(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_child(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_children(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_queue_deletion(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_is_queued_for_deletion(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_set_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_total_time_dilation(int argc, py_StackRef argv) { return true; }

// Node2D
bool cre_pkpy_api_node2d_set_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_global_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_ignore_camera(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_ignore_camera(int argc, py_StackRef argv) { return true; }

// Sprite
bool cre_pkpy_api_sprite_get_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Animated Sprite
bool cre_pkpy_api_animated_sprite_play(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_current_animation_frame(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_add_animation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Text Label
bool cre_pkpy_api_text_label_get_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_font_uid(int argc, py_StackRef argv) { return true; }

// Collider2D
bool cre_pkpy_api_collider2d_get_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_color(int argc, py_StackRef argv) { return true; }

// Color Rect
bool cre_pkpy_api_color_rect_get_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_color(int argc, py_StackRef argv) { return true; }

// Parallax
bool cre_pkpy_api_parallax_get_scroll_speed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_parallax_set_scroll_speed(int argc, py_StackRef argv) { return true; }

// Particles2D
bool cre_pkpy_api_particles2d_get_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_spread(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_spread(int argc, py_StackRef argv) { return true; }
