// Copyright Jordan McWilliams

#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName == InMapName) return SavedMap;
	}
	return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName == InMapName) return true;
	}
	return false;
}
