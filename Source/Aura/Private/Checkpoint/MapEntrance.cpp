// Copyright Jordan McWilliams


#include "Checkpoint/MapEntrance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/GameModeInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance()
{
	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	Mesh->SetupAttachment(MoveToComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCustomDepthStencilValue(CustomDepthRenderValue);
	Mesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(MoveToComponent);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMapEntrance::HighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(true);
}

bool AMapEntrance::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = Sphere->GetComponentLocation();
	return true;
}

void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMapEntrance::OnSphereOverlap);
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (bEndGameInstead)
		{
			EndGameScreen(OtherActor);
			return;
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		/* if (DestinationMap == nullptr)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("Map Entrance doesn't have a destination map set!"));
			return;
		} */
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance()))
			{
				AuraGameInstance->LevelName = AuraGameModeBase->GetMapNameFromMapAssetName(DestinationMap.GetAssetName());
				AuraGameModeBase->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
			}
		}
		//test
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void AMapEntrance::EndGameScreen(AActor* OtherActor) const
{
	UUserWidget* EndScreenWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(this, 0), EndScreenWidgetClass);
	EndScreenWidget->AddToViewport();
	UGameplayStatics::PlaySound2D(this, EndScreenSound);

	//Block player input
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
		const FGameplayTagContainer BlockedTags = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({
		AuraTags.Player_Block_CursorTrace,
		AuraTags.Player_Block_InputHeld,
		AuraTags.Player_Block_InputPressed,
		AuraTags.Player_Block_InputReleased}));
		ASC->AddLooseGameplayTags(BlockedTags);
	}
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
