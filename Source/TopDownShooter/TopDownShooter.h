// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "Engine.h"


// Values below are from ShooterGame sample

/** when you modify this, please note that this information can be saved with instances
also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list */
#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PICKUP		ECC_GameTraceChannel3


#define TD_SURFACE_Default		SurfaceType_Default
#define TD_SURFACE_Concrete		SurfaceType1
#define TD_SURFACE_Dirt			SurfaceType2
#define TD_SURFACE_Water		SurfaceType3
#define TD_SURFACE_Metal		SurfaceType4
#define TD_SURFACE_Wood			SurfaceType5
#define TD_SURFACE_Grass		SurfaceType6
#define TD_SURFACE_Glass		SurfaceType7
#define TD_SURFACE_Flesh		SurfaceType8

/** keep in sync with TDImpactEffect */
UENUM()
enum class ETDPhysMaterialType : uint8
{
		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
};

