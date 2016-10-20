// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HideableMeshInterface.h"
#include "HideableMeshActor.generated.h"

/** Default suffix used for searching translucent version of base material */
static const FString MatTranspSuffix = "_Translucent";

/**
* Lists all mesh materials and their translucent variants
*/
USTRUCT()
struct FMeshMaterials
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<TAssetPtr<UMaterialInterface>> MaterialsOpaque;

	UPROPERTY(VisibleAnywhere)
	TArray<TAssetPtr<UMaterialInterface>> MaterialsTranslucent;
};

/**
* Default events implementation class with minimal API, 
* one can use this when creating custom classes
*/
template <typename T>
class FHideableMeshGeneric
{

public:
	/// Initializes material list used later in game
	void PrepareMaterials()
	{
		TArray<FMeshMaterials>& Materials = static_cast<T*>(this)->GetHMMaterials();
		Materials.Empty();

		TArray<UActorComponent*> Components = static_cast<T*>(this)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (UActorComponent* comp : Components)
		{
			UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(comp);
			FMeshMaterials mats;
			for (UMaterialInterface* mi : smc->GetMaterials())
			{
				mats.MaterialsOpaque.Add(mi);

				FString path = mi->GetOutermost()->GetName() + MatTranspSuffix;
				UMaterialInterface* transparentMat = LoadObject<UMaterialInterface>(nullptr, *path);
				mats.MaterialsTranslucent.Add(transparentMat);
			}
			Materials.Add(mats);
		}
	}

	/// Sets default material instance to mesh
	void OnBecomeOpaqueImpl()
	{
		TArray<FMeshMaterials>& Materials = static_cast<T*>(this)->GetHMMaterials();
		int meshId = 0;
		TArray<UActorComponent*> Components = static_cast<T*>(this)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (UActorComponent* comp : Components)
		{
			UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(comp);

			// internal counter, prevents shifting if material was not assigned
			int i = 0;
			// assign stored material in same order
			for (const auto& mi : Materials[meshId].MaterialsOpaque)
			{
				if (mi.IsValid())
					smc->SetMaterial(i, mi.Get());
				i++;
			}
			meshId++;
		}
	}

	/// Sets translucent material instance to mesh
	void OnBecomeTranslucentImpl()
	{
		TArray<FMeshMaterials>& Materials = static_cast<T*>(this)->GetHMMaterials();
		int meshId = 0;
		TArray<UActorComponent*> Components = static_cast<T*>(this)->GetComponentsByClass(UStaticMeshComponent::StaticClass());
		for (UActorComponent* comp : Components)
		{
			UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(comp);

			// internal counter, prevents shifting if material was not assigned
			int i = 0;
			// assign stored material in same order
			for (const auto& mi : Materials[meshId].MaterialsTranslucent)
			{
				if (mi.IsValid())
					smc->SetMaterial(i, mi.Get());
				i++;
			}
			meshId++;
		}
	}
};


/**
* Basic hideable actor with ability to respond to ITDHideableMeshInterface messages
*/
UCLASS()
class TDHIDEABLEMESH_API AHideableMeshActor : public AActor, public ITDHideableMeshInterface, public FHideableMeshGeneric<AHideableMeshActor>
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHideableMeshActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	// ========================== ITDHideableMeshInterface ==========================

	// Sets default material instance to mesh
	virtual void OnBecomeOpaque_Implementation() override; 

	// Sets translucent material instance to mesh
	virtual void OnBecomeTranslucent_Implementation() override;

private:
	// ========================== FHideableMeshGeneric interface ==========================

	friend class FHideableMeshGeneric<AHideableMeshActor>;

	TArray<FMeshMaterials>& GetHMMaterials() 
	{
		return Materials;
	}

	TArray<UMaterialInstanceDynamic*>& GetHMDynMaterials()
	{
		return DynMaterials;
	}

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	TArray<FMeshMaterials> Materials;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynMaterials;
};