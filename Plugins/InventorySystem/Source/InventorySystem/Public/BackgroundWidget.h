// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackgroundWidget.generated.h"

/**
 * UBackgroundWidget
 */
UCLASS()
class INVENTORYSYSTEM_API UBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UBackgroundWidget(const FObjectInitializer& ObjectInitializer);


	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;	
	
};
