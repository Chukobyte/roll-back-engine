#include "details_ui.h"
#include "../../../../asset_browser.h"
#include "../../../../components/component.h"

ImGuiHelper::PopupModal& OpenedProjectUI::Windows::AnimationEditor::GetPopup(AnimatedSpriteComp* animatedSpriteComp) {
    static ImGuiHelper::PopupModal animationsEditPopup = {
        .name = "Animation Edit Menu",
        .open = nullptr,
        .windowFlags = 0,
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 400.0f, 400.0f },
    };
    static int selectedAnimIndex = 0;
    animationsEditPopup.callbackFunc = [animatedSpriteComp] (ImGuiHelper::Context* context) {
        static std::string selectedAnimName;
        static int selectedAnimFrameIndex = 0;
        static auto CleanupAnimEditState = [] {
            selectedAnimIndex = 0;
            selectedAnimFrameIndex = 0;
            selectedAnimName.clear();
        };
        if (ImGui::Button("Close")) {
            CleanupAnimEditState();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Ok")) {
            CleanupAnimEditState();
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();

        // Animation List
        ImGui::Text("Animations:");
        if (ImGui::Button("+")) {
            animatedSpriteComp->AddDefaultAnimation();
        }
        const ImGuiTreeNodeFlags defaultFlags = ImGuiTreeNodeFlags_Leaf;
        int animIndex = 0;
        struct QueuedAnimToRemove {
            std::string name;
            AnimatedSpriteComp* animSpriteComp = nullptr;
        };
        QueuedAnimToRemove queuedAnimToRemove;
        for (auto& anim : animatedSpriteComp->animations) {
            ImGuiHelper::TreeNode treeNode = {
                .label = anim.name,
                .flags = animIndex == selectedAnimIndex ? defaultFlags | ImGuiTreeNodeFlags_Selected : defaultFlags,
                .callbackFunc = [animatedSpriteComp, &anim, &animIndex, &queuedAnimToRemove] (ImGuiHelper::Context* context) {
                    if (animIndex == selectedAnimIndex) {
                        selectedAnimName = anim.name;
                    }
                    // Left Click
                    if (ImGui::IsItemClicked()) {
                        selectedAnimIndex = animIndex;
                        selectedAnimName = anim.name;
                    }

                    // Right Click
                    const std::string animPopupId = anim.name + "_popup";
                    ImGui::OpenPopupOnItemClick(animPopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
                    static ImGuiHelper::PopupModal renameAnimPopup = {
                        .name = "Rename Animation Menu",
                        .open = nullptr,
                        .windowFlags = 0,
                        .position = ImVec2{ 200.0f, 100.0f },
                        .size = ImVec2{ 200.0f, 200.0f },
                    };
                    bool shouldRenameAnim = false;
                    if (ImGui::BeginPopup(animPopupId.c_str())) {
                        selectedAnimIndex = animIndex;
                        selectedAnimName = anim.name;
                        if (ImGui::MenuItem("Rename")) {
                            shouldRenameAnim = true;
                            renameAnimPopup.callbackFunc = [animatedSpriteComp, &anim] (ImGuiHelper::Context* context) {
                                static std::string newNameText;
                                ImGuiHelper::InputText newNameInputText("New Name", newNameText);
                                ImGuiHelper::BeginInputText(newNameInputText);

                                if (ImGui::Button("Close")) {
                                    newNameText.clear();
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Ok")) {
                                    if (!newNameText.empty() && !animatedSpriteComp->HasAnimationWithName(newNameText)) {
                                        anim.name = newNameText;
                                        selectedAnimName = anim.name;
                                    }
                                    newNameText.clear();
                                    ImGui::CloseCurrentPopup();
                                }
                            };
                            // TODO: Should create a nested popup implementation
//                                    ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&renameAnimPopup);
                        }


                        if (ImGui::MenuItem("Delete")) {
                            // Queue deletion
                            selectedAnimIndex--;
                            selectedAnimName.clear();
                            queuedAnimToRemove.name = anim.name;
                            queuedAnimToRemove.animSpriteComp = animatedSpriteComp;
                        }
                        ImGui::EndPopup();
                    }
                    if (shouldRenameAnim) {
                        ImGui::OpenPopup(renameAnimPopup.name);
                    }
                    ImGuiHelper::BeginPopupModal(renameAnimPopup);
                }
            };
            ImGuiHelper::BeginTreeNode(treeNode);
            animIndex++;
        }
        ImGui::Separator();

        // If there is a selected anim
        if (!selectedAnimName.empty() && animatedSpriteComp->HasAnimationWithName(selectedAnimName)) {
            auto& selectedAnim = animatedSpriteComp->GetAnimationByName(selectedAnimName);
            const size_t frameCount = selectedAnim.animationFrames.size();
            ImGui::Text("Frame Count: %zu", frameCount);

            // Have to define anim frame stuff here in order to refresh combo box when adding frame
            static std::vector<std::string> animFrameTexturePathList = {ImGuiHelper::COMBO_BOX_LIST_NONE };
            static AssetBrowser* assetBrowser = AssetBrowser::Get();
            static auto UpdateTexturePathList = [] {
                animFrameTexturePathList.clear();
                animFrameTexturePathList.emplace_back(ImGuiHelper::COMBO_BOX_LIST_NONE);
                if (assetBrowser->fileCache.extensionToFileNodeMap.count(".png") > 0) {
                    for (auto& fileNode : assetBrowser->fileCache.extensionToFileNodeMap[".png"]) {
                        animFrameTexturePathList.emplace_back(fileNode.GetRelativePath());
                    }
                }
            };
            static FuncObject initializeAnimFrameTexturePathListFunc = FuncObject([] {
                UpdateTexturePathList();
                assetBrowser->RegisterRefreshCallback([](const FileNode& rootNode) {
                    UpdateTexturePathList();
                });
            });
            static ImGuiHelper::ComboBox animFrameTexturePathComboBox("Texture Path", animFrameTexturePathList);

            if (ImGui::Button("Add Frame")) {
                EditorAnimationFrame newAnimFrame;
                newAnimFrame.frame = frameCount;
                selectedAnim.animationFrames.emplace_back(newAnimFrame);
                selectedAnimFrameIndex = newAnimFrame.frame;
                animFrameTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE, false);
            }

            int animFrameToDelete = -1;
            if (frameCount > 0) {
                ImGui::SameLine();
                if (ImGui::Button("Delete Frame")) {
                    animFrameToDelete = selectedAnimFrameIndex;
                    selectedAnimFrameIndex = Helper::Max(selectedAnimFrameIndex - 1, 0);
                }

                auto& selectedAnimFrame = selectedAnim.GetAnimationFrame(selectedAnimFrameIndex);

                // TODO: Wrap asset browser file path lists in a UI Widget
                // Anim Frame Texture Path
                static FuncObject initializeFunc2 = FuncObject([selectedAnimFrame] {
                    if (selectedAnimFrame.texturePath.empty()) {
                        animFrameTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
                    } else {
                        animFrameTexturePathComboBox.SetSelected(selectedAnimFrame.texturePath);
                    }
                });
                animFrameTexturePathComboBox.onSelectionChangeCallback = [animatedSpriteComp, animName = selectedAnim.name, animFrameIndex = selectedAnimFrame.frame](const char* newItem) {
                    if (animatedSpriteComp->HasAnimationWithName(animName)) {
                        auto& anim = animatedSpriteComp->GetAnimationByName(animName);
                        if (anim.HasAnimationFrame(animFrameIndex)) {
                            auto& animFrame = anim.GetAnimationFrame(animFrameIndex);
                            animFrame.texturePath = newItem;
                            if (animFrame.texturePath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                                animFrame.texturePath.clear();
                            }
                        }
                    }
                };
                ImGuiHelper::BeginComboBox(animFrameTexturePathComboBox);

                // Draw Source
                ImGuiHelper::DragFloat4 frameDrawSourceDragFloat4("Draw Source", (float*) &selectedAnimFrame.drawSource);
                ImGuiHelper::BeginDragFloat4(frameDrawSourceDragFloat4);

                // Selection Arrows
                const int beforeArrowsAnimFrame = selectedAnimFrameIndex;
                if (ImGui::Button("<--")) {
                    selectedAnimFrameIndex = Helper::Max(selectedAnimFrameIndex - 1, 0);
                }
                ImGui::SameLine();
                ImGui::Text("Current Frame: %d", selectedAnimFrameIndex);
                ImGui::SameLine();
                if (ImGui::Button("-->")) {
                    selectedAnimFrameIndex = Helper::Min(selectedAnimFrameIndex + 1, (int) frameCount - 1);
                }
                if (beforeArrowsAnimFrame != selectedAnimFrameIndex) {
                    const auto& newSelectedAnimFrame = selectedAnim.GetAnimationFrame(selectedAnimFrameIndex);
                    const std::string newSelectedAnimFrameTexturePath = !newSelectedAnimFrame.texturePath.empty() ? newSelectedAnimFrame.texturePath : ImGuiHelper::COMBO_BOX_LIST_NONE;
                    // Null callback since it's not needed and will be reset next frame
                    animFrameTexturePathComboBox.onSelectionChangeCallback = nullptr;
                    animFrameTexturePathComboBox.SetSelected(newSelectedAnimFrameTexturePath);
                }
            }
            if (animFrameToDelete >= 0) {
                selectedAnim.RemoveAnimatationFrameByIndex(animFrameToDelete);
            }

            ImGui::Separator();

            ImGuiHelper::DragInt speedDragInt("Speed", selectedAnim.speed);
            ImGuiHelper::BeginDragInt(speedDragInt);

            ImGuiHelper::CheckBox loopsCheckBox("Loops", selectedAnim.doesLoop);
            ImGuiHelper::BeginCheckBox(loopsCheckBox);
        }

        // Cleanup
        if (!queuedAnimToRemove.name.empty() && queuedAnimToRemove.animSpriteComp != nullptr) {
            queuedAnimToRemove.animSpriteComp->RemoveAnimationByName(queuedAnimToRemove.name);
        }
    };

    return animationsEditPopup;
}
