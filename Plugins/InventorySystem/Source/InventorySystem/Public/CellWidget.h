// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "CellWidget.generated.h"

class UGridWidget;

/**
 * UCellWidget
 */
UCLASS()
class INVENTORYSYSTEM_API UCellWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UCellWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Cell")
	void SetCellData(const FPoint2D& InCoordinates, float InSize, UGridWidget* InParentWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void OnCellDataReceived();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Grid")
	void SetCellColor(const FSlateBrush& NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Grid")
	void SetCellSize(const float NewSize);

	UFUNCTION()
	void OnItemRotated();
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Cell")
	FPoint2D Coordinates;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Cell")
	float CellSize;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Cell")
	UGridWidget* ParentWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	FSlateBrush DefaultColor;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	FSlateBrush HoveredColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	FSlateBrush ClickedColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Cell")
	FSlateBrush LastStateColor;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Cell")
	uint8 bMouseWasDragging : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	FSlateBrush ValidPlacementColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	FSlateBrush InvalidPlacementColor;

	UPROPERTY(Transient)
	UDragDropOperation* CachedDragDropOperation;


	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
};
