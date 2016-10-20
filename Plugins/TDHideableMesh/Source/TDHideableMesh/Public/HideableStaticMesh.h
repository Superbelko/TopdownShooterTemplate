// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HideableMeshInterface.h"
#include "HideableMeshActor.h"
#include "HideableStaticMesh.generated.h"


UCLASS()
class AHideableStaticMesh : public AStaticMeshActor, public ITDHideableMeshInterface, public FHideableMeshGeneric<AHideableStaticMesh>
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this actor's properties
	//AHideableStaticMesh(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	// ========================== ITDHideableMeshInterface ==========================

	// Sets default material instance to mesh
	virtual void OnBecomeOpaque_Implementation() override;

	// Sets translucent material instance to mesh
	virtual void OnBecomeTranslucent_Implementation() override;

private:
	// ========================== FHideableMeshGeneric interface ==========================

	friend class FHideableMeshGeneric<AHideableStaticMesh>;

	TArray<FMeshMaterials>& GetHMMaterials();

	TArray<UMaterialInstanceDynamic*>& GetHMDynMaterials();

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<FMeshMaterials> Materials;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynMaterials;

};
