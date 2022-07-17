// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * This is just an Unreal version implementation from the following video
 * https://www.youtube.com/watch?v=KPoeNZZ6H4s
 */
class TESTEVERYTHING_API FSecondOrderDynamics
{
public:
	/**
	 * @brief 
	 * @param f frequency
	 * @param z damping
	 * @param r initial response
	 * @param x0 start position
	 */
	FSecondOrderDynamics(float f, float z, float r, FVector x0);
	FSecondOrderDynamics();
	~FSecondOrderDynamics();

	/**
	 * @brief 
	 * @param t Delta time
	 * @param x Target position
	 * @return New position
	 */
	FVector Update(float t, FVector x);

	/**
	 * @brief 
	 * @param t Delta time
	 * @param x Target position
	 * @param xd Target velocity
	 * @return New position
	 */
	FVector Update(float t, FVector x, FVector xd);

private:
	FVector Xp;			// previous input
	FVector Y, Yd;		// state variables
	float K1, K2, K3;	// dynamics constants
};
