#include "asset_manager.h"
#include "asset_browser.h"

static AssetBrowserRefreshSubscriberHandle subscriberHandle = 0;
static bool isInitialized = false;

namespace {
void LoadAllTextures(AssetManager* assetManager, const std::vector<FileNode>& fileNodes) {
    for (const FileNode& textureFile : fileNodes) {
        const std::string relativePath = textureFile.GetRelativePath();
        if (!assetManager->HasTexture(relativePath.c_str())) {
            assetManager->LoadTexture(textureFile.path.string().c_str(), relativePath.c_str());
        }
    }
}

void LoadAllAudioSources(AssetManager* assetManager, const std::vector<FileNode>& fileNodes) {
    for (const FileNode& audioSourceFile : fileNodes) {
        const std::string relativePath = audioSourceFile.GetRelativePath();
        if (!assetManager->HasAudioSource(relativePath.c_str())) {
            assetManager->LoadAudioSource(audioSourceFile.path.string().c_str(), relativePath.c_str());
        }
    }
}
} // namespace

void AssetManager::Initialize() {
    if (!isInitialized) {
        rbe_asset_manager_initialize();
        AssetBrowser* assetBrowser = AssetBrowser::Get();
        subscriberHandle = assetBrowser->RegisterRefreshCallback([this, assetBrowser](const FileNode& rootNode) {
            // Textures
            if (assetBrowser->fileCache.HasFilesWithExtension(".png")) {
                LoadAllTextures(this, assetBrowser->fileCache.GetFilesWithExtension(".png"));
            }
            // Audio Sources
            if (assetBrowser->fileCache.HasFilesWithExtension(".wav")) {
                LoadAllAudioSources(this, assetBrowser->fileCache.GetFilesWithExtension(".wav"));
            }
        });
        isInitialized = true;
    }
}

void AssetManager::Finalize() {
    if (isInitialized) {
        rbe_asset_manager_finalize();
        AssetBrowser::Get()->UnregisterRefreshCallback(subscriberHandle);
        isInitialized = false;
    }
}

// Texture
Texture* AssetManager::LoadTexture(const char* fileName, const char* key) {
    return rbe_asset_manager_load_texture(fileName, key);
}

Texture* AssetManager::GetTexture(const char* key) {
    return rbe_asset_manager_get_texture(key);
}

bool AssetManager::HasTexture(const char* key) const {
    return rbe_asset_manager_has_texture(key);
}

// Font
Font* AssetManager::LoadFont(const char* fileName, const char* key, int size) {
    return rbe_asset_manager_load_font(fileName, key, size);
}

Font* AssetManager::GetFont(const char* key) {
    return rbe_asset_manager_get_font(key);
}

bool AssetManager::HasFont(const char* key) const {
    return rbe_asset_manager_has_font(key);
}

// Audio Source
RBEAudioSource* AssetManager::LoadAudioSource(const char* fileName, const char* key) {
    return rbe_asset_manager_load_audio_source_wav(fileName, key);
}

RBEAudioSource* AssetManager::GetAudioSource(const char* key) {
    return rbe_asset_manager_get_audio_source(key);
}

bool AssetManager::HasAudioSource(const char* key) const {
    return rbe_asset_manager_has_audio_source(key);
}
