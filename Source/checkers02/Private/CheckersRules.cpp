// Copyright Stephen Montague.


#include "CheckersRules.h"

void UCheckersRules::ResetMembers()
{
	NextPossibleBoards.Reset();
	PossibleTurnMovements.Reset();
	PossibleTurnCaptures.Reset();
	TransitionMoves.ActionIndices.Reset();
	JumpedOpponents.ActionIndices.Reset();
	PossibleOrigins.Reset();
	Winner = EWinner::ENone;
}

bool UCheckersRules::GenerateNextPossibleBoards(const FString& Board, const bool IsPlayer1)
{
	ResetMembers();

	const TCHAR PlayerPawn = IsPlayer1 ? Man : Woman;
	const TCHAR PlayerRoyal = IsPlayer1 ? King : Queen;
	const TCHAR OpponentPawn = IsPlayer1 ? Woman : Man;
	const TCHAR OpponentRoyal = IsPlayer1 ? Queen : King;

	if (IsJumpPossibleOnBoard(Board, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal))
	{
		EmplaceAllJumpMoves(Board, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
	}
	else
	{
		EmplaceAllAdjacentMoves(Board, IsPlayer1, PlayerPawn, PlayerRoyal);
	}
	
	if (NextPossibleBoards.Num() > 0)
	{
		return true;
	}
	// Whoever played last, opposite of current, won.
	Winner = IsPlayer1? EWinner::EPlayer2 : EWinner::EPlayer1;
	return false;
}

bool UCheckersRules::GenerateNextPossibleBoardsForPiece(const FString& Board, const bool bIsPlayer1,
	const int32 PieceIndex, bool& bHasCapture)
{
	ResetMembers();
	bHasCapture = false;

	const TCHAR PlayerPawn = bIsPlayer1 ? Man : Woman;
	const TCHAR PlayerRoyal = bIsPlayer1 ? King : Queen;
	const TCHAR OpponentPawn = bIsPlayer1 ? Woman : Man;
	const TCHAR OpponentRoyal = bIsPlayer1 ? Queen : King;

	if (IsJumpPossibleForPiece(Board, bIsPlayer1, PieceIndex, PlayerRoyal, OpponentPawn, OpponentRoyal))
	{
		GenerateNextJumpBoardsForPiece(Board, PieceIndex, bIsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
		bHasCapture = true;
	}
	else if (!IsJumpPossibleOnBoard(Board, bIsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal))
	{
		GenerateNextAdjacentMovesForPiece(Board, bIsPlayer1, PlayerPawn, PlayerRoyal, PieceIndex);
	}
	
	if (NextPossibleBoards.Num() > 0)
	{
		return true;
	}
	return false;
}

bool UCheckersRules::GenerateNextPossibleOrigins(const FString& Board, const bool bIsPlayer1)
{
	ResetMembers();

	const TCHAR PlayerPawn = bIsPlayer1 ? Man : Woman;
	const TCHAR PlayerRoyal = bIsPlayer1 ? King : Queen;
	const TCHAR OpponentPawn = bIsPlayer1 ? Woman : Man;
	const TCHAR OpponentRoyal = bIsPlayer1 ? Queen : King;

	if (IsJumpPossibleOnBoard(Board, bIsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal))
	{
		for (uint8 Index = 0; Index < BoardCellCount; Index++)
		{
			if (Board[Index] == PlayerPawn || Board[Index] == PlayerRoyal)
			{
				if (IsJumpPossibleForPiece(Board, bIsPlayer1, Index, PlayerRoyal, OpponentPawn, OpponentRoyal))
				{
					PossibleOrigins.Emplace(Index);		
				}
			}
		}
	}
	else
	{
		for (uint8 Index = 0; Index < BoardCellCount; Index++)
		{
			if (Board[Index] == PlayerPawn || Board[Index] == PlayerRoyal)
			{
				if (IsAdjacentMovePossibleForPiece(Board, bIsPlayer1, Index, PlayerRoyal))
				{
					PossibleOrigins.Emplace(Index);	
				}
			}
		}
	}
	if (PossibleOrigins.Num() > 0)
	{
		return true;
	}
	return false;
}

bool UCheckersRules::IsJumpPossibleOnBoard(const FString& Board, const bool bIsPlayer1, const TCHAR PlayerPawn, const TCHAR PlayerRoyal, const TCHAR OpponentPawn, const TCHAR OpponentRoyal)
{    
   for (uint8 Index = 0; Index < BoardCellCount; Index++)
   {
      // Check if the cell contains a piece of the current player
      if (Board[Index] == PlayerPawn || Board[Index] == PlayerRoyal)
      {
         // Calculate forward Indexes near piece
         const uint8 FwdLeftIndex = bIsPlayer1 ? NorthWestGet(Index) : SouthEastGet(Index);
         const uint8 FwdRightIndex = bIsPlayer1 ? NorthEastGet(Index) : SouthWestGet(Index);
         const uint8 FwdLeftJumpIndex = bIsPlayer1 ? NorthWestJumpGet(Index) : SouthEastJumpGet(Index);
         const uint8 FwdRightJumpIndex = bIsPlayer1 ? NorthEastJumpGet(Index) : SouthWestJumpGet(Index);
         // Check for forward left jumps
         if (FwdLeftIndex != OffBoard && FwdLeftJumpIndex != OffBoard &&
            Board[FwdLeftJumpIndex] == Empty &&
            (Board[FwdLeftIndex] == OpponentPawn ||
            Board[FwdLeftIndex] == OpponentRoyal))
         {
            return true;
         }
         // Check for forward right jumps
         if (FwdRightIndex != OffBoard && FwdRightJumpIndex != OffBoard &&
            Board[FwdRightJumpIndex] == Empty &&
            (Board[FwdRightIndex] == OpponentPawn ||
            Board[FwdRightIndex] == OpponentRoyal))
         {
            return true;
         }
         if (Board[Index] == PlayerRoyal)
         {
            // Calculate backward cells near the piece
            const uint8 BackLeftIndex = bIsPlayer1 ? SouthWestGet(Index) : NorthEastGet(Index);
            const uint8 BackRightIndex = bIsPlayer1 ? SouthEastGet(Index) : NorthWestGet(Index);
            const uint8 BackLeftJumpIndex = bIsPlayer1 ? SouthWestJumpGet(Index) : NorthEastJumpGet(Index);
            const uint8 BackRightJumpIndex = bIsPlayer1 ? SouthEastJumpGet(Index) : NorthWestJumpGet(Index);
            // Check for back left jumps
            if (BackLeftIndex != OffBoard && BackLeftJumpIndex != OffBoard &&
               Board[BackLeftJumpIndex] == Empty &&
               (Board[BackLeftIndex] == OpponentPawn ||
               Board[BackLeftIndex] == OpponentRoyal))
            {
               return true;
            }
            // Check for back right jumps
            if (BackRightIndex != OffBoard && BackRightJumpIndex != OffBoard &&
               Board[BackRightJumpIndex] == Empty &&
               (Board[BackRightIndex] == OpponentPawn ||
               Board[BackRightIndex] == OpponentRoyal))
            {
               return true;
            }
         }
      }
   }
   return false;
}

bool UCheckersRules::IsJumpPossibleForPiece(const FString& Board, const bool IsPlayer1, const uint8 PieceIndex,
	const TCHAR PlayerRoyal, const TCHAR OpponentPawn, const TCHAR OpponentRoyal)
{
	// Calculate forward Indexes near piece
	const uint8 FwdLeftIndex = IsPlayer1 ? NorthWestGet(PieceIndex) : SouthEastGet(PieceIndex);
    const uint8 FwdRightIndex = IsPlayer1 ? NorthEastGet(PieceIndex) : SouthWestGet(PieceIndex);
    const uint8 FwdLeftJumpIndex = IsPlayer1 ? NorthWestJumpGet(PieceIndex) : SouthEastJumpGet(PieceIndex);
    const uint8 FwdRightJumpIndex = IsPlayer1 ? NorthEastJumpGet(PieceIndex) : SouthWestJumpGet(PieceIndex);
    // Check for forward left jumps
    if (FwdLeftIndex != OffBoard && FwdLeftJumpIndex != OffBoard &&
        Board[FwdLeftJumpIndex] == Empty &&
        (Board[FwdLeftIndex] == OpponentPawn ||
        Board[FwdLeftIndex] == OpponentRoyal))
    {
        return true;
    }
    // Check for forward right jumps
    if (FwdRightIndex != OffBoard && FwdRightJumpIndex != OffBoard &&
        Board[FwdRightJumpIndex] == Empty &&
        (Board[FwdRightIndex] == OpponentPawn ||
        Board[FwdRightIndex] == OpponentRoyal))
    {
        return true;
    }
    if (Board[PieceIndex] == PlayerRoyal)
    {
        // Calculate backward cells near the piece
        const uint8 BackLeftIndex = IsPlayer1 ? SouthWestGet(PieceIndex) : NorthEastGet(PieceIndex);
        const uint8 BackRightIndex = IsPlayer1 ? SouthEastGet(PieceIndex) : NorthWestGet(PieceIndex);
        const uint8 BackLeftJumpIndex = IsPlayer1 ? SouthWestJumpGet(PieceIndex) : NorthEastJumpGet(PieceIndex);
        const uint8 BackRightJumpIndex = IsPlayer1 ? SouthEastJumpGet(PieceIndex) : NorthWestJumpGet(PieceIndex);
        // Check for back left jumps
        if (BackLeftIndex != OffBoard && BackLeftJumpIndex != OffBoard &&
            Board[BackLeftJumpIndex] == Empty &&
            (Board[BackLeftIndex] == OpponentPawn ||
            Board[BackLeftIndex] == OpponentRoyal))
        {
            return true;
        }
        // Check for back right jumps
        if (BackRightIndex != OffBoard && BackRightJumpIndex != OffBoard &&
            Board[BackRightJumpIndex] == Empty &&
            (Board[BackRightIndex] == OpponentPawn ||
            Board[BackRightIndex] == OpponentRoyal))
        {
            return true;
        }
    }
    return false;
}

bool UCheckersRules::IsAdjacentMovePossibleForPiece(const FString& Board, const bool IsPlayer1, const uint8 PieceIndex,
                                                    const TCHAR PlayerRoyal)
{
	// Calculate forward Indexes near piece
	const uint8 FwdLeftIndex = IsPlayer1 ? NorthWestGet(PieceIndex) : SouthEastGet(PieceIndex);
    const uint8 FwdRightIndex = IsPlayer1 ? NorthEastGet(PieceIndex) : SouthWestGet(PieceIndex);
    // Check for a forward left adjacent move
    if (FwdLeftIndex != OffBoard && 
        Board[FwdLeftIndex] == Empty)
    {
        return true;
    }
    // Check for a forward right adjacent move
    if (FwdRightIndex != OffBoard &&
        Board[FwdRightIndex] == Empty)
    {
        return true;
    }
    if (Board[PieceIndex] == PlayerRoyal)
    {
        // Calculate backward cells near the piece
        const uint8 BackLeftIndex = IsPlayer1 ? SouthWestGet(PieceIndex) : NorthEastGet(PieceIndex);
        const uint8 BackRightIndex = IsPlayer1 ? SouthEastGet(PieceIndex) : NorthWestGet(PieceIndex);
        // Check for a back left adjacent move
        if (BackLeftIndex != OffBoard && 
            Board[BackLeftIndex] == Empty)
        {
            return true;
        }
        // Check for a back right adjacent move
        if (BackRightIndex != OffBoard && 
            Board[BackRightIndex] == Empty)
        {
            return true;
        }
    }
    return false;
}

void UCheckersRules::EmplaceAllJumpMoves(const FString& Board, const bool IsPlayer1, const TCHAR PlayerPawn, const TCHAR PlayerRoyal, const TCHAR OpponentPawn, const TCHAR OpponentRoyal) {
	for (uint8 Index = 0; Index < BoardCellCount; Index++) {
		if (Board[Index] == PlayerPawn || Board[Index] == PlayerRoyal) {
			GenerateNextJumpBoardsForPiece(Board, Index, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
		}
	}
}

void UCheckersRules::GenerateNextJumpBoardsForPiece(const FString& Board, const uint8 PieceIndex, const bool IsPlayer1, const TCHAR PlayerPawn,
	const TCHAR PlayerRoyal, const TCHAR OpponentPawn, const TCHAR OpponentRoyal)
{
	// Calculate forward Indexes near piece
    const uint8 FwdLeftIndex = IsPlayer1 ? NorthWestGet(PieceIndex) : SouthEastGet(PieceIndex);
    const uint8 FwdRightIndex = IsPlayer1 ? NorthEastGet(PieceIndex) : SouthWestGet(PieceIndex);
    const uint8 FwdLeftJumpIndex = IsPlayer1 ? NorthWestJumpGet(PieceIndex) : SouthEastJumpGet(PieceIndex);
    const uint8 FwdRightJumpIndex = IsPlayer1 ? NorthEastJumpGet(PieceIndex) : SouthWestJumpGet(PieceIndex);
    // Check for a forward left jump
    if (FwdLeftIndex != OffBoard && FwdLeftJumpIndex != OffBoard &&
        Board[FwdLeftJumpIndex] == Empty &&
        (Board[FwdLeftIndex] == OpponentPawn ||
        Board[FwdLeftIndex] == OpponentRoyal))
    {
        // Get a new board from jumping the opponent's piece
    	bool OutPieceWasPromoted;
        FString NewBoard = Board;
    	UpdateBoardFromMove(NewBoard, OutPieceWasPromoted, PieceIndex,FwdLeftJumpIndex, FwdLeftIndex);
    	
    	// Continue jumping if possible, or if terminal, add the new board
        if (!OutPieceWasPromoted &&
        	IsJumpPossibleForPiece(NewBoard, IsPlayer1, FwdLeftJumpIndex, PlayerRoyal, OpponentPawn, OpponentRoyal))
        {
            TransitionMoves.ActionIndices.Emplace(FwdLeftJumpIndex);
            GenerateNextJumpBoardsForPiece(NewBoard, FwdLeftJumpIndex, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
        }
        else
        {
            TransitionMoves.ActionIndices.Emplace(FwdLeftJumpIndex);
            PossibleTurnMovements.Emplace(TransitionMoves);
        	PossibleTurnCaptures.Emplace(JumpedOpponents);
            NextPossibleBoards.Emplace(NewBoard);
            TransitionMoves.ActionIndices.Reset();
        	JumpedOpponents.ActionIndices.Reset();
        }
    }
    // Check for a forward right jump
    if (FwdRightIndex != OffBoard && FwdRightJumpIndex != OffBoard &&
        Board[FwdRightJumpIndex] == Empty &&
        (Board[FwdRightIndex] == OpponentPawn ||
        Board[FwdRightIndex] == OpponentRoyal))
    {
    	// Get a new board from jumping the opponent's piece
    	bool OutPieceWasPromoted;
    	FString NewBoard = Board;
    	UpdateBoardFromMove(NewBoard, OutPieceWasPromoted, PieceIndex,FwdRightJumpIndex, FwdRightIndex);
    	
    	// Continue jumping if possible, or if terminal, add the new board
        if (!OutPieceWasPromoted &&
        	IsJumpPossibleForPiece(NewBoard, IsPlayer1, FwdRightJumpIndex, PlayerRoyal, OpponentPawn, OpponentRoyal))
        {
            TransitionMoves.ActionIndices.Emplace(FwdRightJumpIndex);
            GenerateNextJumpBoardsForPiece(NewBoard, FwdRightJumpIndex, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
        }
        else
        {
            TransitionMoves.ActionIndices.Emplace(FwdRightJumpIndex);
            PossibleTurnMovements.Emplace(TransitionMoves);
        	PossibleTurnCaptures.Emplace(JumpedOpponents);
            NextPossibleBoards.Emplace(NewBoard);
            TransitionMoves.ActionIndices.Reset();
        	JumpedOpponents.ActionIndices.Reset();
        }
    }
    // Check if the piece is a king 
    if (Board[PieceIndex] == PlayerRoyal)
    {
        // Calculate backward cells near the piece
        const uint8 BackLeftIndex = IsPlayer1 ? SouthWestGet(PieceIndex) : NorthEastGet(PieceIndex);
        const uint8 BackRightIndex = IsPlayer1 ? SouthEastGet(PieceIndex) : NorthWestGet(PieceIndex);
        const uint8 BackLeftJumpIndex = IsPlayer1 ? SouthWestJumpGet(PieceIndex) : NorthEastJumpGet(PieceIndex);
        const uint8 BackRightJumpIndex = IsPlayer1 ? SouthEastJumpGet(PieceIndex) : NorthWestJumpGet(PieceIndex);
        // Check for a back left jump
        if (BackLeftIndex != OffBoard && BackLeftJumpIndex != OffBoard &&
            Board[BackLeftJumpIndex] == Empty &&
            (Board[BackLeftIndex] == OpponentPawn ||
            Board[BackLeftIndex] == OpponentRoyal))
        {
        	// Get a new board from jumping the opponent's piece
        	bool OutPieceWasPromoted;
        	FString NewBoard = Board;
        	UpdateBoardFromMove(NewBoard, OutPieceWasPromoted, PieceIndex,BackLeftJumpIndex, BackLeftIndex);
  	
            // Continue jumping if possible, or if terminal, add the new board
            if (!OutPieceWasPromoted &&
            	IsJumpPossibleForPiece(NewBoard, IsPlayer1, BackLeftJumpIndex, PlayerRoyal, OpponentPawn, OpponentRoyal))
            {
                TransitionMoves.ActionIndices.Emplace(BackLeftJumpIndex);
                GenerateNextJumpBoardsForPiece(NewBoard, BackLeftJumpIndex, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
            }
            else
            {
                TransitionMoves.ActionIndices.Emplace(BackLeftJumpIndex);
                PossibleTurnMovements.Emplace(TransitionMoves);
            	PossibleTurnCaptures.Emplace(JumpedOpponents);
                NextPossibleBoards.Emplace(NewBoard);
                TransitionMoves.ActionIndices.Reset();
            	JumpedOpponents.ActionIndices.Reset();
            }
        }
        // Check for a back right jump
        if (BackRightIndex != OffBoard && BackRightJumpIndex != OffBoard &&
            Board[BackRightJumpIndex] == Empty &&
            (Board[BackRightIndex] == OpponentPawn ||
            Board[BackRightIndex] == OpponentRoyal))
        {
        	// Get a new board from jumping the opponent's piece
        	bool OutPieceWasPromoted;
        	FString NewBoard = Board;
        	UpdateBoardFromMove(NewBoard, OutPieceWasPromoted, PieceIndex,BackRightJumpIndex, BackRightIndex);

            // Continue jumping if possible, or if terminal, add the new board
            if (!OutPieceWasPromoted &&
            	IsJumpPossibleForPiece(NewBoard, IsPlayer1, BackRightJumpIndex, PlayerRoyal, OpponentPawn, OpponentRoyal))
            {
                TransitionMoves.ActionIndices.Emplace(BackRightJumpIndex);
                GenerateNextJumpBoardsForPiece(NewBoard, BackRightJumpIndex, IsPlayer1, PlayerPawn, PlayerRoyal, OpponentPawn, OpponentRoyal);
            }
            else
            {
                TransitionMoves.ActionIndices.Emplace(BackRightJumpIndex);
                PossibleTurnMovements.Emplace(TransitionMoves);
            	PossibleTurnCaptures.Emplace(JumpedOpponents);
                NextPossibleBoards.Emplace(NewBoard);
                TransitionMoves.ActionIndices.Reset();
            	JumpedOpponents.ActionIndices.Reset();
            }
        }
    }
}

void UCheckersRules::GenerateNextAdjacentMovesForPiece(const FString& Board, const bool IsPlayer1,
	const TCHAR PlayerPawn, const TCHAR PlayerRoyal, const uint8 PieceIndex)
{
	if (Board[PieceIndex] == PlayerPawn || Board[PieceIndex] == PlayerRoyal)
	{
		// Calculate forward Indexes near piece.
		const uint8 FwdLeftIndex = IsPlayer1 ? NorthWestGet(PieceIndex) : SouthEastGet(PieceIndex);
		const uint8 FwdRightIndex = IsPlayer1 ? NorthEastGet(PieceIndex) : SouthWestGet(PieceIndex);

		// Check if piece can move to an adjacent cell.
		if (FwdLeftIndex != OffBoard && Board[FwdLeftIndex] == Empty)
		{
			// Make the move on a new board.
			FString NewBoard = Board;
			UpdateBoardFromMove(NewBoard, PieceIndex, FwdLeftIndex);
			// Add the new board to next possible boards.
			TransitionMoves.ActionIndices.Emplace(FwdLeftIndex);
			PossibleTurnMovements.Emplace(TransitionMoves);
			NextPossibleBoards.Emplace(NewBoard);
			TransitionMoves.ActionIndices.Reset();
		}
		if (FwdRightIndex != OffBoard && Board[FwdRightIndex] == Empty) {
			FString NewBoard = Board;
			UpdateBoardFromMove(NewBoard, PieceIndex, FwdRightIndex);
			TransitionMoves.ActionIndices.Emplace(FwdRightIndex);
			PossibleTurnMovements.Emplace(TransitionMoves);
			NextPossibleBoards.Emplace(NewBoard);
			TransitionMoves.ActionIndices.Reset();
		}
		// Check for king moves.
		if (Board[PieceIndex] == PlayerRoyal)
		{
			const uint8 BackLeftIndex = IsPlayer1 ? SouthWestGet(PieceIndex) : NorthEastGet(PieceIndex);
			const uint8 BackRightIndex = IsPlayer1 ? SouthEastGet(PieceIndex) : NorthWestGet(PieceIndex);
			if (BackLeftIndex != OffBoard && Board[BackLeftIndex] == Empty)
			{
				FString NewBoard = Board;
				UpdateBoardFromMove(NewBoard, PieceIndex, BackLeftIndex);
				TransitionMoves.ActionIndices.Emplace(BackLeftIndex);
				PossibleTurnMovements.Emplace(TransitionMoves);
				NextPossibleBoards.Emplace(NewBoard);
				TransitionMoves.ActionIndices.Reset();
			}
			if (BackRightIndex != OffBoard && Board[BackRightIndex] == Empty)
			{
				FString NewBoard = Board;
				UpdateBoardFromMove(NewBoard, PieceIndex, BackRightIndex);
				TransitionMoves.ActionIndices.Emplace(BackRightIndex);
				PossibleTurnMovements.Emplace(TransitionMoves);
				NextPossibleBoards.Emplace(NewBoard);
				TransitionMoves.ActionIndices.Reset();
			}
		}
	}
}

void UCheckersRules::EmplaceAllAdjacentMoves(const FString& Board, const bool IsPlayer1,
                                             const TCHAR PlayerPawn, const TCHAR PlayerRoyal)
{
    for (uint8 Index = 0; Index < BoardCellCount; Index++)
    {
	    GenerateNextAdjacentMovesForPiece(Board, IsPlayer1, PlayerPawn, PlayerRoyal, Index);
	}
}

void UCheckersRules::UpdateBoardFromMove(FString& OutBoard, bool& OutPieceWasPromoted,
	const uint8 OriginIndex,const uint8 DestinationIndex, const uint8 OpponentIndex)
{
	// Move piece to destination.
	OutBoard[DestinationIndex] = OutBoard[OriginIndex];
	OutBoard[OriginIndex] = Empty;
	// Remove jumped opponent.
	OutBoard[OpponentIndex] = Empty;
	JumpedOpponents.ActionIndices.Emplace(OpponentIndex);
	// Check if a pawn reached the last row and promote if necessary.
	OutPieceWasPromoted = false;
	if (OutBoard[DestinationIndex] == Man && DestinationIndex < KingPromotionMax) 
	{
		OutBoard[DestinationIndex] = King;
		OutPieceWasPromoted = true;
	}
	if (OutBoard[DestinationIndex] == Woman && DestinationIndex > QueenPromotionMin) 
	{
		OutBoard[DestinationIndex] = Queen;
		OutPieceWasPromoted = true;
	}
}

void UCheckersRules::UpdateBoardFromMove(FString& OutBoard, const uint8 OriginIndex,const uint8 DestinationIndex)
{
	// Move piece to destination.
	OutBoard[DestinationIndex] = OutBoard[OriginIndex];
	OutBoard[OriginIndex] = Empty;
	// Promote piece if necessary.
	if (OutBoard[DestinationIndex] == Man && DestinationIndex < KingPromotionMax) 
	{
		OutBoard[DestinationIndex] = King;
	}
	if (OutBoard[DestinationIndex] == Woman && DestinationIndex > QueenPromotionMin) 
	{
		OutBoard[DestinationIndex] = Queen;
	}
}

uint8 UCheckersRules::NorthWestGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for topmost cells that have no northWest.
	if (Index < 5)
		return OffBoard;
	// Check rows that offset toward East, which all have a northWest.
	if ((Index % 8) < 4)
	{
		return Index - 4;
	}
	// Row is not offset, so after each remaining row beginning, calculate the northWest.
	else
	{
		// Check if row beginning.
		if (Index % 4 == 0)
			return OffBoard;
		else
			return Index - 5;
	}
}

uint8 UCheckersRules::NorthEastGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check the top row, which has no northEast.
	if (Index < 4)
		return OffBoard;
	// Check rows that offset toward East, which all, except for end cells, have a northEast.
	if ((Index % 8) < 4)
	{
		if (Index % 4 == 3)
			return OffBoard;
		else
			return Index - 3;
	}
	// Row is not offset, so for all cells calculate the northEast.
	else
	{
		return Index - 4;
	}
}

uint8 UCheckersRules::SouthWestGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for lowest row cells that have no southWest.
	if (Index > 27)
		return OffBoard;
	// Check rows that offset toward East, which all have a southWest.
	if ((Index % 8) < 4)
	{
		return Index + 4;
	}
	// Row is not offset, so after each remaining row beginning, calculate the southWest.
	else
	{
		// Check if row beginning.
		if (Index % 4 == 0)
			return OffBoard;
		else
			return Index + 3;
	}
}

uint8 UCheckersRules::SouthEastGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check near bottom row cells, which have no southEast.
	if (Index > 26)
		return OffBoard;
	// Check rows that offset toward East, which all, except for end cells, have a southEast.
	if ((Index % 8) < 4)
	{
		if (Index % 4 == 3)
			return OffBoard;
		else
			return Index + 5;
	}
	// Row is not offset, so for all cells calculate the southEast.
	else
	{
		return Index + 4;
	}
}

uint8 UCheckersRules::NorthWestJumpGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for topmost cells or row beginnings, all which have no northWest jump.
	if (Index < 9 || Index % 4 == 0)
		return OffBoard;
	else
		return Index - 9;
}

uint8 UCheckersRules::NorthEastJumpGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for topmost cells or row ends, all which have no northEast jump.
	if (Index < 8 || Index % 4 == 3)
		return OffBoard;
	else
		return Index - 7;
}

uint8 UCheckersRules::SouthWestJumpGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for bottom cells or row beginnings, which all have no southWest jump.
	if (Index > 23 || Index % 4 == 0)
		return OffBoard;
	else
		return Index + 7;
}

uint8 UCheckersRules::SouthEastJumpGet(uint8 Index)
{
	// Index visual of the checker board to confirm below.
	//   +  0   +  1   +   2   +  3
	//   4  +   5  +   6   +   7  +
	//   +  8   +  9   +  10   + 11
	//  12  +  13  +  14   +  15  +
	//   + 16   + 17   +  18   + 19
	//  20  +  21  +  22   +  23  +
	//   + 24   + 25   +  26   + 27
	//  28  +  29  +  30   +  31  +
	// Check for bottom cells or row ends, which all have no southEast jump.
	if (Index > 23 || Index % 4 == 3)
		return OffBoard;
	else
		return Index + 9;
}

EWinner UCheckersRules::GetPredictedWinner(const FString& Board, const bool IsPlayer1)
{
	const TCHAR ActivePawn = IsPlayer1? Man : Woman;
	const TCHAR ActiveRoyal = IsPlayer1? King : Queen;
	const TCHAR OpponentPawn = IsPlayer1? Woman : Man;
	const TCHAR OpponentRoyal = IsPlayer1? Queen : King;
	uint8 ActivePawnCount = 0;
	uint8 ActiveRoyalCount = 0;
	uint8 OpponentPawnCount = 0;
	uint8 OpponentRoyalCount = 0;
	// Count each piece type on the board.
	for (uint8 i = 0; i < BoardCellCount; i++)
	{
		if (Board[i] == ActivePawn)
			ActivePawnCount++;
		else if (Board[i] == ActiveRoyal)
			ActiveRoyalCount++;
		else if (Board[i] == OpponentPawn)
			OpponentPawnCount++;
		else if (Board[i] == OpponentRoyal)
			OpponentRoyalCount++;
	}
	constexpr uint8 PawnValue = 2;
	constexpr uint8 RoyalValue = 3;
	const uint8 ActivePlayerScore = (PawnValue * ActivePawnCount) + (RoyalValue * ActiveRoyalCount);
	const uint8 OpponentScore = (PawnValue * OpponentPawnCount) + (RoyalValue * OpponentRoyalCount);
	if (ActivePlayerScore > OpponentScore + PawnValue)
		return ( IsPlayer1? EWinner::EPlayer1 : EWinner::EPlayer2 );
	else if (OpponentScore > ActivePlayerScore + PawnValue)
		return ( IsPlayer1? EWinner::EPlayer2 : EWinner::EPlayer1);
	return EWinner::ENone;
}

float UCheckersRules::GetWinProbability(const FString& Board, const bool IsPlayer1)
{
	const TCHAR ActivePawn = IsPlayer1? Man : Woman;
	const TCHAR ActiveRoyal = IsPlayer1? King : Queen;
	const TCHAR OpponentPawn = IsPlayer1? Woman : Man;
	const TCHAR OpponentRoyal = IsPlayer1? Queen : King;
	uint8 ActivePawnCount = 0;
	uint8 ActiveRoyalCount = 0;
	uint8 OpponentPawnCount = 0;
	uint8 OpponentRoyalCount = 0;
	// Count each piece type on the board.
	for (uint8 i = 0; i < BoardCellCount; i++)
	{
		if (Board[i] == ActivePawn)
			ActivePawnCount++;
		else if (Board[i] == ActiveRoyal)
			ActiveRoyalCount++;
		else if (Board[i] == OpponentPawn)
			OpponentPawnCount++;
		else if (Board[i] == OpponentRoyal)
			OpponentRoyalCount++;
	}
	/*
		Predictions are based on the idea that a tie game has a 50% chance to win, and any advantage is added to this base.
		Pawns are worth 50% less than royals, and the exact value of each was set by experimentation.
	
		Under the values given below, winning by 2 pawns results in a prediction of a 66% chance to win, from 50 + (8 * 2).
		Likewise, winning by 1 pawn and 1 royal gives a 70% chance to win, from 50 + 8 + 12.
		Values are clamped between [0,1], so a huge advantage and a grossly huge advantage get the same prediction. 
	*/
	constexpr uint8 PawnValue = 8;
	constexpr uint8 RoyalValue = 12;
	const uint8 ActivePlayerScore = (PawnValue * ActivePawnCount) + (RoyalValue * ActiveRoyalCount);
	const uint8 OpponentScore = (PawnValue * OpponentPawnCount) + (RoyalValue * OpponentRoyalCount);

	float Advantage;
	float Prediction;
	if (ActivePlayerScore > OpponentScore)
	{
		Advantage = (ActivePlayerScore - OpponentScore) + 50.0f;
		Prediction = (Advantage >= 100.0f) ? 1.0f : ( Advantage / 100.0f);
	}
	else if (OpponentScore > ActivePlayerScore)
	{
		Advantage = (OpponentScore - ActivePlayerScore) + 50.0f;
		Prediction = (Advantage >= 100.0f) ? 0.0f : ( 1.0f - (Advantage / 100.0f)); 
	}
	else
		Prediction = 0.5f;
	return Prediction;
}

uint8 UCheckersRules::DeriveMovements(const FString& LastBoard, const FString& ChosenNextBoard, const bool IsPlayer1,
	uint8& ChosenTurnIndex)
{
	if (LastBoard.Len() != BoardCellCount || ChosenNextBoard.Len() != BoardCellCount){ return OffBoard; }
	
	const TCHAR ActivePawn = IsPlayer1? Man : Woman;
	const TCHAR ActiveRoyal = IsPlayer1? King : Queen;
	uint8 PieceOrigin = OffBoard;

	/*
	 * Find each piece on the board belonging to the active player.
	 * If any piece is not on the next board (at the same index), it's the origin.
	 * If all pieces are in the same place, then a piece jumped in a circle.
	 * In this case, the origin is the same as the final destination.		
	*/
	
	// Try to find the origin via a board difference. 
	for (uint8 i = 0; i < BoardCellCount; i++)
	{
		if (LastBoard[i] == ActivePawn || LastBoard[i] == ActiveRoyal)
		{
			if (LastBoard[i] != ChosenNextBoard[i])   
			{
				PieceOrigin = i;
				break;
			} 
		}                
	}
	// Stash all movements, and if moves were circular, the last move is also the origin.
	uint8 Index = 0;
	for (const FString& Board : NextPossibleBoards)
	{
		if (Board == ChosenNextBoard)
		{
			if (PieceOrigin == OffBoard)
			{
				PieceOrigin = PossibleTurnMovements[Index].ActionIndices.Last(); 
			}
			// This works because NextPossibleBoards and PossibleTurnMovements are in the same order,
			// so the index of next boards matches the index of next movements.
			ChosenTurnIndex = Index;
			break;
		}
		++Index;
	}
	return PieceOrigin;
}

UCheckersRules::UCheckersRules()
{
	Winner = EWinner::ENone;
}
