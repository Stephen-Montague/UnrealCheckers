#include "MCTS-PUCT/SelectNode.h"
#include "MCTS-PUCT/SearchSetup.h"
#include "MCTS-PUCT/Node.h"
#include "CheckersRules.h"

TObjectPtr<USearchNode> SelectNode(const TObjectPtr<USearchNode> RootNode, const TObjectPtr<UCheckersRules> Rules)
{
	float BestScore = 0.0f;
	TObjectPtr<USearchNode> BestChild;
	TObjectPtr<USearchNode> SelectedNode = RootNode->Clone();
	uint32 Depth = SelectedNode->Depth;

	// If the selected node has children, find the best descendant.
	while (Depth < TreeDepthLimit && SelectedNode->Children.Num() > 0)
	{
		for (const TObjectPtr<USearchNode> Child : SelectedNode->Children)
		{
			if (Child && Child->VisitCount > 0)
			{
				// Score child by below formula, adjusted for adversarial play. See end notes for more info.
				const float  Predictor = ( 1 - Rules->GetWinProbability(Child->Board, Child->bIsPlayer1));
				const float ChildScore = ( 
					(static_cast<float>(Child->SumValue) / Child->VisitCount) +
					(Predictor * FormulaConstant *
						FMath::Sqrt( FMath::Loge(static_cast<float>(SelectedNode->VisitCount)) / Child->VisitCount ) )
					);
				if (ChildScore > BestScore)
				{                    
					BestScore = ChildScore;
					BestChild = Child; 
				}
			}
		}
		// Continue search under best child, and use the LRU children getter, to record using this child.
		SelectedNode = BestChild ? BestChild : SelectedNode->Children[0]; 
		BestScore = 0.0f;
		BestChild = nullptr;
		Depth++;
	}
	return SelectedNode;
}

/*
The algorithm score above relies on the below formula:

AverageValue + ( Predictor * Constant * Sqrt( ln N / n ) )

---

AvgValue:  node.sumValue / node.visitCount

P: Predicted probability that this node is a winner. 

C: Constant that controls the Exploit-Explore ratio, where 0 is greedy.

ln: natural log

N: parent.visitCount 

n: node.visitCount

---

Note: to avoid division by 0 error, visitCount > zero is required.
It also helps the formula work, to get at least some data from each node.

*/ 