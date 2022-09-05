#include "project_properties.h"

#include "../engine/src/core/scripting/python/rbe_py_file_loader.h"
#include "../engine/src/core/utils/logger.h"

//--- Project Assets ---//
void ProjectAssets::SetAssets(RBEGameProperties* gameProperties) {
    // TODO: Only load fonts as texture and audio sources will be generated by the asset browser file cache lookup
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        textures.emplace_back(TextureAsset(gameProperties->textures[i]));
    }
    for (size_t i = 0; i < gameProperties->audioSourceCount; i++) {
        audioSources.emplace_back(AudioSourceAsset(gameProperties->audioSources[i]));
    }
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        fonts.emplace_back(FontAsset(gameProperties->fonts[i]));
    }
}

//--- Project Inputs ---//
void ProjectInputs::SetInputs(RBEGameProperties *gameProperties) {
    for (size_t i = 0; i < gameProperties->inputActionCount; i++) {
        const RBEInputAction& propertyInputAction = gameProperties->inputActions[i];
        ProjectInputAction inputAction = {
            .name = std::string(propertyInputAction.name),
            .deviceId = propertyInputAction.deviceId
        };
        for (size_t j = 0; j < propertyInputAction.valueCount; j++) {
            inputAction.values.emplace_back(propertyInputAction.values[j]);
        }
        actions.emplace_back(inputAction);
    }
}

//--- Project Properties ---//
ProjectProperties::ProjectProperties(singleton) {
    rbe_game_props_initialize(false);
}

ProjectProperties::~ProjectProperties() {
    rbe_game_props_finalize();
}

void ProjectProperties::LoadPropertiesFromConfig(const char* modulePath) {
    rbe_logger_debug("Loading game properties");
    rbe_game_props_finalize();
    rbe_game_props_initialize(false);
    RBEGameProperties* gameProps = rbe_py_load_game_properties(modulePath);
    gameTitle = std::string(gameProps->gameTitle);
    initialNodePath = std::string(gameProps->initialScenePath);
    windowWidth = gameProps->windowWidth;
    windowHeight = gameProps->windowHeight;
    resolutionWidth = gameProps->resolutionWidth;
    resolutionHeight = gameProps->resolutionHeight;
    targetFPS = gameProps->targetFPS;
    areCollidersVisible = gameProps->areCollidersVisible;
    assets.SetAssets(gameProps);
    inputs.SetInputs(gameProps);
    rbe_logger_debug("Loading game properties finished");
}

void ProjectProperties::PrintProperties() const {
    rbe_logger_debug("game_title = %s, window_width = %d, window_height = %d, target_fps = %d",
                     gameTitle.c_str(), windowWidth, windowHeight, targetFPS);
}

void ProjectProperties::UpdateTextureAsset(const TextureAsset &textureAsset) {
    for (auto& asset : assets.textures) {
        if (asset.file_path == textureAsset.file_path) {
            asset = textureAsset;
            break;
        }
    }
}

void ProjectProperties::UpdateAudioSourceAsset(const AudioSourceAsset &audioSourceAsset) {
    for (auto& asset : assets.audioSources) {
        if (asset.file_path == audioSourceAsset.file_path) {
            asset = audioSourceAsset;
            break;
        }
    }
}

TextureAsset& ProjectProperties::GetTextureAsset(const std::string &texturePath) {
    for (auto& asset : assets.textures) {
        if (texturePath == asset.file_path) {
            return asset;
        }
    }
    rbe_logger_error("Couldn't find texture at path '%s'", texturePath.c_str());
    static TextureAsset invalid("invalid", "invalid", "invalid", "invalid", "invalid");
    return invalid;
}
