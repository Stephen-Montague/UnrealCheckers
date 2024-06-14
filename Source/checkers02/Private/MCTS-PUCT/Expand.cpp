#include "MCTS-PUCT/Expand.h"
#include "MCTS-PUCT/Node.h"
#include "CheckersRules.h"
#include "MCTS-PUCT/Backpropagate.h"
#include "MCTS-PUCT/SearchSetup.h"


void Expand(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules)
{
	if (HasNextState(Node, Rules))
	{
		Node->Children.Reserve(Rules->NextPossibleBoards.Num());
		for (const FString& Board : Rules->NextPossibleBoards)
		{
			if (TObjectPtr<USearchNode> Child = NewObject<USearchNode>())
			{
				Child->Init(Board, !Node->bIsPlayer1, 0, 0, Node);
				Node->Children.Emplace(Child);
			}
		}
	}
	else
	{
		HandleLeaf(Node, Rules);
	}
}

void HandleLeaf(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules)
{
	uint32 Result = 0;
	if (IsTie(Rules))
	{
		Result = TieReward;
	}
	else if (IsWin(Node, Rules))
	{
		Result = CalculateProvenWinReward(Node->Depth);
		Node->bIsProvenWinner = true;
	}
	Node->SumValue += Result;
	Node->VisitCount++;
	BackPropagate(Node, Result);
}

bool HasNextState(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules)
{
	return (Rules->GenerateNextPossibleBoards(Node->Board, Node->bIsPlayer1));
}

bool IsTie(const TObjectPtr<UCheckersRules> Rules)
{
	return (Rules->GetWinner() == EWinner::ENone);
}

bool IsWin(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules)
{
	return (Rules->GetWinner() == EWinner::EPlayer1 && Node->Parent->bIsPlayer1 ||
		Rules->GetWinner() == EWinner::EPlayer2 && !Node->Parent->bIsPlayer1);
}

/** An exponential decay function for rewards by Depth, from Depth 1 : 1 million to Depth > 9.
 These large rewards may help to differentiate between a winning game vs a won game.
 Basically, a proven win should be worth more than a win guess after simulation.
 Likewise, a near win should be worth more than a distant win.
 Otherwise, a winning depth-limited agent may be happy to push pieces in a circle.
*/ 
uint32 CalculateProvenWinReward(uint32 Depth)
{
	switch(Depth)
	{
	// case 1: Doesn't need a reward, since immediate wins are always chosen.
	case 2:
		return 400000;
	case 3:
		return 150000;
	case 4:
		return 60000;
	case 5:
		return 20000;
	case 6:
		return 6000;
	case 7:
		return 1500;
	case 8:
		return 400;
	case 9:
		return 120;
	default: 
		return 30;
	}
}
