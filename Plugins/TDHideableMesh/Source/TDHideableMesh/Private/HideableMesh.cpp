// Copyright 2016 V.Khmelevskiy <absxv@yandex.ru>

#include "HideableMesh.h"
#include "GenericApplication.h"
#include "GenericApplicationMessageHandler.h"


class FTDHideableMeshModule : public ITDHideableMeshModule
{
public:
	virtual void StartupModule() override
	{		

	}
	
	virtual bool IsGameModule() const override
	{
		return true;
	}
};

IMPLEMENT_GAME_MODULE(FTDHideableMeshModule, TDHideableMesh);
