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
	if (Board.Len() != BoardWidth * BoardHeight){ return; }
	
	FString LineToPrint = TEXT("");
	for (int32 y = BoardHeight - 1; y > -1; y--)
	{
		for (int32 x = 0; x < BoardWidth; x++)
		{
			const int32 CellIndex = (y * BoardWidth) + x;
			if (y % 2 == 1)
			{
				LineToPrint += Board[CellIndex];
				LineToPrint += TEXT(" ");
			}
			else if (x % BoardWidth == 0)
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
			GEngine->AddOnScreenDebugMessage(y, 8.0f, FColor::Yellow,
				FString::Printf(TEXT("%s"), *LineToPrint));
		}
		LineToPrint = TEXT("");
	}
}
