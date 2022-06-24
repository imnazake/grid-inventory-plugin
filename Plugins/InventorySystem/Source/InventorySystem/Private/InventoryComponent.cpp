// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "ItemInstance.h"
#include "Item.h"
#include "Pickup.h"

bool FSlot::IsOnMaxStackSize() const
{
	if (ItemInstance == nullptr)
	{
		return false;
	}

	if (ItemInstance->Item->bCanBeStacked)
	{
		if (Quantity >= ItemInstance->Item->MaxStackSize)
		{
			return true;
		}

		return false;
	}

	return false;
}

int32 FSlot::GetMissingStackQuantity() const
{
	if (ItemInstance == nullptr)
	{
		return 0;
	}

	if (ItemInstance->Item->bCanBeStacked)
	{
		return ItemInstance->Item->MaxStackSize - Quantity;
	}

	return 0;
}

void FSlot::SetQuantity(const int32 InQuantity)
{
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	if (ItemInstance->Item->bCanBeStacked)
	{
		Quantity = FMath::Clamp(InQuantity, 0, ItemInstance->Item->MaxStackSize);
	}
	else
	{
		Quantity = FMath::Clamp(InQuantity, 0, 1);
	}
}

void FSlot::UpdateQuantity(const int32 InQuantity)
{
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	if (ItemInstance->Item->bCanBeStacked)
	{
		Quantity = FMath::Clamp(Quantity + InQuantity, 0, ItemInstance->Item->MaxStackSize);
	}
	else
	{
		Quantity = FMath::Clamp(Quantity + InQuantity, 0, 1);
	}
}

bool FSlot::IsEmpty() const
{
	return (ItemInstance == nullptr && Quantity == 0 && OwnerInventory != nullptr);
}

bool FSlot::IsOccupied() const
{
	return (ItemInstance != nullptr && Quantity > 0 && OwnerInventory != nullptr);
}

bool FSlot::IsValid() const
{
	return (OwnerInventory != nullptr && Quantity >= 0);
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	GridSize = FPoint2D(1, 1);
	CellSize = 50.0f;
	
	DefaultMoney = 100;
	Money = 0;
	
	CurrentWeight = 0.0f;
	MaxWeight = 0.0f;

	bUseScaledMaxWeight = true;
	PickupSpawnRadiusFromPlayer = 100.0f;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	AddStartupItems();
}

UItemInstance* UInventoryComponent::CreateItemInstance(const TSubclassOf<UItemInstance> ItemInstanceClass) const
{
	UItemInstance* ItemInstance = NewObject<UItemInstance>(GetOwner(), ItemInstanceClass);
	check(ItemInstance != nullptr);

	ItemInstance->NativeOnConstruct();
	ItemInstance->OnConstruct();

	if (ItemInstance->Item == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Item data asset. Please select an item data asset in %s"), *GetNameSafe(ItemInstance));
	}

	if (ItemInstance->Item && ItemInstance->Item->ItemInstanceClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ItemInstance class. Please select an ItemInstance class in %s"), *GetNameSafe(ItemInstance->Item));
	}

	return ItemInstance;
}

bool UInventoryComponent::IsWithinBoundaries(const FPoint2D& Coordinates) const
{
	const bool bIsWithinBoundaries = Coordinates.X >= 0 && Coordinates.Y >= 0 && Coordinates.X < GridSize.X && Coordinates.Y < GridSize.Y;
	return bIsWithinBoundaries;
}

bool UInventoryComponent::IsFreeCell(const FPoint2D& Coordinates)
{
	if (!IsWithinBoundaries(Coordinates))
	{
		return false;
	}

	for (const FSlot& Slot: Slots)
	{
		for (const FPoint2D& Cell: Slot.ItemInstance->SizeInCells)
		{
			const bool bIsNotFree = (Cell + Slot.ItemInstance->TopLeftCoordinates == Coordinates);
			if (bIsNotFree)
			{
				return false;
			}
		}
	}

	return true;
}

bool UInventoryComponent::DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates)
{
	for (const FPoint2D& Cell: SizeInCells)
	{
		if (!IsFreeCell(FPoint2D(Coordinates + Cell)))
		{
			return false;
		}
	}

	return true;
}

FPoint2D UInventoryComponent::GetFreeCell()
{
	for (const FPoint2D& Cell: Cells)
	{
		if (IsFreeCell(Cell))
		{
			return Cell;
		}
	}

	return FPoint2D(INDEX_NONE, INDEX_NONE);
}

FPoint2D UInventoryComponent::GetFreeCellWhereItemCanFit(const TArray<FPoint2D>& SizeInCells)
{
	for (const FPoint2D& Cell: Cells)
	{
		const bool bItemCanFit = IsFreeCell(Cell) && DoesItemFit(SizeInCells, Cell);
		if (bItemCanFit)
		{
			return Cell;
		}
	}

	return FPoint2D(INDEX_NONE, INDEX_NONE);
}

bool UInventoryComponent::IsFull() const
{
	return CurrentWeight >= MaxWeight;
}

bool UInventoryComponent::DoesItemExist(const UItem* Item)
{
	for (const FSlot& Slot: Slots)
	{
		if (Slot.ItemInstance->Item == Item)
		{
			return true;
		}
	}

	return false;
}

int32 UInventoryComponent::CountItemQuantity(const UItem* Item)
{
	int32 Quantity = 0;
	
	for (const FSlot& Slot: Slots)
	{
		if (Slot.ItemInstance->Item == Item)
		{
			Quantity += Slot.Quantity;
		}
	}

	return Quantity;
}

FSlot UInventoryComponent::GetSlotByCoordinates(const FPoint2D& Coordinates)
{
	for (const FSlot& Slot: Slots)
	{
		for (const FPoint2D& Cell: Slot.ItemInstance->SizeInCells)
		{
			const bool bIsWithinThisSlot = (Slot.ItemInstance->TopLeftCoordinates + Cell == Coordinates);
			if (bIsWithinThisSlot)
			{
				return Slot;
			}
		}
	}

	return FSlot();
}

int32 UInventoryComponent::GetSlotIndexByCoordinates(const FPoint2D& Coordinates)
{
	int32 Index = INDEX_NONE;
	
	for (const FSlot& Slot: Slots)
	{
		Index++;
		
		for (const FPoint2D& Cell: Slot.ItemInstance->SizeInCells)
		{
			const bool bIsWithinThisSlot = (Slot.ItemInstance->TopLeftCoordinates + Cell == Coordinates);
			if (bIsWithinThisSlot)
			{
				return Index;
			}
		}
	}

	return INDEX_NONE;
}

bool UInventoryComponent::CanCarryItem(const UItem* Item, const int32 Quantity) const
{
	float EstimatedWeight;

	if (Item->bUseScaledWeight)
	{
		EstimatedWeight = Quantity * Item->GetScaledWeight();
	}
	else
	{
		EstimatedWeight = Quantity * Item->Weight;
	}

	return (CurrentWeight + EstimatedWeight <= MaxWeight);
}

void UInventoryComponent::Initialize()
{
	if (bUseScaledMaxWeight)
	{
		MaxWeight = GridSize.X * GridSize.Y;
	}
	
	CurrentWeight = 0.0f;
	Cells.Empty();
	Slots.Empty();

	for (int32 I = 0; I < GridSize.X; I++)
	{
		for (int32 J = 0; J < GridSize.Y; J++)
		{
			Cells.Add(FPoint2D(I, J));
		}
	}

	for (FEquipmentSlot& EquipmentSlot: EquipmentSlots)
	{
		EquipmentSlot.Data.ItemInstance = nullptr;
		EquipmentSlot.Data.OwnerInventory = this;
		EquipmentSlot.Data.Quantity = 0;
	}
	
	NotifyInventoryInitialized();
}

void UInventoryComponent::AddStartupItems()
{
	for (const FStartupItem& StartupItem: StartupItems)
	{
		int32 AddedQuantity = 0;
		AddNewItem(StartupItem.Item, StartupItem.Quantity, AddedQuantity);
	}

	AddMoney(DefaultMoney);
}

bool UInventoryComponent::AddNewItem(UItem* Item, const int32 Quantity, int32& AddedQuantity)
{
	AddedQuantity = 0;

	if (Item == nullptr)
	{
		return false;
	}

	if (Quantity <= 0)
	{
		return false;
	}

	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	int32 RemainingQuantity = Quantity;

	if (Item->bCanBeStacked)
	{
		if (DoesItemExist(Item))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemInstance->Item == Item;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(Item, MissingStackQuantity))
						{
							NotifyInventoryUpdated();
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
					
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, AddedQuantity);
						return true;
					}

					if (!CanCarryItem(Item, MissingStackQuantity))
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}

		while (RemainingQuantity >= Item->MaxStackSize)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);

			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, Item->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
				Slots.Add(NewSlot);

				AddedQuantity += Item->MaxStackSize;
				RemainingQuantity -= Item->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, Item->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

						FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
						Slots.Add(NewSlot);

						AddedQuantity += Item->MaxStackSize;
						RemainingQuantity -= Item->MaxStackSize;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);

			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;

				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);

				AddedQuantity += RemainingQuantity;

				NotifyInventoryUpdated();
				NotifyInventoryWeightChanged();
				NotifyInventoryItemAdded(Item, RemainingQuantity);

				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);

						AddedQuantity += RemainingQuantity;

						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, RemainingQuantity);

						return true;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemAdded(Item, AddedQuantity);

		return true;
	}

	while (RemainingQuantity > 0)
	{
		UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
		check(NewItemInstance != nullptr);

		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;

			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);

			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (Item->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);

				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;

					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);

					AddedQuantity += 1;
					RemainingQuantity -= 1;
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryUpdated();
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemAdded(Item, AddedQuantity);

	return true;
}

bool UInventoryComponent::AddExistingItem(UItemInstance* ItemInstance, const int32 Quantity, int32& AddedQuantity)
{
	AddedQuantity = 0;
	UItem* Item = ItemInstance->Item;
	
	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	if (Item == nullptr)
	{
		return false;
	}
	
	int32 RemainingQuantity = Quantity;
	
	if (Item->bCanBeStacked)
	{
		if (DoesItemExist(Item))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemInstance->Item == Item;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(Item, MissingStackQuantity))
						{
							NotifyInventoryUpdated();
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
					
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, AddedQuantity);
						return true;
					}
	
					if (!CanCarryItem(Item, MissingStackQuantity))
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}
	
		while (RemainingQuantity >= Item->MaxStackSize)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);
	
			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, Item->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += Item->MaxStackSize;
				RemainingQuantity -= Item->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, Item->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += Item->MaxStackSize;
						RemainingQuantity -= Item->MaxStackSize;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);
	
			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += RemainingQuantity;
	
				NotifyInventoryUpdated();
				NotifyInventoryWeightChanged();
				NotifyInventoryItemAdded(Item, RemainingQuantity);
	
				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += RemainingQuantity;
	
						NotifyInventoryUpdated();
						NotifyInventoryWeightChanged();
						NotifyInventoryItemAdded(Item, RemainingQuantity);
	
						return true;
					}
					else
					{
						NotifyInventoryUpdated();
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
	
		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemAdded(Item, AddedQuantity);
	
		return true;
	}
	
	while (RemainingQuantity > 0)
	{
		UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
		check(NewItemInstance != nullptr);
	
		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);
	
			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (Item->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);
	
					AddedQuantity += 1;
					RemainingQuantity -= 1;
				}
				else
				{
					NotifyInventoryUpdated();
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryUpdated();
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}
	
	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemAdded(Item, AddedQuantity);

	return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item, const int32 Quantity, int32& RemovedQuantity)
{
	RemovedQuantity = 0;

	if (Item == nullptr)
	{
		return false;
	}

	if (Quantity <= 0)
	{
		return false;
	}

	if (!DoesItemExist(Item))
	{
		return false;
	}

	const int32 ExistingQuantity = CountItemQuantity(Item);
	int32 PendingQuantity = Quantity;

	if (Quantity >= ExistingQuantity)
	{
		for (auto It = Slots.CreateIterator(); It; ++It)
		{
			if (Item == It->ItemInstance->Item)
			{
				RemovedQuantity += It->Quantity;
				It.RemoveCurrent();
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(Item, RemovedQuantity);
		return true;
	}

	if (Item->bCanBeStacked)
	{
		for (auto It = Slots.CreateIterator(); It; ++It)
		{
			if (Item == It->ItemInstance->Item && PendingQuantity > 0)
			{
				if (PendingQuantity >= It->Quantity)
				{
					RemovedQuantity += It->Quantity;
					PendingQuantity -= It->Quantity;
					It.RemoveCurrent();
				}
				else
				{
					It->UpdateQuantity(-PendingQuantity);
					RemovedQuantity += PendingQuantity;
					PendingQuantity = 0;
					break;
				}
			}
		}

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(Item, RemovedQuantity);
		return true;
	}

	// Removing non-stackable items
	for (auto It = Slots.CreateIterator(); It; ++It)
	{
		if (Item == It->ItemInstance->Item && PendingQuantity > 0)
		{
			RemovedQuantity += It->Quantity;
			PendingQuantity -= It->Quantity;
			It.RemoveCurrent();
		}
	}

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemRemoved(Item, RemovedQuantity);
	return true;
}

bool UInventoryComponent::RemoveItemOnSlot(const FSlot& Slot, const int32 Quantity, int32& RemovedQuantity)
{
	RemovedQuantity = 0;

	if (Quantity <= 0)
	{
		return false;
	}

	const int32 SlotIndex = GetSlotIndexByCoordinates(Slot.ItemInstance->TopLeftCoordinates);

	if (Quantity >= Slot.Quantity)
	{
		UItem* RemovedItem = Slot.ItemInstance->Item;
		
		RemovedQuantity += Slot.Quantity;
		Slots.Remove(Slot);

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
		NotifyInventoryItemRemoved(RemovedItem, RemovedQuantity);
		return true;
	}

	RemovedQuantity += Quantity;
	Slots[SlotIndex].UpdateQuantity(-Quantity);

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	NotifyInventoryItemRemoved(Slot.ItemInstance->Item, RemovedQuantity);
	return true;
}

bool UInventoryComponent::MoveItemOnSlot(const FSlot& Slot, const FPoint2D& Destination)
{
	if (!IsFreeCell(Destination))
	{
		Slots.Add(Slot);
		return false;
	}

	if (!DoesItemFit(Slot.ItemInstance->SizeInCells, Destination))
	{
		Slots.Add(Slot);
		return false;
	}

	Slot.ItemInstance->TopLeftCoordinates = Destination;
	Slots.Add(Slot);

	NotifyInventoryUpdated();
	NotifyInventoryWeightChanged();
	return true;
}

void UInventoryComponent::StackItemStackOnSlot(const FSlot& Slot, const FPoint2D& Destination, const int32 Quantity)
{
	if (IsFreeCell(Destination))
	{
		return;
	}

	if (!IsWithinBoundaries(Destination))
	{
		return;
	}

	if (Quantity <= 0)
	{
		return;
	}

	const FSlot DestinationSlot = GetSlotByCoordinates(Destination);
	const int32 DestinationIndex = Slots.Find(DestinationSlot);
	const int32 SourceIndex = Slots.Find(Slot);

	if (Slot.ItemInstance->Item != DestinationSlot.ItemInstance->Item || !DestinationSlot.ItemInstance->Item->bCanBeStacked)
	{
		return;
	}

	if (DestinationSlot.IsOnMaxStackSize() || Quantity > Slot.Quantity)
	{
		return;
	}

	const int32 MissingStackQuantity = DestinationSlot.GetMissingStackQuantity();

	if (Slot.Quantity <= Quantity)
	{
		if (Quantity <= MissingStackQuantity)
		{
			Slots[DestinationIndex].UpdateQuantity(Slot.Quantity);
			Slots.Remove(Slot);

			NotifyInventoryWeightChanged();
			NotifyInventoryUpdated();

			return;
		}

		Slots[DestinationIndex].UpdateQuantity(MissingStackQuantity);
		Slots[SourceIndex].UpdateQuantity(-MissingStackQuantity);

		NotifyInventoryWeightChanged();
		NotifyInventoryUpdated();

		return;
	}

	if (Quantity <= MissingStackQuantity)
	{
		Slots[DestinationIndex].UpdateQuantity(Quantity);
		Slots[SourceIndex].UpdateQuantity(-Quantity);

		NotifyInventoryWeightChanged();
		NotifyInventoryUpdated();

		return;
	}

	Slots[DestinationIndex].UpdateQuantity(MissingStackQuantity);
	Slots[SourceIndex].UpdateQuantity(-MissingStackQuantity);

	NotifyInventoryWeightChanged();
	NotifyInventoryUpdated();
}

void UInventoryComponent::EquipItemOnSlot(const FSlot& Slot)
{
	if (EquipmentSlots.Num() <= 0)
	{
		return;
	}
	
	if (!Slot.IsValid() || Slot.IsEmpty())
	{
		return;
	}

	if (!Slot.ItemInstance->Item->bCanBeEquipped)
	{
		return;
	}

	if (!UInventoryFunctionLibrary::DoesItemHaveValidEquipmentSlot(Slot.ItemInstance->Item))
	{
		return;
	}

	if (!IsValidEquipmentSlots())	// check if we have filled equipment slots array from editor
	{
		return;
	}

	const FEquipmentSlot PrimarySlot = GetEquipmentSlotByType(Slot.ItemInstance->Item->PrimaryEquipmentSlot);
	const FEquipmentSlot SecondarySlot = GetEquipmentSlotByType(Slot.ItemInstance->Item->SecondaryEquipmentSlot);

	if (PrimarySlot.Data.IsEmpty())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(PrimarySlot.Type);

		EquipmentSlots[EquipmentSlotIndex].Data = Slot;
		EquipmentSlots[EquipmentSlotIndex].Data.ItemInstance->ResetRotation();
		
		NotifyInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
		K2_OnInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
		Slots.Remove(Slot);

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
	}
	else if (SecondarySlot.IsValid() && SecondarySlot.Data.IsEmpty())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(SecondarySlot.Type);

		EquipmentSlots[EquipmentSlotIndex].Data = Slot;
		EquipmentSlots[EquipmentSlotIndex].Data.ItemInstance->ResetRotation();
		
		NotifyInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
		K2_OnInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
		Slots.Remove(Slot);

		NotifyInventoryUpdated();
		NotifyInventoryWeightChanged();
	}
	else if (PrimarySlot.Data.IsOccupied())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(PrimarySlot.Type);

		int32 AddedQuantity = 0;
		const bool bIsAdded = AddExistingItem_Internal(PrimarySlot.Data.ItemInstance, PrimarySlot.Data.Quantity, AddedQuantity);

		if (bIsAdded)
		{
			EquipmentSlots[EquipmentSlotIndex].Data = Slot;
			EquipmentSlots[EquipmentSlotIndex].Data.ItemInstance->ResetRotation();
			
			NotifyInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
			K2_OnInventoryItemEquipped(Slot.ItemInstance->Item, Slot.Quantity);
			Slots.Remove(Slot);

			NotifyInventoryUpdated();
			NotifyInventoryWeightChanged();
		}
	}
}

void UInventoryComponent::UnequipItem(const EEquipmentSlotType EquipmentSlot)
{
	if (EquipmentSlot == EEquipmentSlotType::None)
	{
		return;
	}

	const FEquipmentSlot TargetSlot = GetEquipmentSlotByType(EquipmentSlot);

	if (TargetSlot.Data.IsOccupied())
	{
		const int32 EquipmentSlotIndex = GetEquipmentSlotIndexByType(EquipmentSlot);
		
		int32 AddedQuantity = 0;
		const bool bIsAdded = AddExistingItem_Internal(TargetSlot.Data.ItemInstance, TargetSlot.Data.Quantity, AddedQuantity);

		if (bIsAdded && AddedQuantity == TargetSlot.Data.Quantity)
		{
			//EquipmentSlots[EquipmentSlotIndex].Data.OwnerInventory = nullptr;
			EquipmentSlots[EquipmentSlotIndex].Data.ItemInstance = nullptr;
			EquipmentSlots[EquipmentSlotIndex].Data.Quantity = 0;

			NotifyInventoryItemUnequipped(TargetSlot.Data.ItemInstance->Item, TargetSlot.Data.Quantity);
			K2_OnInventoryItemUnequipped(TargetSlot.Data.ItemInstance->Item, TargetSlot.Data.Quantity);

			NotifyInventoryUpdated();
			NotifyInventoryWeightChanged();
		}
	}
}

bool UInventoryComponent::DropItemOnSlot(const FSlot& Slot)
{
	if (!Slot.ItemInstance->Item->bCanBeDropped)
	{
		return false;
	}

	if (!Slot.ItemInstance->Item->PickupClass)
	{
		return false;
	}

	const FSlot DataCopy = Slot;
	
	int32 RemovedQuantity = 0;
    const bool bIsRemoved = RemoveItemOnSlot(Slot, Slot.Quantity, RemovedQuantity);
    if (bIsRemoved)
    {
    	const FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * PickupSpawnRadiusFromPlayer;

    	FActorSpawnParameters SpawnParams;
    	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    	APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(DataCopy.ItemInstance->Item->PickupClass, SpawnLocation, FRotator(), SpawnParams);
    	if (SpawnedPickup)
    	{
    		SpawnedPickup->SetActorScale3D(DataCopy.ItemInstance->Item->PickupStaticMeshScale);
    		SpawnedPickup->SetPickupData(DataCopy.ItemInstance, DataCopy.Quantity);
    		return true;
    	}

    	return false;
    }

	return false;
}

bool UInventoryComponent::LootItem(APickup* Pickup, int32& LootedQuantity)
{
	LootedQuantity = 0;
	
	if (Pickup == nullptr)
	{
		return false;
	}

	int32 AddedQuantity = 0;
	const bool bIsLooted = AddExistingItem(Pickup->ItemInstance, Pickup->Quantity, AddedQuantity);

	LootedQuantity = AddedQuantity;
	
	if (bIsLooted)
	{
		if (Pickup->Quantity - LootedQuantity <= 0)
		{
			Pickup->Destroy();
		}
		else
		{
			Pickup->Quantity = FMath::Clamp((Pickup->Quantity - LootedQuantity), 0, INT32_MAX);
		}

		return true;
	}
	
	return false;
}

void UInventoryComponent::SpawnItem(const UItem* Item, const int32 Quantity, const FTransform& Transform)
{
	if (Item == nullptr)
	{
		return;
	}

	if (Quantity <= 0)
	{
		return;
	}

	if (!Transform.IsValid())
	{
		return;
	}

	if (!Item->bCanBeDropped)
	{
		return;
	}

	if (!Item->PickupClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UItemInstance* ItemInstance = CreateItemInstance(Item->ItemInstanceClass);
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	APickup* SpawnedPickup = GetWorld()->SpawnActor<APickup>(Item->PickupClass, Transform.GetLocation(), Transform.GetRotation().Rotator(), SpawnParams);
	if (SpawnedPickup)
	{
		SpawnedPickup->SetActorScale3D(Item->PickupStaticMeshScale);
		SpawnedPickup->SetPickupData(ItemInstance, Quantity);
	}
}

void UInventoryComponent::UseItemOnSlot(const FSlot& Slot)
{
	if (Slot.IsEmpty())
	{
		return;
	}
	
	if (!Slot.ItemInstance->Item->bCanBeConsumed)
	{
		return;
	}

	if (Slot.Quantity < Slot.ItemInstance->Item->ConsumedQuantityPerUsage)
	{
		return;
	}

	UItemInstance* UsedItemInstance = Slot.ItemInstance;
	const int32 UsedQuantity = UsedItemInstance->Item->ConsumedQuantityPerUsage;

	int32 RemovedQuantity = 0;
	const bool bIsConsumed = RemoveItemOnSlot(Slot, Slot.ItemInstance->Item->ConsumedQuantityPerUsage, RemovedQuantity);
	if (bIsConsumed)
	{
		UsedItemInstance->OnUsed();
		NotifyInventoryItemUsed(UsedItemInstance->Item, UsedQuantity);
	}
}

void UInventoryComponent::AddMoney(const int32 Value)
{
	if (Value > 0)
	{
		Money = FMath::Clamp(Money + Value, 0, INT32_MAX);
		NotifyMoneyChanged();
	}
}

void UInventoryComponent::RemoveMoney(const int32 Value)
{
	if (Value > 0)
	{
		Money = FMath::Clamp(Money - Value, 0, INT32_MAX);
		NotifyMoneyChanged();
	}
}

int32 UInventoryComponent::GetMoney() const
{
	return Money;
}

bool UInventoryComponent::IsValidEquipmentSlots()
{
	if (EquipmentSlots.Num() <= 0)
	{
		return false;
	}
	
	for (const FEquipmentSlot& EquipmentSlot: EquipmentSlots)
	{
		if (EquipmentSlot.Type == EEquipmentSlotType::None)
		{
			return false;
		}
	}

	int32 CountSlotType = 0;
	
	// check if equipment slots have some duplicate entries
	for (const FEquipmentSlot& PrimarySlot: EquipmentSlots)
	{
		for (const FEquipmentSlot& SecondarySlot: EquipmentSlots)
		{
			if (PrimarySlot.Type == SecondarySlot.Type)
			{
				CountSlotType++;
			}
		}

		if (CountSlotType > 1)
		{
			return false;
		}
		else
		{
			CountSlotType = 0;
		}
	}

	return true;
}

FEquipmentSlot UInventoryComponent::GetEquipmentSlotByType(const EEquipmentSlotType SlotType)
{
	for (const FEquipmentSlot& Slot: EquipmentSlots)
	{
		if (Slot.Type == SlotType)
		{
			return Slot;
		}
	}

	return FEquipmentSlot();
}

int32 UInventoryComponent::GetEquipmentSlotIndexByType(const EEquipmentSlotType SlotType)
{
	int32 Index = INDEX_NONE;
	for (const FEquipmentSlot& Slot: EquipmentSlots)
	{
		Index++;
		
		if (Slot.Type == SlotType)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

bool UInventoryComponent::AddExistingItem_Internal(const UItemInstance* ItemInstance, const int32 Quantity, int32& AddedQuantity)
{
	AddedQuantity = 0;
	const UItem* Item = ItemInstance->Item;
	
	if (IsFull())
	{
		NotifyInventoryInsufficientSpace();
		return false;
	}
	
	if (Item == nullptr)
	{
		return false;
	}
	
	int32 RemainingQuantity = Quantity;
	
	if (Item->bCanBeStacked)
	{
		if (DoesItemExist(Item))
		{
			for (FSlot& Slot: Slots)
			{
				const bool bCanStack = !Slot.IsOnMaxStackSize() && Slot.ItemInstance->Item == Item;
				if (bCanStack)
				{
					const int32 MissingStackQuantity = Slot.GetMissingStackQuantity();
				
					if (RemainingQuantity <= MissingStackQuantity)
					{
						if (!CanCarryItem(Item, MissingStackQuantity))
						{
							NotifyInventoryInsufficientSpace();
							return false;
						}
					
						Slot.UpdateQuantity(MissingStackQuantity);
						AddedQuantity += MissingStackQuantity;
						RemainingQuantity -= MissingStackQuantity;
						
						return true;
					}
	
					if (!CanCarryItem(Item, MissingStackQuantity))
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				
					Slot.UpdateQuantity(MissingStackQuantity);
					AddedQuantity += MissingStackQuantity;
					RemainingQuantity -= MissingStackQuantity;
				}
			}
		}
	
		while (RemainingQuantity >= Item->MaxStackSize)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);
	
			FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, Item->MaxStackSize))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
				
				FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += Item->MaxStackSize;
				RemainingQuantity -= Item->MaxStackSize;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, Item->MaxStackSize))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						FSlot NewSlot = FSlot(NewItemInstance, Item->MaxStackSize, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += Item->MaxStackSize;
						RemainingQuantity -= Item->MaxStackSize;
					}
					else
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
		
		if (RemainingQuantity > 0)
		{
			UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
			check(NewItemInstance != nullptr);
	
			const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
			if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, RemainingQuantity))
			{
				NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
				const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
				Slots.Add(NewSlot);
	
				AddedQuantity += RemainingQuantity;
	
				return true;
			}
			else
			{
				// try to rotate the item and see if it fits
				if (Item->CanBeRotated())
				{
					NewItemInstance->Rotate();
					const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
					if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, RemainingQuantity))
					{
						NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
						const FSlot NewSlot = FSlot(NewItemInstance, RemainingQuantity, this);
						Slots.Add(NewSlot);
	
						AddedQuantity += RemainingQuantity;
	
						return true;
					}
					else
					{
						NotifyInventoryInsufficientSpace();
						return false;
					}
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
		}
	
		return true;
	}
	
	while (RemainingQuantity > 0)
	{
		UItemInstance* NewItemInstance = CreateItemInstance(Item->ItemInstanceClass);
		check(NewItemInstance != nullptr);
	
		const FPoint2D CoordsWhereItemCanFit = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
		if (IsWithinBoundaries(CoordsWhereItemCanFit) && CanCarryItem(Item, 1))
		{
			NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFit;
	
			const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
			Slots.Add(NewSlot);
	
			AddedQuantity += 1;
			RemainingQuantity -= 1;
		}
		else
		{
			// try to rotate the item and see if it fits
			if (Item->CanBeRotated())
			{
				NewItemInstance->Rotate();
				const FPoint2D CoordsWhereItemCanFitRotated = GetFreeCellWhereItemCanFit(NewItemInstance->SizeInCells);
	
				if (IsWithinBoundaries(CoordsWhereItemCanFitRotated) && CanCarryItem(Item, 1))
				{
					NewItemInstance->TopLeftCoordinates = CoordsWhereItemCanFitRotated;
	
					const FSlot NewSlot = FSlot(NewItemInstance, 1, this);
					Slots.Add(NewSlot);
	
					AddedQuantity += 1;
					RemainingQuantity -= 1;
				}
				else
				{
					NotifyInventoryInsufficientSpace();
					return false;
				}
			}
			else
			{
				NotifyInventoryInsufficientSpace();
				return false;
			}
		}
	}

	return true;
}

void UInventoryComponent::NotifyInventoryInitialized()
{
	OnInventoryInitialized.Broadcast();
	K2_OnInventoryInitialized();
}

void UInventoryComponent::NotifyInventoryUpdated()
{
	OnInventoryUpdated.Broadcast();
	K2_OnInventoryUpdated();
}

void UInventoryComponent::NotifyInventoryInsufficientSpace()
{
	OnInsufficientSpace.Broadcast();
	K2_OnInventoryInsufficientSpace();
}

void UInventoryComponent::NotifyInventoryWeightChanged() 
{
	OnWeightChanged.Broadcast();
	K2_OnInventoryWeightChanged();
}

void UInventoryComponent::NotifyInventoryItemAdded(UItem* InItem, const int32 InQuantity)
{
	OnItemAdded.Broadcast(InItem, InQuantity);
	K2_OnInventoryItemAdded(InItem, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemRemoved(UItem* InItem, const int32 InQuantity)
{
	OnItemRemoved.Broadcast(InItem, InQuantity);
	K2_OnInventoryItemRemoved(InItem, InQuantity);
}

void UInventoryComponent::NotifyMoneyChanged()
{
	OnMoneyChanged.Broadcast();
	K2_OnMoneyChanged();
}

void UInventoryComponent::NotifyInventoryItemEquipped(UItem* InItem, const int32 InQuantity)
{
	OnItemEquipped.Broadcast(InItem, InQuantity);
	K2_OnInventoryItemEquipped(InItem, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemUnequipped(UItem* InItem, const int32 InQuantity)
{
	OnItemUnequipped.Broadcast(InItem, InQuantity);
	K2_OnInventoryItemUnequipped(InItem, InQuantity);
}

void UInventoryComponent::NotifyInventoryItemUsed(UItem* InItem, const int32 InQuantity)
{
	OnItemUsed.Broadcast(InItem, InQuantity);
	K2_OnInventoryItemUsed(InItem, InQuantity);
}
