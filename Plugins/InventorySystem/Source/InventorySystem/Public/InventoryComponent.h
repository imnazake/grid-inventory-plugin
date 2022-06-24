// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItem;
class UItemInstance;
class UInventoryComponent;

/**
 * Point2D 
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FPoint2D
{
	GENERATED_BODY()

	FPoint2D()
	{
		X = 0;
		Y = 0;
	}

	FPoint2D(const int32 InX, const int32 InY)
	{
		X = InX;
		Y = InY;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	int32 Y;

	bool operator == (const FPoint2D& Other) const
	{
		return Other.X == X && Other.Y == Y;
	}

	FPoint2D operator + (const FPoint2D& Other) const
	{
		return FPoint2D(Other.X + X, Other.Y + Y);
	}

	bool operator > (const FPoint2D& Other) const
	{
		return Other.X > X && Other.Y > Y;
	}

	bool operator >= (const FPoint2D& Other) const
	{
		return Other.X >= X && Other.Y >= Y;
	}

	bool operator < (const FPoint2D& Other) const
	{
		return (Other.X < X && Other.Y < Y);
	}

	bool operator <= (const FPoint2D& Other) const
	{
		return (Other.X <= X && Other.Y <= Y);
	}
};

/**
 * Slot
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FSlot
{
	GENERATED_BODY()

	FSlot()
	{
		OwnerInventory = nullptr;
		ItemInstance = nullptr;
		Quantity = 0;
	}

	FSlot(UItemInstance* InItemInstance, const int32 InQuantity, UInventoryComponent* InOwnerInventory)
	{
		OwnerInventory = InOwnerInventory;
		ItemInstance = InItemInstance;
		Quantity = InQuantity;
	}

	UPROPERTY(BlueprintReadOnly)
	UItemInstance* ItemInstance;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity;

	UPROPERTY(BlueprintReadOnly)
	UInventoryComponent* OwnerInventory;
	

	bool operator == (const FSlot& Other) const
	{
		return Other.ItemInstance == ItemInstance && Other.Quantity == Quantity;
	}

	bool operator != (const FSlot& Other) const
	{
		return Other.ItemInstance != ItemInstance && Other.Quantity != Quantity;
	}
	
	bool IsOnMaxStackSize() const;
	int32 GetMissingStackQuantity() const;

	void SetQuantity(int32 InQuantity);
	void UpdateQuantity(int32 InQuantity);

	bool IsEmpty() const;
	bool IsOccupied() const;
	bool IsValid() const;
	
};

/**
 * Startup Item
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FStartupItem
{
	GENERATED_BODY()

	FStartupItem()
	{
		Item = nullptr;
		Quantity = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItem* Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	int32 Quantity;
	
	bool operator == (const FStartupItem& Other) const
	{
		return Other.Item == Item && Other.Quantity == Quantity;
	}

	bool operator != (const FStartupItem& Other) const
	{
		return Other.Item != Item && Other.Quantity != Quantity;
	}
	
};

/**
 * ItemType
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None								UMETA(DisplayName = "None"),
	Equipment							UMETA(DisplayName = "Equipment"),
	Consumable							UMETA(DisplayName = "Consumable"),
	Ammunition							UMETA(DisplayName = "Ammunition"),
};

/**
 * EquipmentSlotType
 */
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	None								UMETA(DisplayName = "None"),
	PrimaryWeapon						UMETA(DisplayName = "PrimaryWeapon"),
	SecondaryWeapon						UMETA(DisplayName = "SecondaryWeapon"),

};

/**
 * Startup Item
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot()
	{
		Type = EEquipmentSlotType::None;
		Data = FSlot();
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType Type;
	
	UPROPERTY(BlueprintReadWrite)
	FSlot Data;
	
	bool IsValid() const
	{
		return Type != EEquipmentSlotType::None;
	}
};

/**
 * Delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemEvent, UItem*, Item, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEquipmentEvent, UItem*, Item, int32, Quantity);

/**
 * UInventoryComponent
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInventoryComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* CreateItemInstance(TSubclassOf<UItemInstance> ItemInstanceClass) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsWithinBoundaries(const FPoint2D& Coordinates) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsFreeCell(const FPoint2D& Coordinates);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool DoesItemFit(const TArray<FPoint2D>& SizeInCells, const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FPoint2D GetFreeCell();
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FPoint2D GetFreeCellWhereItemCanFit(const TArray<FPoint2D>& SizeInCells);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsFull() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool DoesItemExist(const UItem* Item);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 CountItemQuantity(const UItem* Item);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FSlot GetSlotByCoordinates(const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotIndexByCoordinates(const FPoint2D& Coordinates);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanCarryItem(const UItem* Item, const int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddStartupItems();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddNewItem(UItem* Item, int32 Quantity, int32& AddedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddExistingItem(UItemInstance* ItemInstance, int32 Quantity, int32& AddedQuantity);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItem* Item, int32 Quantity, int32& RemovedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemOnSlot(const FSlot& Slot, int32 Quantity, int32& RemovedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItemOnSlot(const FSlot& Slot, const FPoint2D& Destination);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void StackItemStackOnSlot(const FSlot& Slot, const FPoint2D& Destination, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItemOnSlot(const FSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnequipItem(EEquipmentSlotType EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItemOnSlot(const FSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool LootItem(class APickup* Pickup, int32& LootedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SpawnItem(const UItem* Item, int32 Quantity, const FTransform& Transform);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItemOnSlot(const FSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddMoney(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveMoney(int32 Value);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetMoney() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsValidEquipmentSlots();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FEquipmentSlot GetEquipmentSlotByType(EEquipmentSlotType SlotType);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetEquipmentSlotIndexByType(EEquipmentSlotType SlotType);


	/** Internal functions used in native code (c++ only) */
	bool AddExistingItem_Internal(const UItemInstance* ItemInstance, int32 Quantity, int32& AddedQuantity);
	// void RemoveItemOnSlot_Internal();
	// void RemoveItem_Internal();
	
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Initialized"), Category = "Inventory")
	void K2_OnInventoryInitialized();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Money Changed"), Category = "Inventory")
	void K2_OnMoneyChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Updated"), Category = "Inventory")
	void K2_OnInventoryUpdated();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Insufficient Space"), Category = "Inventory")
	void K2_OnInventoryInsufficientSpace();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Weight Changed"), Category = "Inventory")
	void K2_OnInventoryWeightChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Added"), Category = "Inventory")
	void K2_OnInventoryItemAdded(UItem* Item, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Removed"), Category = "Inventory")
	void K2_OnInventoryItemRemoved(UItem* Item, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Equipped"), Category = "Inventory")
	void K2_OnInventoryItemEquipped(UItem* Item, int32 Quantity);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Unequipped"), Category = "Inventory")
	void K2_OnInventoryItemUnequipped(UItem* Item, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Item Used"), Category = "Inventory")
	void K2_OnInventoryItemUsed(UItem* Item, int32 Quantity);

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FPoint2D GridSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1.0f, UIMin = 1.0f), Category = "Inventory")
	float CellSize;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FPoint2D> Cells;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FSlot> Slots;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	float CurrentWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	uint8 bUseScaledMaxWeight : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1.0f, UIMin = 1.0f, EditCondition = "!bUseScaledMaxWeight"), Category = "Inventory")
	float MaxWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0), Category = "Inventory")
	int32 DefaultMoney;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, UIMin = 1), Category = "Inventory")
	float PickupSpawnRadiusFromPlayer;

	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0), Category = "Inventory")
	int32 Money;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TArray<FStartupItem> StartupItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TArray<FEquipmentSlot> EquipmentSlots;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInventoryInitialized;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnInsufficientSpace;

	UPROPERTY(BlueprintAssignable)	
	FInventoryEvent OnWeightChanged;

	UPROPERTY(BlueprintAssignable)	
	FInventoryItemEvent OnItemAdded;

	UPROPERTY(BlueprintAssignable)	
	FInventoryItemEvent OnItemRemoved;

	UPROPERTY(BlueprintAssignable)
	FInventoryEvent OnMoneyChanged;

	UPROPERTY(BlueprintAssignable)
	FInventoryEquipmentEvent OnItemEquipped;

	UPROPERTY(BlueprintAssignable)
	FInventoryEquipmentEvent OnItemUnequipped;

	UPROPERTY(BlueprintAssignable)
	FInventoryItemEvent OnItemUsed;


	void NotifyInventoryInitialized();
	void NotifyInventoryUpdated();
	void NotifyInventoryInsufficientSpace();
	void NotifyInventoryWeightChanged();
	void NotifyInventoryItemAdded(UItem* InItem, int32 InQuantity);
	void NotifyInventoryItemRemoved(UItem* InItem, int32 InQuantity);
	void NotifyMoneyChanged();
	void NotifyInventoryItemEquipped(UItem* InItem, int32 InQuantity);
	void NotifyInventoryItemUnequipped(UItem* InItem, int32 InQuantity);
	void NotifyInventoryItemUsed(UItem* InItem, int32 InQuantity);
	
};
