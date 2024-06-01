#pragma once

class UCheckersRules;
class USearchNode;

TObjectPtr<USearchNode> SelectNode(const TObjectPtr<USearchNode> RootNode, const TObjectPtr<UCheckersRules> Rules);
