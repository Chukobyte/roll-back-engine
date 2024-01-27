#include "json_helper.h"

#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

const char* json_get_string(cJSON* json, const char* key) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    SE_ASSERT_FMT(cJSON_IsString(stringJson) && (stringJson->valuestring != NULL), "Not able to load json string value for key '%s'", key);
    return stringJson->valuestring;
}

const char* json_get_string_default(cJSON* json, const char* key, const char* defaultValue) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(stringJson) && (stringJson->valuestring != NULL)) {
        return stringJson->valuestring;
    }
    return defaultValue;
}

char* json_get_string_new(cJSON* json, const char* key) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    SE_ASSERT_FMT(cJSON_IsString(stringJson) && (stringJson->valuestring != NULL), "Not able to load json string value for key '%s'", key);
    return se_strdup(stringJson->valuestring);
}

char* json_get_string_new_unchecked(cJSON* json, const char* key) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(stringJson) && stringJson->valuestring != NULL) {
        return se_strdup(stringJson->valuestring);
    }
    return NULL;
}

char* json_get_string_default_new(cJSON* json, const char* key, const char* defaultValue) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(stringJson) && (stringJson->valuestring != NULL)) {
        return se_strdup(stringJson->valuestring);
    }
    if (defaultValue == NULL) {
        return NULL;
    }
    return se_strdup(defaultValue);
}

int json_get_int(cJSON* json, const char* key) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    SE_ASSERT_FMT(cJSON_IsNumber(numJson), "Not able to load json int value for key '%s'", key);
    return (int) numJson->valuedouble;
}

int json_get_int_default(cJSON* json, const char* key, int defaultValue) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(numJson)) {
        return (int) numJson->valuedouble;
    }
    return defaultValue;
}

double json_get_double(cJSON* json, const char* key) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    SE_ASSERT_FMT(cJSON_IsNumber(numJson), "Not able to load json int value for key '%s'", key);
    return numJson->valuedouble;
}

double json_get_double_default(cJSON* json, const char* key, double defaultValue) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(numJson)) {
        return numJson->valuedouble;
    }
    return defaultValue;
}

bool json_get_bool(cJSON* json, const char* key) {
    cJSON* boolJson = cJSON_GetObjectItemCaseSensitive(json, key);
    SE_ASSERT_FMT(cJSON_IsBool(boolJson), "Not able to load json bool value for key '%s'", key);
    return cJSON_IsTrue(boolJson) ? true : false;
}

bool json_get_bool_default(cJSON* json, const char* key, bool defaultValue) {
    cJSON* boolJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsBool(boolJson)) {
        return cJSON_IsTrue(boolJson) ? true : false;
    }
    return defaultValue;
}

SKAVector2 json_get_vec2(cJSON* json, const char* key) {
    cJSON* vec2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    return (SKAVector2) {
        .x = (float) json_get_double(vec2Json, "x"),
        .y = (float) json_get_double(vec2Json, "y")
    };
}

SKAVector2 json_get_vec2_default(cJSON* json, const char* key, SKAVector2 defaultValue) {
    cJSON* vec2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    if (vec2Json != NULL) {
        return (SKAVector2) {
            .x = (float) json_get_double_default(vec2Json, "x", defaultValue.x),
            .y = (float) json_get_double_default(vec2Json, "y", defaultValue.y)
        };
    }
    return defaultValue;
}

SKASize2D json_get_size2d(cJSON* json, const char* key) {
    cJSON* size2dJson = cJSON_GetObjectItemCaseSensitive(json, key);
    return (SKASize2D) {
        .w = (float) json_get_double(size2dJson, "w"),
        .h = (float) json_get_double(size2dJson, "h")
    };
}

SKASize2D json_get_size2d_default(cJSON* json, const char* key, SKASize2D defaultValue) {
    cJSON* size2dJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (size2dJson != NULL) {
        return (SKASize2D) {
            .w = (float) json_get_double(size2dJson, "w"),
            .h = (float) json_get_double(size2dJson, "h")
        };
    }
    return defaultValue;
}

SKARect2 json_get_rect2(cJSON* json, const char* key) {
    cJSON* rect2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    return (SKARect2) {
        .x = (float) json_get_double(rect2Json, "x"),
        .y = (float) json_get_double(rect2Json, "y"),
        .w = (float) json_get_double(rect2Json, "w"),
        .h = (float) json_get_double(rect2Json, "h")
    };
}

SKARect2 json_get_rect2_default(cJSON* json, const char* key, SKARect2 defaultValue) {
    cJSON* rect2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    if (rect2Json != NULL) {
        return (SKARect2) {
            .x = (float) json_get_double(rect2Json, "x"),
            .y = (float) json_get_double(rect2Json, "y"),
            .w = (float) json_get_double(rect2Json, "w"),
            .h = (float) json_get_double(rect2Json, "h")
        };
    }
    return defaultValue;
}

SKAColor json_get_linear_color(cJSON* json, const char* key) {
    cJSON* colorJson = cJSON_GetObjectItemCaseSensitive(json, key);
    return (SKAColor) {
        .r = ((float) json_get_int(colorJson, "r") / 255.0f),
        .g = ((float) json_get_int(colorJson, "g") / 255.0f),
        .b = ((float) json_get_int(colorJson, "b") / 255.0f),
        .a = ((float) json_get_int(colorJson, "a") / 255.0f)
    };
}

SKAColor json_get_linear_color_default(cJSON* json, const char* key, SKAColor defaultValue) {
    cJSON* colorJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (colorJson != NULL) {
        return (SKAColor) {
            .r = ((float) json_get_int_default(colorJson, "r", 255) / 255.0f),
            .g = ((float) json_get_int_default(colorJson, "g", 255) / 255.0f),
            .b = ((float) json_get_int_default(colorJson, "b", 255) / 255.0f),
            .a = ((float) json_get_int_default(colorJson, "a", 255) / 255.0f)
        };
    }
    return defaultValue;
}

SKAMinMaxVec2 json_get_minmax_vec2(cJSON* json, const char* key) {
    cJSON* minmaxJson = cJSON_GetObjectItemCaseSensitive(json, key);
    const SKAVector2 valueMin = json_get_vec2(minmaxJson, "min");
    const SKAVector2 valueMax = json_get_vec2(minmaxJson, "max");
    return (SKAMinMaxVec2) { .min = valueMin, .max = valueMax };
}

SKAMinMaxVec2 json_get_minmax_vec2_default(cJSON* json, const char* key, SKAMinMaxVec2 minmax) {
    cJSON* minmaxJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (minmaxJson) {
        const SKAVector2 valueMin = json_get_vec2(minmaxJson, "min");
        const SKAVector2 valueMax = json_get_vec2(minmaxJson, "max");
        return (SKAMinMaxVec2) { .min = valueMin, .max = valueMax };
    }
    return minmax;
}
