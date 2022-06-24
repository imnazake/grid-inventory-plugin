// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentSlotWidget.h"

UEquipmentSlotWidget::UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	EquipmentSlotType = EEquipmentSlotType::None;
}

void UEquipmentSlotWidget::SetEquipmentSlotData(const FEquipmentSlot& InEquipmentSlot)
{
	EquipmentSlot = InEquipmentSlot;
	OnEquipmentSlotDataReceived();
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bMouseWasDragging = false;
		SetSlotColor(ClickedColor);
		NativeOnSlotLeftClick();
		OnSlotLeftClick();
		return FReply::Handled();
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

FReply UEquipmentSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseWasDragging = false;
	SetSlotColor(LastStateColor);
	return FReply::Handled();
}

void UEquipmentSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SetSlotColor(HoveredColor);
	LastStateColor = HoveredColor;
}

void UEquipmentSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SetSlotColor(DefaultColor);
	LastStateColor = DefaultColor;
}

void UEquipmentSlotWidget::NativeOnSlotLeftClick()
{
	
}

void UEquipmentSlotWidget::NativeOnSlotRightClick()
{
	
}
