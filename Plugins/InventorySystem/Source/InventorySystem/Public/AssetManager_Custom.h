// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AssetManager_Custom.generated.h"

/**
 * UAssetManager_Custom
 */
UCLASS()
class INVENTORYSYSTEM_API UAssetManager_Custom : public UAssetManager
{
	GENERATED_BODY()

public:

	UAssetManager_Custom();
	
	static const FPrimaryAssetType InventoryItem;

	static UAssetManager_Custom& Get();
	virtual void StartInitialLoading() override;

	/**
	 * Synchronously loads an Item subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bLogWarning If true, this will log a warning if the item failed to load
	 */
	UAssetManager_Custom* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true) const;
};
