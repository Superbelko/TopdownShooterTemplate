// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDSaveGame.h"
#include "ArchiveUObject.h"



UTDSaveableInterface::UTDSaveableInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FTDSaveGameArchive::FTDSaveGameArchive(FArchive& InInnerArchive)
	: FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
{
	ArIsSaveGame = true;
}

UTDSaveGameData::UTDSaveGameData(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// TODO: replace with constant MAX_NUM_PLAYERS
	PlayerNames.SetNum(4);
	PlayerRecords.SetNum(4);
}
