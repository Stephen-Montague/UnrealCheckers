// Copyright Stephen Montague.

#include "MCTS-PUCT/Simulate.h"
#include "MCTS-PUCT/Node.h"
#include "MCTS-PUCT/SearchSetup.h"
#include "CheckersRules.h"


uint32 Simulate(TObjectPtr<USearchNode> Child, TObjectPtr<UCheckersRules> SimRules)
{
	uint32 Result = 0;
	const EWinner Winner = GetSimulationWinner(Child->Board, Child->bIsPlayer1, SimRules);
	if (Winner == EWinner::ENone)
	{
		Result = TieReward;
	}
	else if (Winner == EWinner::EPlayer1 && Child->Parent->bIsPlayer1 ||
		Winner == EWinner::EPlayer2 && !Child->Parent->bIsPlayer1)
	{
		Result = WinReward;
	}
	Child->SumValue += Result;
	Child->VisitCount++;
	return Result;
}

EWinner GetSimulationWinner(const FString& Board, const bool bIsPlayer1, TObjectPtr<UCheckersRules> SimRules)
{
	// Game loop for sim
	uint32 Depth = 0;
	FString SimBoard = Board;
	bool bSimPlayer1 = bIsPlayer1;
	while(true)
	{
		const bool HasNextPossibleBoard = SimRules->GenerateNextPossibleBoards(SimBoard, bSimPlayer1);
		if (!HasNextPossibleBoard)
		{
			return SimRules->GetWinner();
		}
		if (Depth < SimDepthLimit)
		{
			SimBoard = GetPredictedNextBoard(bSimPlayer1, SimRules);
			bSimPlayer1 = !bSimPlayer1;
			Depth++;
		}
		else 
		{
			return SimRules->GetPredictedWinner(SimBoard, bSimPlayer1);
		}
	}
}

FString GetPredictedNextBoard(const bool bSimPlayer1, const TObjectPtr<UCheckersRules> SimRules)
{
	// Choose the board that has the least predicted chance for the opponent to win.
	float BestPrediction = FLT_MAX;
	TArray<FString> BestBoards;
	for (const FString& NextPossibleBoard : SimRules->NextPossibleBoards)
	{
		const float Prediction = SimRules->GetWinProbability(NextPossibleBoard, !bSimPlayer1);
		if (Prediction < BestPrediction)
		{
			BestPrediction = Prediction;
			BestBoards.Reset();
			BestBoards.Emplace(NextPossibleBoard);
		}
		else if (Prediction == BestPrediction)
		{
			BestBoards.Emplace(NextPossibleBoard);
		}
	}
	// Return the best, or if multiple best, randomly choose one of the best.
	return (BestBoards.Num() > 1) ? BestBoards[FMath::RandHelper(BestBoards.Num())] : BestBoards[0];
}
