// Fill out your copyright notice in the Description page of Project Settings.

#include "HideableMesh.h"
#include "HideableStaticMesh.h"


AHideableStaticMesh::AHideableStaticMesh(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// just for compiler so its not get optimized out
	DynMaterials.Empty();
	Materials.Empty();
}

void AHideableStaticMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	PrepareMaterials();
}

void AHideableStaticMesh::OnBecomeOpaque_Implementation()
{
	OnBecomeOpaqueImpl();
}

void AHideableStaticMesh::OnBecomeTranslucent_Implementation()
{
	OnBecomeTranslucentImpl();
}

TArray<FMeshMaterials>& AHideableStaticMesh::GetHMMaterials()
{
	return Materials;
}

TArray<UMaterialInstanceDynamic*>& AHideableStaticMesh::GetHMDynMaterials()
{
	return DynMaterials;
}

