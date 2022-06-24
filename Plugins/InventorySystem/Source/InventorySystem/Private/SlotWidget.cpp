// Fill out your copyright notice in the Description page of Project Settings.

#include "SlotWidget.h"

#include "CellWidget.h"
#include "DraggedSlotWidget.h"
#include "GridWidget.h"
#include "ItemInstance.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"

USlotWidget::USlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void USlotWidget::SetSlotData(const FSlot& InInventorySlot, UGridWidget* InParentWidget)
{
	InventorySlot = InInventorySlot;
	ParentWidget = InParentWidget;
	
	OnSlotDataReceived();
}

FReply USlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FReply Reply = FReply::Handled();
		bMouseWasDragging = false;
		SetSlotColor(ClickedColor);
		NativeOnSlotLeftClick();
		OnSlotLeftClick();
		
		const TSharedPtr<SWidget> WidgetDetectingDrag = GetCachedWidget();
		Reply.DetectDrag(WidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);	
		return Reply;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bMouseWasDragging = false;
		SetSlotColor(ClickedColor);
		NativeOnSlotRightClick();
		OnSlotRightClick();
		return FReply::Handled();
	}
	else
	{
		bMouseWasDragging = false;
		return FReply::Handled();
	}
}

FReply USlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseWasDragging = false;
	SetSlotColor(LastStateColor);
	return FReply::Handled();
}

void USlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SetSlotColor(HoveredColor);
	LastStateColor = HoveredColor;
}

void USlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SetSlotColor(DefaultColor);
	LastStateColor = DefaultColor;
}

void USlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	SetSlotColor(DefaultColor);
	LastStateColor = DefaultColor;

	for (USlotWidget* SlotWidget: ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(1);
		}
	}

	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	ParentWidget->Inventory->Slots.Add(DraggedSlotWidget->InventorySlot);

	OnDragCompleted(true);
}

void USlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	bMouseWasDragging = true;

	for (USlotWidget* SlotWidget: ParentWidget->SlotsWidgets)
	{
		UGridSlot* GridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(SlotWidget);
		if (GridSlot)
		{
			GridSlot->SetLayer(-1);
		}
	}

	OnDragStarted();

	UDraggedSlotWidget* DraggedSlotWidget = CreateWidget<UDraggedSlotWidget>(GetOwningPlayer(), DraggedSlotWidgetClass);
	check(DraggedSlotWidget != nullptr);

	DraggedSlotWidget->SetDraggedSlotData(InventorySlot, ParentWidget);
	DraggedSlotWidget->SetDraggedSlotSize(ParentWidget->Inventory->CellSize);

	UDragDropOperation* DragDropOperation = NewObject<UDragDropOperation>(GetOwningPlayer());
	check(DragDropOperation != nullptr);

	DragDropOperation->DefaultDragVisual = DraggedSlotWidget;
	DragDropOperation->Pivot = EDragPivot::TopLeft;

	ParentWidget->Inventory->Slots.Remove(InventorySlot);
	OutOperation = DragDropOperation;
}

bool USlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bMouseWasDragging = false;

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
	
	const UDraggedSlotWidget* DraggedSlotWidget = Cast<UDraggedSlotWidget>(InOperation->DefaultDragVisual);
	//ParentWidget->Inventory->Slots.Add(DraggedSlotWidget->InventorySlot);

	ParentWidget->Inventory->StackItemStackOnSlot(DraggedSlotWidget->InventorySlot, InventorySlot.ItemInstance->TopLeftCoordinates, DraggedSlotWidget->InventorySlot.Quantity);

	OnDragCompleted(false);
	return true;
}

void USlotWidget::NativeOnSlotLeftClick()
{
}

void USlotWidget::NativeOnSlotRightClick()
{
}
