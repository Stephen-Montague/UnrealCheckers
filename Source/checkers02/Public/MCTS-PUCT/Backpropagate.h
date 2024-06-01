// Copyright Stephen Montague.

#pragma once

class USearchNode;

/**
 * BackPropagation functions for Monte Carlo Tree Search.
 */

void BackPropagate(TObjectPtr<USearchNode> SimulatedNode, uint32 Result);
	
