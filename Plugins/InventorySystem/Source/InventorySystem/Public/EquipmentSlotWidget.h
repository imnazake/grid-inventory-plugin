// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlotWidget.generated.h"

/**
 * UEquipmentSlotWidget
 */
UCLASS()
class INVENTORYSYSTEM_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer);


	UFUNCTION(BlueprintCallable, Category = "EquipmentSlot")
	void SetEquipmentSlotData(const FEquipmentSlot& InEquipmentSlot);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "EquipmentSlot")
	void OnEquipmentSlotDataReceived();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void SetSlotColor(const FSlateBrush& NewColor);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void NativeOnSlotLeftClick();
	void NativeOnSlotRightClick();
	

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotRightClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotLeftClick();

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true), Category = "EquipmentSlot")
	EEquipmentSlotType EquipmentSlotType;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "EquipmentSlot")
	FEquipmentSlot EquipmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentSlot")
	FSlateBrush DefaultColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentSlot")
	FSlateBrush HoveredColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentSlot")
	FSlateBrush ClickedColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "EquipmentSlot")
	FSlateBrush LastStateColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "EquipmentSlot")
	uint8 bMouseWasDragging : 1;
	
};
