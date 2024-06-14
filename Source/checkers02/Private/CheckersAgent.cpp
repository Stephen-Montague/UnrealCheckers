// Copyright Stephen Montague.


#include "CheckersAgent.h"
#include "MCTS-PUCT/MonteCarloTreeSearch.h"
#include "CheckersGame.h"
#include "CheckersRules.h"

ACheckersAgent::ACheckersAgent() : bIsPlayer1(false), WinCount(0)
{
	Logic = CreateDefaultSubobject<UMonteCarloTreeSearch>(TEXT("MonteCarloTreeSearch"));
}

void ACheckersAgent::RunAI(ACheckersGame* const CurrentGame, const FString& CurrentBoard, const bool IsPlayer1)
{
	if (!CurrentGame ){ return; }
	if (CurrentBoard.Len() != CurrentGame->Rules->BoardCellCount) { return; }

	if (!bIsRunning)
	{
		bIsRunning = true;
		Game = CurrentGame;
		Game->SetInitialBoard(CurrentBoard);
		bIsPlayer1 = IsPlayer1;
		PlayNextTurn();
	}
}

void ACheckersAgent::PlayNextTurn()
{
	if (Game && Game->Rules)
	{
		Game->bHasNextPossibleBoard = Game->Rules->GenerateNextPossibleBoards(Game->Board, bIsPlayer1);
		if (Game->HasWinner())
		{
			Game->bIsDone = true;
		}
		else if(Game->IsOver())
		{
			Game->bIsDone = true;
		}
		else
		{
			ChooseNextMoves();
			Game->LogBoard();
		}
	}
}

void ACheckersAgent::ChooseNextMoves()
{
	if (Game && Game->Rules && Logic)
	{
		// Cache current board.
		Game->LastBoard = Game->Board;
		// Choose next board.
		Logic->Init(Game->Board, Game->Rules, bIsPlayer1);
		Game->Board = Logic->ChooseNextBoard();
		// Derive piece movements.
		PieceOrigin = Game->Rules->DeriveMovements(Game->LastBoard, Game->Board, bIsPlayer1, ChosenMovesIndex);
		ReceiveChosenMoves.Broadcast(PieceOrigin, ChosenMovesIndex);
		bIsRunning = false;
	} 
}

// Based on a similarly named Blueprint Node, but returns the Index (not Distance) with the nearest Actor.
void ACheckersAgent::FindNearestActorIndex(FVector Origin, const TArray<AActor*>& ActorsToCheck,
	int32& NearestActorIndex, AActor*& NearestActor)
{
	NearestActor = nullptr;
	float DistanceFromNearestActor = TNumericLimits<float>::Max();

	int32 ActorIndex = 0;
	for (AActor* ActorToCheck : ActorsToCheck)
	{
		if (ActorToCheck)
		{
			const float DistanceFromActorToCheck = (Origin - ActorToCheck->GetActorLocation()).SizeSquared();
			if (DistanceFromActorToCheck < DistanceFromNearestActor)
			{
				NearestActor = ActorToCheck;
				NearestActorIndex = ActorIndex;
				DistanceFromNearestActor = DistanceFromActorToCheck;
			}
		}
		++ActorIndex;
	}
}
