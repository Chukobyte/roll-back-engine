#pragma once

#include <string>
#include <vector>

#include <seika/logger.h>

#define EDITOR_SETTINGS_FILE_NAME "editor_settings.json"

struct RecentlyLoadedProjectData {
    std::string name;
    std::string fullPath;
};

class EditorSettings {
  public:
    bool Load();
    void Save() const;
    void AddToRecentlyLoadedProjectsList(const std::string& name, const std::string& path);
    void SetEditorLogLevel(SELogLevel level);
    void SetEditorLogLevel(const std::string& level);
    void SetGameLogLevel(SELogLevel level);
    void SetGameLogLevel(const std::string& level);
    [[nodiscard]] SELogLevel GetEditorLogLevel() const {
        return editorLogLevel;
    }
    [[nodiscard]] std::string GetEditorLogLevelString() const {
        return se_logger_get_log_level_string(editorLogLevel);
    }
    [[nodiscard]] SELogLevel GetGameLogLevel() const {
        return gameLogLevel;
    }
    [[nodiscard]] std::string GetGameLogLevelString() const {
        return se_logger_get_log_level_string(gameLogLevel);
    }

    size_t recentlyLoadedProjectsLimit = 5;
    std::vector<RecentlyLoadedProjectData> recentlyLoadedProjects;

  private:
    SELogLevel editorLogLevel = SkaLogLevel_ERROR;
    SELogLevel gameLogLevel = SkaLogLevel_ERROR;
};
