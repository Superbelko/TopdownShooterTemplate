// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "TDItem.generated.h"

/**
 * Base class for all pick up items
 */
UCLASS()
class TOPDOWNSHOOTER_API UTDItem : public UObject
{
	GENERATED_BODY()

public:
	UTDItem(const FObjectInitializer& ObjectInitializer);

};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem : public UTDItem
{
	GENERATED_BODY()
};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem_Light : public UTDItem
{
	GENERATED_BODY()
};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem_Shells : public UTDItem
{
	GENERATED_BODY()
};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem_Energy : public UTDItem
{
	GENERATED_BODY()
};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem_Rockets : public UTDItem
{
	GENERATED_BODY()
};

UCLASS()
class TOPDOWNSHOOTER_API UTDAmmoItem_Heavy : public UTDItem
{
	GENERATED_BODY()
};