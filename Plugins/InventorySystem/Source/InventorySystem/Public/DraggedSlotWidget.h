// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "DraggedSlotWidget.generated.h"

class UGridWidget;

/**
 * UDraggedSlotWidget
 */
UCLASS()
class INVENTORYSYSTEM_API UDraggedSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UDraggedSlotWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "DraggedSlot")
	void SetDraggedSlotData(const FSlot& InSlot, UGridWidget* InParentWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DraggedSlot")
	void OnDraggedSlotDataReceived();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DraggedSlot")
	void SetDraggedSlotSize(float NewSize);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "DraggedSlot")
	void OnRotate();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "DraggedSlot")
	UGridWidget* ParentWidget;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "DraggedSlot")
	FSlot InventorySlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FName RotateInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush ValidPlacementColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DraggedSlot")
	FSlateBrush InvalidPlacementColor;
	
	UFUNCTION()
	void OnRotateItem();
	
	FOnInputAction RotateItemCallback;
	
};
