// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Engine/DataAsset.h"
#include "Item.generated.h"

class UItemInstance;
class APickup;

/**
 * UItem
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYSYSTEM_API UItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UItem();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintPure, Category = "Item")
	FString GetIdentifierString() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	FString GetAssetName() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	const FPrimaryAssetType& GetAssetType() const
	{
		return AssetType;
	}

	UFUNCTION(BlueprintPure, Category = "Item")
	TArray<FPoint2D> GetSizeInCells() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	virtual bool CanBeRotated() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	float GetScaledWeight() const;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FPrimaryAssetType AssetType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FSlateBrush Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	EItemType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	TSubclassOf<UItemInstance> ItemInstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeDropped : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeDropped"), Category = "Item")
	TSubclassOf<APickup> PickupClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FString UnitPrefix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FString UnitSuffix;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeStacked : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 2, UIMin = 2), Category = "Item")
	int32 MaxStackSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FPoint2D Size;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bUseScaledWeight : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 0.0f, UIMin = 0.0f, EditCondition = "!bUseScaledWeight"), Category = "Item")
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	uint8 bCanBeRotated : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "!bCanBeConsumed"), Category = "Item")
	uint8 bCanBeEquipped : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeEquipped && !bCanBeConsumed"), Category = "Item")
	EEquipmentSlotType PrimaryEquipmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeEquipped && !bCanBeConsumed"), Category = "Item")
	EEquipmentSlotType SecondaryEquipmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "!bCanBeEquipped"), Category = "Item")
	uint8 bCanBeConsumed : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bCanBeConsumed", ClampMin = 1, UIMin = 1), Category = "Item")
	int32 ConsumedQuantityPerUsage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	UStaticMesh* PickupStaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Item")
	FVector PickupStaticMeshScale;
	
};
