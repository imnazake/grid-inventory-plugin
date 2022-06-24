// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "ItemInstance.h"
#include "Item.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;

	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetMassOverrideInKg(NAME_None, 100.0f, true);
	PickupMesh->SetEnableGravity(true);
	PickupMesh->SetConstraintMode(EDOFMode::Default);
	PickupMesh->SetGenerateOverlapEvents(true);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	PickupMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickup::OnPickupDataReceived() const
{
	if (ItemInstance->Item->PickupStaticMesh)
	{
		PickupMesh->SetStaticMesh(ItemInstance->Item->PickupStaticMesh);
	}
}

void APickup::SetPickupData(UItemInstance* InItemInstance, const int32 InQuantity)
{
	ItemInstance = InItemInstance;
	Quantity = InQuantity;

	OnPickupDataReceived();
	K2_OnPickupDataReceived();
}
