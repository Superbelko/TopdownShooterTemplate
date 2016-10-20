// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HideableMeshInterface.generated.h"


/** Interface used for managing hideable actors */
UINTERFACE(Blueprintable, MinimalAPI)
class UTDHideableMeshInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


/** Native interface used for managing hideable actors */
class TDHIDEABLEMESH_API ITDHideableMeshInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	/** Sets opaque material instance to mesh(es), can hook blueprint event for interpolating parameters */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=HideableMesh)
	void OnBecomeOpaque();

	/** Sets translucent material instance to mesh(es), can hook blueprint event for interpolating parameters */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=HideableMesh)
	void OnBecomeTranslucent();
};