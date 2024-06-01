// Copyright Stephen Montague.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MonteCarloTreeSearch.generated.h"

class UCheckersRules;
class ACheckersGame;
class USearchNode;

/**
 * Algorithm for Monte Carlo Tree Search (MCTS) with Predictive Upper Confidence for Trees (PUCT).
 */
UCLASS()
class CHECKERS02_API UMonteCarloTreeSearch : public UObject
{
	GENERATED_BODY()
public:
	UMonteCarloTreeSearch();
	void Init(const FString& Board, const TObjectPtr<UCheckersRules> GameRules, bool IsPlayer1);
	FString ChooseNextBoard() const;

private:
	void ExpandRoot(const TArray<FString>& NextPossibleBoards) const;
	bool HasTimeToThink() const;
	bool HasMoreIterations() const;
	FString GetBestBoard() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCheckersRules> SimulationRules;

	TObjectPtr<USearchNode> RootNode;
	const double SearchTime = 1.8;
	double EndSearchTime;
};
