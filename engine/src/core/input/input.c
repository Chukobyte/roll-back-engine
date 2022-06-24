#include "input.h"

#include <string.h>

#include "input_action.h"
#include "../data_structures/rbe_hash_map_string.h"
#include "../utils/logger.h"
#include "../utils/rbe_string_util.h"
#include "../utils/rbe_assert.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

void input_process_keyboard();
void input_process_mouse(SDL_Event event);
void input_load_gamepads();
void input_process_gamepad(SDL_Event event);

//--- Input ---//
RBEStringHashMap* inputActionMap = NULL;
const char* inputActionNames[24];
size_t inputActionNamesCount = 0;

RBEStringHashMap* keyboardStringValuesMap = NULL;
RBEStringHashMap* gamepadStringValuesMap = NULL;

typedef struct InputFlagCleaner {
    InputAction* inputActions[10];
    size_t count;
} InputFlagCleaner;

InputFlagCleaner actionJustPressedClean = {0};
InputFlagCleaner actionJustReleasedClean = {0};

bool rbe_input_initialize() {
    inputActionMap = rbe_string_hash_map_create(32);
    // SETUP INPUT STRING VALUES
    // Keyboard
    keyboardStringValuesMap = rbe_string_hash_map_create(80);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "0", SDL_SCANCODE_0);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "1", SDL_SCANCODE_1);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "2", SDL_SCANCODE_2);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "3", SDL_SCANCODE_3);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "4", SDL_SCANCODE_4);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "5", SDL_SCANCODE_5);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "6", SDL_SCANCODE_6);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "7", SDL_SCANCODE_7);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "8", SDL_SCANCODE_8);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "9", SDL_SCANCODE_9);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, "a", SDL_SCANCODE_A);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "b", SDL_SCANCODE_B);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "c", SDL_SCANCODE_C);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "d", SDL_SCANCODE_D);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "e", SDL_SCANCODE_E);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f", SDL_SCANCODE_F);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "g", SDL_SCANCODE_G);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "h", SDL_SCANCODE_H);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "i", SDL_SCANCODE_I);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "j", SDL_SCANCODE_J);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "k", SDL_SCANCODE_K);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "l", SDL_SCANCODE_L);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "m", SDL_SCANCODE_M);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "n", SDL_SCANCODE_N);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "o", SDL_SCANCODE_O);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "p", SDL_SCANCODE_P);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "q", SDL_SCANCODE_Q);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "r", SDL_SCANCODE_R);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "s", SDL_SCANCODE_S);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "t", SDL_SCANCODE_T);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "u", SDL_SCANCODE_U);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "v", SDL_SCANCODE_V);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "w", SDL_SCANCODE_W);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "x", SDL_SCANCODE_X);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "y", SDL_SCANCODE_Y);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "z", SDL_SCANCODE_Z);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, "left", SDL_SCANCODE_LEFT);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "right", SDL_SCANCODE_RIGHT);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "up", SDL_SCANCODE_UP);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "down", SDL_SCANCODE_DOWN);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, "return", SDL_SCANCODE_RETURN);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "space", SDL_SCANCODE_SPACE);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "esc", SDL_SCANCODE_ESCAPE);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f1", SDL_SCANCODE_F1);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f2", SDL_SCANCODE_F2);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f3", SDL_SCANCODE_F3);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f4", SDL_SCANCODE_F4);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f5", SDL_SCANCODE_F5);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f6", SDL_SCANCODE_F6);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f7", SDL_SCANCODE_F7);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f8", SDL_SCANCODE_F8);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f9", SDL_SCANCODE_F9);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f10", SDL_SCANCODE_F10);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f11", SDL_SCANCODE_F11);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, "f12", SDL_SCANCODE_F12);

    // Gamepad
    gamepadStringValuesMap = rbe_string_hash_map_create(30);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_a", SDL_CONTROLLER_BUTTON_A);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_b", SDL_CONTROLLER_BUTTON_B);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_x", SDL_CONTROLLER_BUTTON_X);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_y", SDL_CONTROLLER_BUTTON_Y);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_left", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_up", SDL_CONTROLLER_BUTTON_DPAD_UP);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_down", SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_start", SDL_CONTROLLER_BUTTON_START);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_back", SDL_CONTROLLER_BUTTON_BACK);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_shoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_shoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog", SDL_CONTROLLER_BUTTON_LEFTSTICK);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog", SDL_CONTROLLER_BUTTON_RIGHTSTICK);

    // Non Game Controller Button Action
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_trigger", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_trigger", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_left", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_right", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_up", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_down", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_left", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_right", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_up", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_trigger", SDL_CONTROLLER_BUTTON_INVALID);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_down", SDL_CONTROLLER_BUTTON_INVALID);

    // Initialize game pads
    input_load_gamepads();

    return true;
}

void rbe_input_finalize() {}

void rbe_input_add_action_value(const char* actionName, const char* actionValue, int deviceId) {
    if (!rbe_string_hash_map_has(inputActionMap, actionName)) {
        rbe_string_hash_map_add(inputActionMap, actionName, rbe_input_action_create_new_input_action(actionName, deviceId), sizeof(InputAction));
        inputActionNames[inputActionNamesCount] = actionName;
        inputActionNamesCount++;
    }
    InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
    // TODO: Remember to properly delete char* from rbe_strdup
    if (rbe_string_hash_map_has(keyboardStringValuesMap, actionValue)) {
        inputAction->keyboardValues[inputAction->keyboardValueCount] = rbe_string_hash_map_get_int(keyboardStringValuesMap, actionValue);
        inputAction->keyboardValueCount++;
        rbe_logger_debug("Added keyboard action | name: '%s', value: '%s', scancode: '%d'", actionName, actionValue, inputAction->keyboardValues[inputAction->keyboardValueCount - 1]);
    } else if (rbe_string_hash_map_has(gamepadStringValuesMap, actionValue)) {
        inputAction->gamepadValues[inputAction->gamepadValueCount] = rbe_strdup(actionValue);
        inputAction->gamepadValueCount++;
        rbe_logger_debug("Added gamepad value '%s'", actionValue);
    } else if (strcmp(actionValue, "mb_left") == 0 || strcmp(actionValue, "mb_right") == 0 || strcmp(actionValue, "mb_middle") == 0) {
        strcpy(&inputAction->mouseValues[inputAction->mouseValueCount], actionValue);
        rbe_logger_debug("Added mouse action | name: '%s', value: '%s'", actionName, inputAction->mouseValues[inputAction->mouseValueCount]);
        inputAction->mouseValueCount++;
    } else {
        rbe_logger_error("No valid value found for action | name: '%s', value: '%s'", actionName, actionValue);
    }
}

void rbe_input_remove_action_value(const char* actionName, const char* actionValue) {}

void rbe_input_remove_action(const char* actionName) {}

void rbe_input_process(SDL_Event event) {
    input_process_keyboard();
    input_process_mouse(event);
    input_process_gamepad(event);
}

void rbe_input_clean_up_flags() {
    // Just Pressed
    for (size_t i = 0; i < actionJustPressedClean.count; i++) {
        actionJustPressedClean.inputActions[i]->isActionJustPressed = false;
        actionJustPressedClean.inputActions[i] = NULL;
    }
    actionJustPressedClean.count = 0;
    // Just Released
    for (size_t i = 0; i < actionJustReleasedClean.count; i++) {
        actionJustReleasedClean.inputActions[i]->isActionJustReleased = false;
        actionJustReleasedClean.inputActions[i] = NULL;
    }
    actionJustReleasedClean.count = 0;
}

// Queries
bool rbe_input_is_action_pressed(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionPressed;
    }
    return false;
}

bool rbe_input_is_action_just_pressed(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustPressed;
    }
    return false;
}

bool rbe_input_is_action_just_released(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustReleased;
    }
    return false;
}

// TODO: Centralize looping through input actions names to prevent multiple loops
//--- Keyboard ---//
void input_process_keyboard() {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    for (size_t i = 0; i < inputActionNamesCount; i++) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
        for (size_t j = 0; j < inputAction->keyboardValueCount; j++) {
            SDL_Scancode scancode = (SDL_Scancode) inputAction->keyboardValues[j];
            bool isKeyboardValuePressed = keyboardState[scancode];
            if (isKeyboardValuePressed && !inputAction->isActionPressed) {
//                rbe_logger_debug("isKeyboardValuePressed = %d, scancode = %d", isKeyboardValuePressed, scancode);
                inputAction->isActionPressed = true;
                inputAction->isActionJustPressed = true;
                inputAction->lastScancodePressed = scancode;
                actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                break;
            } else if (!isKeyboardValuePressed && !inputAction->isActionJustReleased && inputAction->isActionPressed && (int) scancode == inputAction->lastScancodePressed) {
                inputAction->isActionPressed = false;
                inputAction->isActionJustPressed = false;
                inputAction->isActionJustReleased = true;
                actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
            }
        }
    }
}

//--- Mouse ---//
void input_process_mouse(SDL_Event event) {
    bool isMouseEvent = false;
    bool mousePressed = false;
    Uint8 mouseButton = 0;
    switch (event.type) {
    case SDL_MOUSEMOTION:
//            inputEvent.type = InputEventType::MOUSE;
//            inputEvent.mouseMotion = Vector2(event.motion.x, event.motion.y);
        break;
    case SDL_MOUSEWHEEL:
//            inputEvent.type = InputEventType::MOUSE;
        break;
    case SDL_MOUSEBUTTONDOWN:
        isMouseEvent = true;
        mousePressed = true;
        mouseButton = event.button.button;
        break;
    case SDL_MOUSEBUTTONUP:
        isMouseEvent = true;
        mousePressed = false;
        mouseButton = event.button.button;
        break;
    default:
        break;
    }
    if (isMouseEvent) {
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->mouseValueCount; j++) {
                bool isLeftMouseButton = strcmp(&inputAction->mouseValues[j], "mb_left") == 0 && mouseButton == SDL_BUTTON_LEFT;
                bool isRightMouseButton = strcmp(&inputAction->mouseValues[j], "mb_right") == 0 && mouseButton == SDL_BUTTON_RIGHT;
                if (isLeftMouseButton || isRightMouseButton) {
                    // Event yes
                    if (mousePressed) {
                        inputAction->isActionPressed = true;
                        inputAction->isActionJustPressed = true;
                        actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                    } else {
                        inputAction->isActionPressed = false;
                        inputAction->isActionJustPressed = false;
                        inputAction->isActionJustReleased = true;
                        actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
                    }
                    break;
                }
            }
        }
    }
}

//--- Gamepad ---//
#define RBE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS 15 // SDL_CONTROLLER_BUTTON_MAX

typedef struct GamepadInputButtonAction {
    bool isPressed;
    bool isJustPressed;
    bool isJustReleased;
} GamepadInputButtonAction;

GamepadInputButtonAction gamepadInputButtonActions[RBE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS];

SDL_Joystick* joystickController = NULL;
SDL_GameController* gameController = NULL;

// TODO: Make better (e.g. loading more than one controller and checking for connects/disconnects)...
void input_load_gamepads() {
    // Initialize gamepad input action array
    for (size_t i = 0; i < RBE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS; i++) {
        gamepadInputButtonActions[i].isPressed = false;
        gamepadInputButtonActions[i].isJustPressed = false;
        gamepadInputButtonActions[i].isJustReleased = false;
    }

    // Load gamepads
    int result = SDL_GameControllerAddMappingsFromFile("assets/resources/game_controller_db.txt");
    RBE_ASSERT_FMT(result >= 0, "Couldn't load sdl controller mapping file!");
    if (SDL_NumJoysticks() > 0) {
        joystickController = SDL_JoystickOpen(0);
        RBE_ASSERT_FMT(joystickController != NULL, "SDL 'joystick' object didn't load correctly!");
        gameController = SDL_GameControllerOpen(0);
        RBE_ASSERT_FMT(gameController != NULL, "SDL 'game controller' object didn't load correctly!");
        rbe_logger_debug("Loaded one gamepad!");
    } else {
        rbe_logger_debug("No gamepads detected.");
    }
}

void input_process_gamepad(SDL_Event event) {
    bool hasGamepadInput = false;
    switch (event.type) {
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: {
        hasGamepadInput = true;
        const int controllerId = event.jbutton.which;
        const bool isButtonPressed = event.jbutton.state == SDL_PRESSED;
        const uint8_t buttonValue = event.jbutton.button;
        if (controllerId == 0) {
            if (isButtonPressed) {
                // Button Pressed
//                ProcessButtonPress(inputEvent);
            } else {
                // Button Released
//                ProcessButtonRelease(inputEvent);
            }
        }
        break;
    }
    case SDL_JOYHATMOTION: {
        hasGamepadInput = true;
        const int controllerId = event.jhat.which;
        const uint8_t hat = event.jhat.hat;
        const uint8_t hatValue = event.jhat.value;
        if (controllerId == 0) {
//            ProcessJoyhatMotion(inputEvent);
        }
        break;
    }
    default:
        break;
    }
//    ProcessAxisMotion();

    if (hasGamepadInput) {
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            InputAction *inputAction = (InputAction *) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->gamepadValueCount; j++) {
                const char* gamepadValue = inputAction->gamepadValues[j];
                const uint8_t buttonScancode = rbe_string_hash_map_get_int(gamepadStringValuesMap, gamepadValue);
                if (false) {} // TODO: For breakpoint testing
            }
        }
    }
}
