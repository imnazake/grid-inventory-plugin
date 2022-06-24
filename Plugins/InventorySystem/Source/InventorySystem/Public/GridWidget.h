// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "GridWidget.generated.h"

class UCellWidget;
class USlotWidget;
class UInventoryComponent;

/**
 * UGridWidget
 */
UCLASS()
class INVENTORYSYSTEM_API UGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UGridWidget(const FObjectInitializer& ObjectInitializer);


	void NativeOnInventoryDataReceived();

	int32 GetCellIndex(const FPoint2D& Coordinates);
	
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetInventoryData(UInventoryComponent* NewInventory);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnPrePopulateData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnInventoryDataReceived();

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnCellWidgetCreated(UCellWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnSlotWidgetCreated(USlotWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnSlotWidgetRemoved(USlotWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void SetWeight(float CurrentWeight, float MaxWeight);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<UCellWidget> CellWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<USlotWidget> SlotWidgetClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	TArray<USlotWidget*> SlotsWidgets;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	TArray<UCellWidget*> CellsWidgets;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Grid")
	UInventoryComponent* Inventory;


	UFUNCTION()
	void OnInventoryUpdated();
	
	UFUNCTION()
	void OnInventoryWeightChanged();
	
};
