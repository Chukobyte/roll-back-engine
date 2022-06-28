#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../../animation/animation.h"
#include "../../data_structures/rbe_hash_map_string.h"
#include "../../math/rbe_math.h"

#define ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS 16

typedef struct AnimatedSpriteComponent {
    Animation animations[ANIMATED_SPRITE_COMPONENT_MAX_ANIMATIONS];
    Animation currentAnimation;
    size_t animationCount;
    Color modulate;
    bool isPlaying;
    bool flipX;
    bool flipY;
    uint32_t startAnimationTickTime;
} AnimatedSpriteComponent;

AnimatedSpriteComponent* animated_sprite_component_create();

void animated_sprite_component_add_animation(AnimatedSpriteComponent* animatedSpriteComponent, Animation animation);

Animation animated_sprite_component_get_animation(AnimatedSpriteComponent* animatedSpriteComponent, const char* name);

Animation* animated_sprite_component_get_animation_ref(AnimatedSpriteComponent* animatedSpriteComponent,
                                                       const char* name);
