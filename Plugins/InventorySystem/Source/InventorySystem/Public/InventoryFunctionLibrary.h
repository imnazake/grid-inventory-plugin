// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

/**
 * UInventoryFunctionLibrary
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsValidSlot(const FSlot& Slot);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsEmptySlot(const FSlot& Slot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsOccupiedSlot(const FSlot& Slot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsSlotOnMaxStackSize(const FSlot& Slot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static int32 GetSlotMissingStackQuantity(const FSlot& Slot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool IsValidEquipmentSlot(const FEquipmentSlot& EquipmentSlot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	static bool DoesItemHaveValidEquipmentSlot(const UItem* Item);
	
};
