// Copyright Stephen Montague.

#pragma once

class USearchNode;
class UCheckersRules;
enum class EWinner : uint8;

/**
 * Simulation functions for Monte Carlo Tree Search.
 */

uint32 Simulate(TObjectPtr<USearchNode> Child, TObjectPtr<UCheckersRules> SimRules);

EWinner GetSimulationWinner(const FString& Board, const bool bIsPlayer1, TObjectPtr<UCheckersRules> SimRules);

FString GetPredictedNextBoard(bool bSimPlayer1, TObjectPtr<UCheckersRules> SimRules);