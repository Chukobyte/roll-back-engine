#pragma once

#include "utils/singleton.h"

#include <vector>
#include <filesystem>

#include "SquidTasks/Task.h"

// File Node

// A wrapper around filesystem path
enum class FileNodeType {
    Invalid,
    File,
    Directory,
};

enum class FileNodeRegularFileType {
    Invalid,
    Texture,
    AudioSource,
//    PythonScript,
};

struct FileNode {
    std::string GetRelativePath() const;

    std::filesystem::path path;
    FileNodeType type = FileNodeType::Invalid;
    unsigned int index = 0;
    FileNodeRegularFileType regularFileType = FileNodeRegularFileType::Invalid;
    std::vector<FileNode> directories;
    std::vector<FileNode> files;
};

namespace FileNodeUtils {
FileNodeRegularFileType GetFileNodeRegularType(const std::string& fileName);
void LoadFileNodeDirEntries(FileNode& fileNode, unsigned int& nodeIndex);
void DisplayFileNodeTree(FileNode& fileNode, bool isRoot = false);
} // namespace FileNodeUtils

// Asset Browser

using AssetBrowserRefreshFunc = std::function<void(const FileNode& rootNode)>;

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}

    Task<> UpdateFileSystemCache();

    void RegisterRefreshCallback(const AssetBrowserRefreshFunc& func);

    FileNode rootNode;
    std::optional<FileNode> selectedFileNode;

  private:
    void RefreshCache();

    // For now assumes one time subscribe only with no unsubscriptions
    std::vector<AssetBrowserRefreshFunc> registerRefreshFuncs;
};
