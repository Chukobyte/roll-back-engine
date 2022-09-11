#include "asset_browser.h"

#include "editor_context.h"
#include "ui/imgui/imgui_helper.h"
#include "utils/file_system_helper.h"
#include "../../../engine/src/core/utils/logger.h"

using namespace Squid;

//--- FileNode ---//
std::string FileNode::GetRelativePath() const {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
    std::filesystem::path relativePath = std::filesystem::relative(path, projectRootDir);
    return relativePath.generic_string();
}

//--- FileNodeUtils ---//
void FileNodeUtils::LoadFileNodeDirEntries(FileNode& fileNode, unsigned int& nodeIndex, std::unordered_map<std::string, std::vector<FileNode>>& extensionToFileNodeMap) {
    for (auto const& dir_entry : std::filesystem::directory_iterator{fileNode.path}) {
//        rbe_logger_debug("dir entry path: '%s'", dir_entry.path().string().c_str());
//        rbe_logger_debug("dir entry relative path: '%s'", std::filesystem::relative(dir_entry.path(), fileNode.path).string().c_str());
        const std::string& fileName = dir_entry.path().filename().string();
        if (fileName == "__pycache__" || fileName[0] == '.') {
            continue;
        }
        if (std::filesystem::is_directory(dir_entry)) {
            FileNode dirNode = { dir_entry.path(), FileNodeType::Directory, nodeIndex++ };
            LoadFileNodeDirEntries(dirNode, nodeIndex, extensionToFileNodeMap);
            fileNode.directories.emplace_back(dirNode);
        } else if (std::filesystem::is_regular_file(dir_entry)) {
            FileNode regularFileNode = { dir_entry.path(), FileNodeType::File, nodeIndex++, GetFileNodeRegularType(dir_entry.path().filename().string()) };
            fileNode.files.emplace_back(regularFileNode);
            const std::string extension = regularFileNode.path.extension().string();
            if (extensionToFileNodeMap.count(extension) <= 0) {
                extensionToFileNodeMap.emplace(extension, std::vector<FileNode> {});
            }
            extensionToFileNodeMap[extension].emplace_back(regularFileNode);
        }
    }
}

FileNodeRegularFileType FileNodeUtils::GetFileNodeRegularType(const std::string& fileName) {
    if (fileName.ends_with(".png")) {
        return FileNodeRegularFileType::Texture;
    } else if (fileName.ends_with(".wav")) {
        return FileNodeRegularFileType::AudioSource;
    }
//    else if (fileName.ends_with(".py")) {
//        return FileNodeRegularFileType::PythonScript;
//    }
    return FileNodeRegularFileType::Invalid;
}

void FileNodeUtils::DisplayFileNodeTree(FileNode &fileNode, const bool isRoot) {
    static AssetBrowser* assetBrowser = AssetBrowser::Get();
    const ImGuiTreeNodeFlags dirFlags = isRoot ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
    ImGuiTreeNodeFlags defaultFlags = fileNode.type == FileNodeType::Directory ? dirFlags : ImGuiTreeNodeFlags_Leaf;
    if (assetBrowser->selectedFileNode.has_value() && assetBrowser->selectedFileNode->index == fileNode.index) {
        defaultFlags |= ImGuiTreeNodeFlags_Selected;
    }
    ImGuiHelper::TreeNode treeNode = {
        .label = isRoot ? "res://" : fileNode.path.filename().string(),
        .flags = defaultFlags,
        .callbackFunc = [fileNode, isRoot] (ImGuiHelper::Context* context) {
            // Left Click
            if (ImGui::IsItemClicked()) {
                assetBrowser->selectedFileNode = fileNode;
            }

            // Right Click
            const std::string fileNodePopupId = std::to_string(fileNode.index) + "_file_popup";
            ImGui::OpenPopupOnItemClick(fileNodePopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup(fileNodePopupId.c_str())) {
                assetBrowser->selectedFileNode = fileNode;
                if (fileNode.type == FileNodeType::Directory && ImGui::MenuItem("Create Directory")) {
                }

                if (!isRoot) {
                    if (ImGui::MenuItem("Rename")) {
                        static ImGuiHelper::PopupModal renameFilePopup = {
                            .name = "Rename File",
                            .open = nullptr,
                            .windowFlags = 0,
                            .position = ImVec2{ 100.0f, 100.0f },
                            .size = ImVec2{ 250.0f, 100.0f },
                        };
                        renameFilePopup.callbackFunc = [fileNode] (ImGuiHelper::Context* context) {
                            static std::string newFileName;
                            ImGuiHelper::InputText newFileNameText("File Name", newFileName);
                            ImGuiHelper::BeginInputText(newFileNameText);
                            if (ImGui::Button("Close")) {
                                newFileName.clear();
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Ok") && !newFileName.empty()) {
                                assetBrowser->RenameFile(fileNode.path, newFileName);
                                newFileName.clear();
                                ImGui::CloseCurrentPopup();
                            }
                        };
                        ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&renameFilePopup);
                    }

                    if (ImGui::MenuItem("Delete")) {
                        assetBrowser->DeleteFile(fileNode.path);
                    }
                }

                ImGui::EndPopup();
            }

            // Files
            for (FileNode dirFileNode : fileNode.directories) {
                DisplayFileNodeTree(dirFileNode);
            }
            // Directories
            for (FileNode regularFileNode : fileNode.files) {
                DisplayFileNodeTree(regularFileNode);
            }
        }
    };
    ImGuiHelper::BeginTreeNode(treeNode);
}

//--- AssetBrowser ---//
Task<> AssetBrowser::UpdateFileSystemCache() {
    RefreshCache();
    while (true) {
//        co_await WaitSeconds(10.0f, EditorContext::Time);
        co_await WaitUntil([this] { return refreshCacheQueued; });
        RefreshCache();
        refreshCacheQueued = false;
    }
}

void AssetBrowser::RefreshCache() {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
//    rbe_logger_debug("root name: %s", projectRootDir.root_name().c_str());
//    rbe_logger_debug("root path: %s", projectRootDir.string().c_str());

    rootNode.path = projectRootDir;
    rootNode.type = FileNodeType::Directory;
    rootNode.directories.clear();
    rootNode.files.clear();
    extensionToFileNodeMap.clear();
    unsigned int startingIndex = rootNode.index + 1;
    if (!selectedFileNode.has_value()) {
        selectedFileNode = rootNode;
    }
    FileNodeUtils::LoadFileNodeDirEntries(rootNode, startingIndex, extensionToFileNodeMap);
    for (auto& func : registerRefreshFuncs) {
        func(rootNode);
    }
}

void AssetBrowser::QueueRefreshCache() {
    refreshCacheQueued = true;
}

void AssetBrowser::RenameFile(const std::filesystem::path& oldPath, const std::string& newName) {
    const std::filesystem::path parentPath = oldPath.parent_path();
    const std::filesystem::path newFilePath = parentPath / newName;
    rbe_logger_debug("old file path = %s, new file path = %s", oldPath.string().c_str(), newFilePath.string().c_str());
    std::error_code ec;
    std::filesystem::rename(oldPath, newFilePath, ec);
    if (ec.value() != 0) {
        rbe_logger_error("ec value = %d, message = %s", ec.value(), ec.message().c_str());
    }

    QueueRefreshCache();
}

// TODO: Add more validation checks
void AssetBrowser::DeleteFile(const std::filesystem::path& path) {
    std::error_code ec;
    if (std::filesystem::is_directory(path)) {
        std::uintmax_t numberDeleted = std::filesystem::remove_all(path, ec);
        rbe_logger_debug("Number of files deleted from path '%s' = '%zu'", path.string().c_str(), numberDeleted);
    } else {
        std::filesystem::remove(path, ec);
    }
    if (ec.value() != 0) {
        rbe_logger_error("Error deleting from path '%s'!\nec value = %d, message = %s",
                         path.string().c_str(), ec.value(), ec.message().c_str());
    }
    QueueRefreshCache();
}

void AssetBrowser::RunFuncOnAllNodeFiles(FileNode &node, std::function<bool(FileNode &)> func) {
    if (!func(node)) {
        for (auto& dir : node.files) {
            if (func(dir)) {
                break;
            }
        }
    }
}

void AssetBrowser::RunFuncOnAllNodeDirs(FileNode& node, std::function<bool(FileNode &)> func) {
    if (!func(node)) {
        for (auto& dir : node.directories) {
            if (func(dir)) {
                break;
            }
        }
    }
}

void AssetBrowser::RegisterRefreshCallback(const AssetBrowserRefreshFunc& func) {
    registerRefreshFuncs.emplace_back(func);
}
