// Copyright Stephen Montague.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CheckersRules.generated.h"

UENUM(BlueprintType)
enum class EWinner : uint8
{
	ENone,
	EPlayer1,
	EPlayer2
};

/**
 * Has a TArray<uint8> named MoveIndices used to store a list of piece moves from origin to destination.
 * This wrapper enables storing lists of moves in another array visible in Blueprints.
 */
USTRUCT(BlueprintType)
struct FTransitionMoves
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<uint8> MoveIndices;
};

/**
 * Checkers rules used to get a new board, decide a winner, generate next possible boards, and make predictions.
 */
UCLASS()
class CHECKERS02_API UCheckersRules : public UObject
{
	GENERATED_BODY()
	friend class ACheckersAgent;
	friend class UMonteCarloTreeSearch;
public:
	UCheckersRules();
	
	static void AssignInitialBoard(FString& OutInitialBoard)
	{
		OutInitialBoard = FString("WWWWWWWWWWWW++++++++MMMMMMMMMMMM");
	}

	UFUNCTION(BlueprintCallable)
	bool GenerateNextPossibleBoards(const FString& Board, const bool IsPlayer1);

	EWinner GetWinner() const { return Winner; }
	
	/** An eval function to guess the future winner.
	 *  An advantage of more than one pawn predicts a Win, otherwise a Tie.
	 */
	static EWinner GetPredictedWinner(const FString& Board, const bool IsPlayer1);

	/* Returns a win probability between [0,1].  */
	static float GetWinProbability(const FString& Board, const bool IsPlayer1);

	/* Returns a board index of the move origin and fills an out-parameter array with all movements. */
	uint8 DeriveMovements(const FString& LastBoard, const FString& NextBoard, const bool IsPlayer1, TArray<uint8>& OutMovements);	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> NextPossibleBoards;
	
	constexpr static TCHAR Man = 'M';  // Player1 pawn.
	constexpr static TCHAR King = 'K';  // Player1 royal.
	constexpr static TCHAR Woman = 'W'; // Player2 pawn.
	constexpr static TCHAR Queen = 'Q';  // Player2 royal.
	constexpr static TCHAR Empty = '+';
	
private:
	static bool IsJumpPossibleOnBoard(const FString& Board, const bool IsPlayer1,
	                           const TCHAR PlayerPawn, const TCHAR PlayerRoyal,
	                           const TCHAR OpponentPawn, const TCHAR OpponentRoyal);

	static bool IsJumpPossibleForPiece(const FString& Board, const bool IsPlayer1, const uint8 PieceIndex,
		const TCHAR PlayerRoyal, const TCHAR OpponentPawn, const TCHAR OpponentRoyal);
	
	void EmplaceAllJumpMoves(const FString& Board, const bool IsPlayer1,
		const TCHAR PlayerPawn, const TCHAR PlayerRoyal,
		const TCHAR OpponentPawn, const TCHAR OpponentRoyal);

	void GenerateNextJumpBoards(const FString& Board, const uint8 PieceIndex, const bool IsPlayer1,
		const TCHAR PlayerPawn, const TCHAR PlayerRoyal,
		const TCHAR OpponentPawn, const TCHAR OpponentRoyal);

	void EmplaceAllAdjacentMoves(const FString& Board, const bool IsPlayer1, const TCHAR PlayerPawn, const TCHAR PlayerRoyal);

	static void AssignNewBoardFromMove(FString& OutBoard, bool& OutPieceWasPromoted,
		const uint8 OriginIndex,const uint8 DestinationIndex, const uint8 OpponentIndex);

	static void AssignNewBoardFromMove(FString& OutBoard, const uint8 OriginIndex,const uint8 DestinationIndex);

	/** Functions to find an index near a piece.
	 *  Returns the described index near a given index.
	 *  Returns 255 (uint8 max) if off the board.
	 */
	static uint8 NorthWestGet(uint8 Index);
	static uint8 NorthEastGet(uint8 Index);
	static uint8 SouthWestGet(uint8 Index);
	static uint8 SouthEastGet(uint8 Index);
	static uint8 NorthWestJumpGet(uint8 Index);
	static uint8 NorthEastJumpGet(uint8 Index);
	static uint8 SouthWestJumpGet(uint8 Index);
	static uint8 SouthEastJumpGet(uint8 Index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	EWinner Winner;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<FTransitionMoves> PossibleTurnMovements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FTransitionMoves TransitionMoves;
	
	constexpr static uint8 BoardCellCount = 32;
	constexpr static uint8 KingPromotionMax = 4; // Max index, exclusive, of the top row.
	constexpr static uint8 QueenPromotionMin = 27; // Min index, exclusive, of the bottom row. 
	constexpr static uint8 OffBoard = 255;
};
