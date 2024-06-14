// Copyright Stephen Montague.


#include "MCTS-PUCT/Backpropagate.h"
#include "MCTS-PUCT/Node.h"

void BackPropagate(const TObjectPtr<USearchNode> SimulatedNode, uint32 Result)
{
	TObjectPtr<USearchNode> Parent = SimulatedNode->Parent;

	// Update all ancestors who match the simulated node by player, ending after root is updated.
	while(Parent)
	{
		if (Parent->bIsPlayer1 == SimulatedNode->bIsPlayer1)
		{
			Parent->SumValue += Result;
		}
		Parent->VisitCount++;
		Parent = Parent->Parent;
	}	
}
