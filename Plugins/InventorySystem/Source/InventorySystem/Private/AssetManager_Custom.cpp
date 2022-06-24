// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetManager_Custom.h"

const FPrimaryAssetType	UAssetManager_Custom::InventoryItem = TEXT("InventoryItem");

UAssetManager_Custom::UAssetManager_Custom()
{
	
}

UAssetManager_Custom& UAssetManager_Custom::Get()
{
	UAssetManager_Custom* This = Cast<UAssetManager_Custom>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be AssetManager_Custom!"));
		return *NewObject<UAssetManager_Custom>();
	}
}

void UAssetManager_Custom::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UAssetManager_Custom* UAssetManager_Custom::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, const bool bLogWarning) const
{
	const FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UAssetManager_Custom* LoadedItem = Cast<UAssetManager_Custom>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
