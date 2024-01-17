#pragma once

#include <string>
#include <algorithm>
#include <utility>

#include <seika/utils/logger.h>
#include <seika/utils/se_assert.h>

#include "../engine/src/core/ecs/component/animated_sprite_component.h"
#include "../engine/src/core/ecs/component/collider2d_component.h"
#include "../engine/src/core/ecs/component/color_rect_component.h"
#include "../engine/src/core/ecs/component/parallax_component.h"
#include "../engine/src/core/ecs/component/script_component.h"
#include "../engine/src/core/ecs/component/sprite_component.h"
#include "../engine/src/core/ecs/component/text_label_component.h"
#include "../engine/src/core/ecs/component/transform2d_component.h"
#include "../engine/src/core/ecs/component/particles2d_component.h"

struct EditorComponent {};

struct Transform2DComp : public EditorComponent {
    Transform2DComp() = default;

    explicit Transform2DComp(const Transform2DComponent* transform2DComponent)
        : transform2D(transform2DComponent->localTransform),
          zIndex(transform2DComponent->zIndex),
          isZIndexRelativeToParent(transform2DComponent->isZIndexRelativeToParent),
          ignoreCamera(transform2DComponent->ignoreCamera) {}

    SKATransform2D transform2D = SKA_TRANSFORM_IDENTITY;
    int zIndex = 0;
    bool isZIndexRelativeToParent = true;
    bool ignoreCamera = false;
};

struct SpriteComp : public EditorComponent {
    SpriteComp() = default;

    explicit SpriteComp(const SpriteComponent* spriteComponent, std::string texturePath, std::string shaderPath)
        : texturePath(std::move(texturePath)),
          drawSource(spriteComponent->drawSource),
          origin(spriteComponent->origin),
          flipH(spriteComponent->flipH),
          flipV(spriteComponent->flipV),
          modulate(spriteComponent->modulate),
          shaderPath(std::move(shaderPath)) {}

    std::string texturePath;
    SKARect2 drawSource = SKA_RECT2D_ZERO;
    SKAVector2 origin = SKA_VECTOR2_ZERO;
    bool flipH = false;
    bool flipV = false;
    SKAColor modulate = SKA_COLOR_WHITE;
    std::string shaderPath;
};

// TODO: Put editor animation stuff in another file...
struct EditorAnimationFrame {
    std::string texturePath;
    SKARect2 drawSource = SKA_RECT2D_ZERO;
    int frame = -1;
};

struct EditorAnimation {
    void RemoveAnimatationFrameByIndex(int frameIndex) {
        // Erase index from vector
        bool hasRemoved = false;
        animationFrames.erase(std::remove_if(animationFrames.begin(), animationFrames.end(), [frameIndex, &hasRemoved](const EditorAnimationFrame& animFrame) {
            const bool indexMatches = frameIndex == animFrame.frame;
            if (indexMatches) {
                hasRemoved = true;
            }
            return indexMatches;
        }), animationFrames.end());
        if (hasRemoved) {
            // Shift previous frames that were in front back a frame
            for (auto& animFrame : animationFrames) {
                if (animFrame.frame >= frameIndex) {
                    animFrame.frame--;
                }
            }
        } else {
            se_logger_error("Tried to remove frame nonexistent frame '%d' from animation '%s'", frameIndex,
                            name.c_str());
        }
    }

    bool HasAnimationFrame(int frameIndex) const {
        for (auto& animFrame : animationFrames) {
            if (animFrame.frame == frameIndex) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] EditorAnimationFrame& GetAnimationFrame(int frameIndex) {
        for (auto& animFrame : animationFrames) {
            if (animFrame.frame == frameIndex) {
                return animFrame;
            }
        }
        se_logger_error("Anim '%s' doesn't have frame '%d'", name.c_str(), frameIndex);
        static EditorAnimationFrame errorAnimFrame;
        return errorAnimFrame;
    }

    std::string name;
    int speed = 100;
    bool doesLoop = true;
    std::vector<EditorAnimationFrame> animationFrames;
};

struct AnimatedSpriteComp : public EditorComponent {
    AnimatedSpriteComp() = default;

    explicit AnimatedSpriteComp(const AnimatedSpriteComponentData* animatedSpriteComponentData, std::string shaderPath)
        : currentAnimationName(animatedSpriteComponentData->currentAnimation.name),
          modulate(animatedSpriteComponentData->modulate),
          isPlaying(animatedSpriteComponentData->isPlaying),
          origin(animatedSpriteComponentData->origin),
          flipH(animatedSpriteComponentData->flipH),
          flipV(animatedSpriteComponentData->flipV),
          staggerStartAnimationTimes(animatedSpriteComponentData->staggerStartAnimationTimes),
          shaderPath(std::move(shaderPath)) {
        for (size_t animationIndex = 0; animationIndex < animatedSpriteComponentData->animationCount; animationIndex++) {
            const AnimationData& animData = animatedSpriteComponentData->animations[animationIndex];
            EditorAnimation animation = { animData.name, animData.speed, animData.doesLoop };
            SE_ASSERT_FMT(!animation.name.empty(), "Animation is empty!");
            for (size_t frameIndex = 0; (int) frameIndex < animData.frameCount; frameIndex++) {
                const AnimationFrameData& frameData = animData.animationFrames[frameIndex];
                const EditorAnimationFrame animationFrame = { frameData.texturePath, frameData.drawSource, frameData.frame };
                animation.animationFrames.emplace_back(animationFrame);
            }
            animations.emplace_back(animation);
        }
    }

    void AddDefaultAnimation() {
        std::string animNameCandidate = "default";
        int nameIndex = 0;
        while (HasAnimationWithName(animNameCandidate)) {
            animNameCandidate = "default" + std::to_string(nameIndex++);
        }
        animations.emplace_back(EditorAnimation{ .name = animNameCandidate });
    }

    [[nodiscard]] bool HasAnimationWithName(const std::string& name) const {
        for (auto& anim : animations) {
            if (anim.name == name) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] EditorAnimation& GetAnimationByName(const std::string& name) const {
        for (auto& anim : animations) {
            if (anim.name == name) {
                return const_cast<EditorAnimation &>(anim);
            }
        }
        se_logger_error("Failed to get anim at with name '%s'", name.c_str());
        static EditorAnimation failedAnim;
        return failedAnim;
    }

    void RemoveAnimationByName(const std::string& name) {
        animations.erase(std::remove_if(animations.begin(), animations.end(), [name](const EditorAnimation& anim) {
            return name == anim.name;
        }), animations.end());
    }

    std::string currentAnimationName;
    std::vector<EditorAnimation> animations;
    SKAColor modulate = SKA_COLOR_WHITE;
    bool isPlaying = false;
    SKAVector2 origin = SKA_VECTOR2_ZERO;
    bool flipH = false;
    bool flipV = false;
    bool staggerStartAnimationTimes = false;
    std::string shaderPath;
};

struct TextLabelComp : public EditorComponent {
    TextLabelComp() = default;

    explicit TextLabelComp(const TextLabelComponent* textLabelComponent, std::string fontUID)
        : text(textLabelComponent->text),
          fontUID(std::move(fontUID)),
          color(textLabelComponent->color) {}

    std::string text;
    std::string fontUID;
    SKAColor color = SKA_COLOR_WHITE;
};

struct ScriptComp : public EditorComponent {
    ScriptComp() = default;

    explicit ScriptComp(const ScriptComponent* scriptComponent) :
        classPath(scriptComponent->classPath),
        className(scriptComponent->className) {}

    std::string classPath;
    std::string className;
};

struct Collider2DComp : public EditorComponent {
    Collider2DComp() = default;

    explicit Collider2DComp(const Collider2DComponent* collider2DComponent)
        : extents(collider2DComponent->extents),
          color(collider2DComponent->color) {}

    SKASize2D extents = SKA_SIZE2D_ZERO;
    SKAColor color = { .r = 0.0f, .g = 0.0f, .b = 0.8f, .a = 0.8f };
};

struct ColorRectComp : public EditorComponent {
    ColorRectComp() = default;

    explicit ColorRectComp(const ColorRectComponent* colorSquareComp)
        : size(colorSquareComp->size),
          color(colorSquareComp->color) {}

    SKASize2D size = { .w = 32.0f, .h = 32.0f };
    SKAColor color = SKA_COLOR_WHITE;
};

struct ParallaxComp : public EditorComponent {
    ParallaxComp() = default;

    explicit ParallaxComp(const ParallaxComponent* parallaxComponent)
        : scrollSpeed(parallaxComponent->scrollSpeed) {}

    SKAVector2 scrollSpeed = SKA_VECTOR2_ZERO;
};

struct Particles2DComp : public EditorComponent {
    Particles2DComp() = default;

    explicit Particles2DComp(const Particles2DComponent* particles2DComp)
            : amount(particles2DComp->amount),
              initialVelocity(particles2DComp->initialVelocity),
              color(particles2DComp->color),
              spread(particles2DComp->spread),
              lifeTime(particles2DComp->lifeTime),
              damping(particles2DComp->damping),
              explosiveness(particles2DComp->explosiveness) {}

    int amount = 8;
    SKAVector2 initialVelocity = SKA_VECTOR2_ZERO;
    SKAColor color = SKA_COLOR_WHITE;
    float spread = 45.0f;
    float lifeTime = 4.0f;
    float damping = 1.0f;
    float explosiveness = 0.0f;
};
