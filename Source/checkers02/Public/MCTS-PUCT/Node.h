// Copyright Stephen Montague.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Node.generated.h"


/**
 * Node class to support Monte Carlo Tree Search.
 */
UCLASS()
class CHECKERS02_API USearchNode : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FString& InBoard, const bool InIsPlayer1,
		const uint32 InVisitCount = 0, const uint32 InSumValue = 0,
		TObjectPtr<USearchNode> const InParent = nullptr, bool InIsProvenWinner = false);

	TObjectPtr<USearchNode> Clone();
	
	FString Board;
	bool bIsPlayer1;
	uint32 VisitCount;
	uint32 SumValue;
	TObjectPtr<USearchNode> Parent;
	uint32 Depth; 
	TArray<TObjectPtr<USearchNode>> Children; 
	bool bIsProvenWinner;
};
