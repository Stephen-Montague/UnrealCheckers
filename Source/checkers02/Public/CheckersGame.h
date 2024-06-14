// Copyright Stephen Montague.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CheckersGame.generated.h"

class UCheckersRules;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CHECKERS02_API ACheckersGame : public APawn
{
	GENERATED_BODY()
	friend class ACheckersAgent;
	
public:
	ACheckersGame();

	bool HasWinner() const;
	
	bool IsOver() const;
	
	UFUNCTION(BlueprintCallable)
	void LogBoard();

	FORCEINLINE void SetInitialBoard(const FString& InBoard){ Board = InBoard; }
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCheckersRules> Rules;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FString Board;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FString LastBoard;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString LogName = TEXT("Checkers");
	
	bool bHasNextPossibleBoard;

	bool bIsDone;
};

