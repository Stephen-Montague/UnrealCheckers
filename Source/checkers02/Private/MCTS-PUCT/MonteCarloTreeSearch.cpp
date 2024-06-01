// Copyright Stephen Montague.


#include "MCTS-PUCT/MonteCarloTreeSearch.h"
#include "MCTS-PUCT/SearchSetup.h"
#include "MCTS-PUCT/Node.h"
#include "MCTS-PUCT/SelectNode.h"
#include "MCTS-PUCT/Expand.h"
#include "MCTS-PUCT/Simulate.h"
#include "MCTS-PUCT/Backpropagate.h"
#include "CheckersRules.h"

UMonteCarloTreeSearch::UMonteCarloTreeSearch(): EndSearchTime(0.0)
{
	SimulationRules = CreateDefaultSubobject<UCheckersRules>(TEXT("CheckersRules"));
	RootNode = CreateDefaultSubobject<USearchNode>(TEXT("RootNode"));
}

void UMonteCarloTreeSearch::Init(const FString& Board, const TObjectPtr<UCheckersRules> GameRules, const bool IsPlayer1)
{
	EndSearchTime = FPlatformTime::Seconds() + SearchTime;
	RootNode->Init(Board, IsPlayer1);
	ExpandRoot(GameRules->NextPossibleBoards);
	for (const TObjectPtr<USearchNode> Child : RootNode->Children)
	{
		const uint32 Result = Simulate(Child, SimulationRules);
		BackPropagate(Child, Result);
	}
}

void UMonteCarloTreeSearch::ExpandRoot(const TArray<FString>& NextPossibleBoards) const
{
	for (const FString& Board : NextPossibleBoards)
	{
		TObjectPtr<USearchNode> Node = NewObject<USearchNode>();
		Node->Init(Board, !RootNode->bIsPlayer1, 0, 0, RootNode);
		RootNode->Children.Emplace(Node);
	}
}

FString UMonteCarloTreeSearch::ChooseNextBoard() const
{
	while (HasTimeToThink() && HasMoreIterations())
	{
		const TObjectPtr<USearchNode> NodeToVisit = SelectNode(RootNode, SimulationRules);
		if (NodeToVisit->Depth < SimDepthLimit)
		{
			Expand(NodeToVisit, SimulationRules);
			for (const TObjectPtr<USearchNode> Child : NodeToVisit->Children)
			{
				if (Child->VisitCount == 0)
				{
					const uint32 Result = Simulate(Child, SimulationRules);
					BackPropagate(Child, Result);
					break;
				}
			}
		}
		else
		{
			// At tree depth limit, don't expand - just simulate the node.
			const uint32 Result = Simulate(NodeToVisit, SimulationRules);
			BackPropagate(NodeToVisit, Result);
		}
	}
	return GetBestBoard();;
}

bool UMonteCarloTreeSearch::HasTimeToThink() const
{
	return (FPlatformTime::Seconds() < EndSearchTime);
}

bool UMonteCarloTreeSearch::HasMoreIterations() const
{
	return (RootNode->VisitCount < MaxIterations);
}

FString UMonteCarloTreeSearch::GetBestBoard() const
{
	TObjectPtr<USearchNode> BestChild;
	uint32 BestVisitCount = 0;

	for (const TObjectPtr<USearchNode> Child : RootNode->Children)
	{
		if (Child->bIsProvenWinner == true)
		{
			BestChild = Child;
			break;
		}
		if (Child->VisitCount > BestVisitCount)
		{
			BestVisitCount = Child->VisitCount;
			BestChild = Child;
		}
	}
	return BestChild->Board;
}
