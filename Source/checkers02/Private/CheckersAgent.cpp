// Copyright Stephen Montague.


#include "CheckersAgent.h"
#include "MCTS-PUCT/MonteCarloTreeSearch.h"
#include "CheckersGame.h"
#include "CheckersRules.h"

ACheckersAgent::ACheckersAgent() : bIsPlayer1(false), WinCount(0)
{
	Logic = CreateDefaultSubobject<UMonteCarloTreeSearch>(TEXT("MonteCarloTreeSearch"));
}

void ACheckersAgent::RunAI(ACheckersGame* const CheckersGame, const FString& CurrentBoard, const bool IsPlayer1)
{
	if (!CheckersGame ){ return; }
	if (CurrentBoard.Len() != 32) { return; }
	
	Game = CheckersGame;
	Game->SetInitialBoard(CurrentBoard);
	bIsPlayer1 = IsPlayer1;
	PlayNextTurn();
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
		if (GEngine)
		{
	 		GEngine->AddOnScreenDebugMessage(-1, 1.8f, FColor::Yellow,
	 			FString::Printf(TEXT("%s is thinking."), *LogName));
		}
		
		// Cache current board.
		Game->LastBoard = Game->Board;
		// Choose next board.
		Logic->Init(Game->Board, Game->Rules, bIsPlayer1);
		Game->Board = Logic->ChooseNextBoard();
		// Derive piece movements.
		Origin = Game->Rules->DeriveMovements(Game->LastBoard, Game->Board, bIsPlayer1, Movements);
		ReceiveChosenMoves.Broadcast(Origin, Movements);
	}
}

// Based on a similarly named Blueprint Node, but returns the Index (not Distance) with the Actor.
AActor* ACheckersAgent::FindNearestActorIndex(FVector Origin, const TArray<AActor*>& ActorsToCheck, int32& NearestActorIndex)
{
	AActor* NearestActor = nullptr;
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
	return NearestActor;
}


// Get random board
// const int Max = Game->Rules->NextPossibleBoards.Num();
// Game->Board = Game->Rules->NextPossibleBoards[FMath::RandHelper(Max)];