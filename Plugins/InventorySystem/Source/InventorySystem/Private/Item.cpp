// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Pickup.h"
#include "ItemInstance.h"
#include "AssetManager_Custom.h"

UItem::UItem()
{
	AssetType = UAssetManager_Custom::InventoryItem;
	ItemInstanceClass = UItemInstance::StaticClass();

	bUseScaledWeight = true;
	bCanBeRotated = false;
	bCanBeStacked = false;

	Weight = 0.0f;
	MaxStackSize = 2;

	Size = FPoint2D(1, 1);

	PrimaryEquipmentSlot = EEquipmentSlotType::None;
	SecondaryEquipmentSlot = EEquipmentSlotType::None;

	ConsumedQuantityPerUsage = 1;

	PickupStaticMeshScale = FVector(0.25f);
}

FPrimaryAssetId UItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, GetFName());
}

FString UItem::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FString UItem::GetAssetName() const
{
	return GetPrimaryAssetId().PrimaryAssetName.ToString();
}

TArray<FPoint2D> UItem::GetSizeInCells() const
{
	TArray<FPoint2D> SizeInCells;
	
	for (int32 I = 0; I < Size.X; I++)
	{
		for (int32 J = 0; J < Size.Y; J++)
		{
			SizeInCells.Add(FPoint2D(I, J));
		}
	}

	return SizeInCells;
}

bool UItem::CanBeRotated() const
{
	return (Size.X != Size.Y || bCanBeRotated);
}

float UItem::GetScaledWeight() const
{
	if (bCanBeStacked)
	{
		return ((Size.X * Size.Y) / MaxStackSize); 
	}

	return (Size.X * Size.Y); 
}
