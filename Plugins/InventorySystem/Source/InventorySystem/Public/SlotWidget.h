// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "SlotWidget.generated.h"

class UGridWidget;
class UDraggedSlotWidget;

/**
 * USlotWidget
 */
UCLASS()
class INVENTORYSYSTEM_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	USlotWidget(const FObjectInitializer& ObjectInitializer);


	UFUNCTION(BlueprintCallable, Category = "Slot")
	void SetSlotData(const FSlot& InInventorySlot, UGridWidget* InParentWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotDataReceived();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void SetSlotSize(const float NewSize);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void SetSlotColor(const FSlateBrush& NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void OnDragStarted();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Slot")
	void OnDragCompleted(bool bCancelled);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void NativeOnSlotLeftClick();
	void NativeOnSlotRightClick();
	

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotRightClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotLeftClick();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	UGridWidget* ParentWidget;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	FSlot InventorySlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush DefaultColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush HoveredColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FSlateBrush ClickedColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	FSlateBrush LastStateColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Slot")
	uint8 bMouseWasDragging : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	TSubclassOf<UDraggedSlotWidget> DraggedSlotWidgetClass;
	
};
