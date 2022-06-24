// Fill out your copyright notice in the Description page of Project Settings.

#include "BackgroundWidget.h"
#include "CellWidget.h"
#include "GridWidget.h"
#include "DraggedSlotWidget.h"
#include "SlotWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"

UBackgroundWidget::UBackgroundWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

bool UBackgroundWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	
	for (UCellWidget* CellWidget: DraggedSlotWidget->ParentWidget->CellsWidgets)
	{
		CellWidget->SetCellColor(CellWidget->DefaultColor);
	}

	for (USlotWidget* SlotWidget: DraggedSlotWidget->ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(1);
		}
	}

	DraggedSlotWidget->ParentWidget->Inventory->DropItemOnSlot(DraggedSlotWidget->InventorySlot);

	return true;
}
