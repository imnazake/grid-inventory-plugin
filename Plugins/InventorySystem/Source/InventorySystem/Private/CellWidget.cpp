// Fill out your copyright notice in the Description page of Project Settings.

#include "CellWidget.h"
#include "DraggedSlotWidget.h"
#include "GridWidget.h"
#include "ItemInstance.h"
#include "SlotWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"

UCellWidget::UCellWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UCellWidget::SetCellData(const FPoint2D& InCoordinates, const float InSize, UGridWidget* InParentWidget)
{
	ParentWidget = InParentWidget;
	Coordinates = InCoordinates;
	CellSize = InSize;

	OnCellDataReceived();
}

void UCellWidget::OnItemRotated()
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(CachedDragDropOperation->DefaultDragVisual);
	
	// if (ParentWidget->Inventory->IsFreeCell(Coordinates))
	// {
	// 	if (ParentWidget->Inventory->DoesItemFit(DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells, Coordinates))
	// 	{
	// 		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
	// 		{
	// 			FPoint2D TargetCell = Cell + Coordinates;
	// 			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);
	//
	// 			if (CellIndex > INDEX_NONE)
	// 			{
	// 				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(ValidPlacementColor);
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
	// 		{
	// 			FPoint2D TargetCell = Cell + Coordinates;
	// 			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);
	//
	// 			if (CellIndex > INDEX_NONE)
	// 			{
	// 				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(InvalidPlacementColor);
	// 			}
	// 		}
	// 	}
	// }

	if (ParentWidget->Inventory->DoesItemFit(DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells, Coordinates))
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(ValidPlacementColor);
			}
		}
	}
	else
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(InvalidPlacementColor);
			}
		}
	}
	
}

FReply UCellWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bMouseWasDragging = false;
		SetCellColor(ClickedColor);
		return FReply::Handled();
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bMouseWasDragging = false;
		SetCellColor(ClickedColor);
		return FReply::Handled();
	}
	else
	{
		bMouseWasDragging = false;
		return FReply::Handled();
	}
}

FReply UCellWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseWasDragging = false;
	SetCellColor(LastStateColor);
	return FReply::Handled();
}

void UCellWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SetCellColor(HoveredColor);
	LastStateColor = HoveredColor;
}

void UCellWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SetCellColor(DefaultColor);
	LastStateColor = DefaultColor;
}

void UCellWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	CachedDragDropOperation = InOperation;

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	DraggedSlotWidget->InventorySlot.ItemInstance->OnItemRotated.AddDynamic(this, &ThisClass::OnItemRotated);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	if (ParentWidget->Inventory->IsFreeCell(Coordinates))
	{
		
	}

	if (ParentWidget->Inventory->DoesItemFit(DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells, Coordinates))
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(ValidPlacementColor);
			}
		}
	}
	else
	{
		for (const FPoint2D& Cell: DraggedSlotWidget->InventorySlot.ItemInstance->SizeInCells)
		{
			FPoint2D TargetCell = Cell + Coordinates;
			const int32 CellIndex = ParentWidget->GetCellIndex(TargetCell);

			if (CellIndex > INDEX_NONE)
			{
				ParentWidget->CellsWidgets[CellIndex]->SetCellColor(InvalidPlacementColor);
			}
		}
	}
}

void UCellWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	DraggedSlotWidget->InventorySlot.ItemInstance->OnItemRotated.RemoveAll(this);
	//ParentWidget->Inventory->Slots.Add(DraggedSlotWidget->InventorySlot); //TODO: Wtf is this?
}

void UCellWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}
	
	DraggedSlotWidget->InventorySlot.ItemInstance->OnItemRotated.RemoveAll(this);
	ParentWidget->Inventory->Slots.Add(DraggedSlotWidget->InventorySlot);
}

bool UCellWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);

	for (UCellWidget* CellWidget: ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	for (USlotWidget* SlotWidget: ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(1);
		}
	}

	DraggedSlotWidget->InventorySlot.ItemInstance->OnItemRotated.RemoveAll(this);
	
	ParentWidget->Inventory->MoveItemOnSlot(DraggedSlotWidget->InventorySlot, Coordinates);
	
	return true;
}
