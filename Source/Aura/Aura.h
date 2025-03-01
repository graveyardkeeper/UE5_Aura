// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define AURA_DEBUG
#undef AURA_DEBUG

#define CUSTOM_RENDER_DEPTH_RED 250
#define CUSTOM_RENDER_DEPTH_BLUE 251
#define CUSTOM_RENDER_DEPTH_TAN 252

// Collision Response Channels
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2
#define ECC_ExcludePlayers ECollisionChannel::ECC_GameTraceChannel3
