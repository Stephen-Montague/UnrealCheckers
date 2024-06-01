#pragma once


class UCheckersRules;
class USearchNode;

void Expand(TObjectPtr<USearchNode> Node, TObjectPtr<UCheckersRules> Rules);

bool HasNextState(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules);

void HandleLeaf(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules);

bool IsTie(const TObjectPtr<UCheckersRules> Rules);

bool IsWin(const TObjectPtr<USearchNode> Node, const TObjectPtr<UCheckersRules> Rules);

uint32 CalculateProvenWinReward(uint32 Depth);
