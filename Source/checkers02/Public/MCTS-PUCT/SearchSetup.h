// Copyright Stephen Montague.

#pragma once


/**
 * Variables to setup Monte Carlo Tree Search.
 */

constexpr uint32 MaxIterations = 2E6;
constexpr uint32 TreeDepthLimit = 18;
constexpr uint32 SimDepthLimit = 4;
constexpr uint32 WinReward = 2;
constexpr uint32 TieReward = 1;
constexpr uint32 FormulaConstant = 2;
