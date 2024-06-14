// Copyright Stephen Montague.


#include "CheckersGame.h"
#include "CheckersRules.h"

ACheckersGame::ACheckersGame() :
bHasNextPossibleBoard(true),
bIsDone(false)
{
	Rules = CreateDefaultSubobject<UCheckersRules>(TEXT("CheckersRules"));
}

bool ACheckersGame::HasWinner() const
{
	if (Rules)
	{
		return (Rules->GetWinner() != EWinner::ENone);
	}
	return false;
}

bool ACheckersGame::IsOver() const
{
	return !bHasNextPossibleBoard;
}

void ACheckersGame::LogBoard()
{
	if (!Rules) { return; }
	if (Board.Len() != Rules->BoardCellCount){ return; }
	
	FString LineToPrint = TEXT("");
	for (uint32 y = 0; y < Rules->BoardHeight; y++)
	{
		for (uint32 x = 0; x < Rules->BoardWidth; x++)
		{
			const uint32 CellIndex = (y * Rules->BoardWidth) + x;
			if (y % 2 == 1)
			{
				LineToPrint += Board[CellIndex];
				LineToPrint += TEXT(" ");
			}
			else if (x % Rules->BoardWidth == 0)
			{
				LineToPrint += TEXT(" ");
				LineToPrint += Board[CellIndex];
				LineToPrint += TEXT(" ");
			}
			else
			{
				LineToPrint += Board[CellIndex];
				LineToPrint += TEXT(" ");
			}
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 8.0f, FColor::Yellow,
				FString::Printf(TEXT("%s"), *LineToPrint), false);
		}
		LineToPrint = TEXT("");
	}
}
