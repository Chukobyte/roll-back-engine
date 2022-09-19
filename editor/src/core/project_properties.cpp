#include "project_properties.h"

#include "../engine/src/core/scripting/python/rbe_py_file_loader.h"
#include "../engine/src/core/utils/logger.h"

#include "asset_browser.h"

//--- Project Assets ---//
void ProjectAssets::SetAssets(RBEGameProperties* gameProperties) {
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
namespace {
void CreateDefaultAssetIfNonExisting(const FileNode& dirNode) {
    static ProjectProperties* projectProperties = ProjectProperties::Get();
    // Create default asset if it doesn't exist
    for (const auto& fileNode : dirNode.files) {
        const std::string fileName = fileNode.path.filename().string();
        if (fileName.ends_with(".png") && !projectProperties->HasTextureWithPath(fileNode.GetRelativePath())) {
            projectProperties->assets.textures.emplace_back(TextureAsset(fileNode.GetRelativePath()));
        } else if (fileName.ends_with(".wav") && !projectProperties->HasAudioSourceWithPath(fileNode.GetRelativePath())) {
            projectProperties->assets.audioSources.emplace_back(AudioSourceAsset(fileNode.GetRelativePath()));
        }
    }

    // Recurse on all dirs
    for (const auto& childDirNode : dirNode.directories) {
        CreateDefaultAssetIfNonExisting(childDirNode);
    }
}
} // namespace

ProjectProperties::ProjectProperties(singleton) {
    rbe_game_props_initialize(false);

    // Register project properties with asset browser refresh cache
    AssetBrowser* assetBrowser = AssetBrowser::Get();
    assetBrowser->RegisterRefreshCallback([this](const FileNode& rootNode) {
        CreateDefaultAssetIfNonExisting(rootNode);
    });
}

void ProjectProperties::ResetToDefault() {
    gameTitle.clear();
    initialNodePath.clear();
    windowWidth = 800;
    windowHeight = 600;
    resolutionWidth = 800;
    resolutionHeight = 600;
    targetFPS = 66;
    areCollidersVisible = false;
    assets.textures.clear();
    assets.audioSources.clear();
    assets.fonts.clear();
    inputs.actions.clear();
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

bool ProjectProperties::HasTextureWithPath(const std::string &path) const {
    for (const auto& texture : assets.textures) {
        if (texture.file_path == path) {
            return true;
        }
    }
    return false;
}

bool ProjectProperties::HasAudioSourceWithPath(const std::string &path) const {
    for (const auto& audioSource : assets.audioSources) {
        if (audioSource.file_path == path) {
            return true;
        }
    }
    return false;
}

// Default project property file
std::string ProjectProperties::GetDefaultProjectPropertyFileContent(const std::string& gameTitle) {
    return "from crescent_api import *\n"
           "\n"
           "configure_game(\n"
           "        game_title=\"" + gameTitle + "\",\n"
           "        window_width=800,\n"
           "        window_height=600,\n"
           "        resolution_width=800,\n"
           "        resolution_height=600,\n"
           "        target_fps=66,\n"
           "        initial_node_path=\"\",\n"
           "        colliders_visible=False,\n"
           ")\n"
           "\n"
           "configure_assets(\n"
           "        audio_sources=[],\n"
           "        textures=[],\n"
           "        fonts=[],\n"
           ")\n"
           "\n"
           "configure_inputs(\n"
           "        input_actions=[]\n"
           ")\n";
}

std::string ProjectProperties::GetPathRelativeToProjectPath(const std::string& path) {
    std::filesystem::path relativePath = std::filesystem::relative(path, projectPath);
    return relativePath.generic_string();
}
