// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDImpactEffect.h"


ATDImpactEffect::ATDImpactEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAutoDestroyWhenFinished = true;
}

void ATDImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* ATDImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
	case TD_SURFACE_Concrete:		ImpactFX = ConcreteFX; break;
	case TD_SURFACE_Dirt:			ImpactFX = DirtFX; break;
	case TD_SURFACE_Water:			ImpactFX = WaterFX; break;
	case TD_SURFACE_Metal:			ImpactFX = MetalFX; break;
	case TD_SURFACE_Wood:			ImpactFX = WoodFX; break;
	case TD_SURFACE_Grass:			ImpactFX = GrassFX; break;
	case TD_SURFACE_Glass:			ImpactFX = GlassFX; break;
	case TD_SURFACE_Flesh:			ImpactFX = FleshFX; break;
	default:						ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* ATDImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = NULL;

	switch (SurfaceType)
	{
	case TD_SURFACE_Concrete:		ImpactSound = ConcreteSound; break;
	case TD_SURFACE_Dirt:			ImpactSound = DirtSound; break;
	case TD_SURFACE_Water:			ImpactSound = WaterSound; break;
	case TD_SURFACE_Metal:			ImpactSound = MetalSound; break;
	case TD_SURFACE_Wood:			ImpactSound = WoodSound; break;
	case TD_SURFACE_Grass:			ImpactSound = GrassSound; break;
	case TD_SURFACE_Glass:			ImpactSound = GlassSound; break;
	case TD_SURFACE_Flesh:			ImpactSound = FleshSound; break;
	default:						ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}