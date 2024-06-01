// Copyright Stephen Montague.


#include "MCTS-PUCT/Node.h"

void USearchNode::Init(const FString& InBoard, const bool InIsPlayer1, const uint32 InVisitCount,
	const uint32 InSumValue, TObjectPtr<USearchNode> const InParent, const bool InIsProvenWinner)
{
	Board = InBoard;
	bIsPlayer1 = InIsPlayer1;
	VisitCount = InVisitCount;
	SumValue = InSumValue;
	Parent = InParent;
	Depth = (Parent == nullptr)? 0 : Parent->Depth + 1;
	Children.Reset(); 
	bIsProvenWinner = InIsProvenWinner;
}

TObjectPtr<USearchNode> USearchNode::Clone()
{
	const TObjectPtr<USearchNode> Clone = NewObject<USearchNode>();
	Clone->Board = Board;
	Clone->bIsPlayer1 = bIsPlayer1;
	Clone->VisitCount = VisitCount;
	Clone->SumValue = SumValue;
	Clone->Parent = Parent;
	Clone->Children = Children;
	Clone->bIsProvenWinner = bIsProvenWinner;
	return Clone;
}
