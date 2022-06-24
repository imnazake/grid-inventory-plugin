// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryFunctionLibrary.h"
#include "Item.h"

bool UInventoryFunctionLibrary::IsValidSlot(const FSlot& Slot)
{
	return Slot.IsValid();
}

bool UInventoryFunctionLibrary::IsEmptySlot(const FSlot& Slot)
{
	return Slot.IsEmpty();
}

bool UInventoryFunctionLibrary::IsOccupiedSlot(const FSlot& Slot)
{
	return Slot.IsOccupied();
}

bool UInventoryFunctionLibrary::IsSlotOnMaxStackSize(const FSlot& Slot)
{
	return Slot.IsOnMaxStackSize();
}

int32 UInventoryFunctionLibrary::GetSlotMissingStackQuantity(const FSlot& Slot)
{
	return Slot.GetMissingStackQuantity();
}

bool UInventoryFunctionLibrary::IsValidEquipmentSlot(const FEquipmentSlot& EquipmentSlot)
{
	return EquipmentSlot.IsValid();
}

bool UInventoryFunctionLibrary::DoesItemHaveValidEquipmentSlot(const UItem* Item)
{
	if (Item == nullptr)
	{
		return false;
	}
	
	if (Item->PrimaryEquipmentSlot == EEquipmentSlotType::None)
	{
		return false;
	}

	return true;
}
