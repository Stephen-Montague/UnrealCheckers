// Copyright Stephen Montague.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UObject/Object.h"
#include "CheckersAgent.generated.h"

class UMonteCarloTreeSearch;
class ACheckersGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveChosenMoves, uint8, OriginIndex, const TArray<uint8>&, MoveIndices);

/**
 * An AI agent that has the logic to begin and continue playing a checkers game. 
 */
UCLASS(Blueprintable, BlueprintType)
class CHECKERS02_API ACheckersAgent : public AAIController
{
	GENERATED_BODY()
public:
	ACheckersAgent();

	UFUNCTION(BlueprintCallable)
	void RunAI(ACheckersGame* const CheckersGame, const FString& CurrentBoard, const bool IsPlayer1);

	void PlayNextTurn();
	void ChooseNextMoves();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static AActor* FindNearestActorIndex(FVector Origin, const TArray<AActor*>& ActorsToCheck, int32& NearestActorIndex);

private:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess="true"))
	FReceiveChosenMoves ReceiveChosenMoves;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString LogName = TEXT("Checkers Agent");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMonteCarloTreeSearch> Logic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ACheckersGame> Game;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bIsPlayer1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	int32 WinCount;

	uint8 Origin;
	TArray<uint8> Movements;
};
