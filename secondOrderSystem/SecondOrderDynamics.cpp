// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondOrderDynamics.h"

FSecondOrderDynamics::FSecondOrderDynamics()
{
	K1 = K2 = K3 = 0;
}

FSecondOrderDynamics::FSecondOrderDynamics(float f, float z, float r, FVector x0)
{
	const float TwoPIf = 2 * PI * f;

	// compute constants
	K1 = z / (PI * f);
	K2 = 1.0f / (TwoPIf * TwoPIf);
	K3 = r * z / TwoPIf;

	// initialize variables
	Xp = x0;
	Y = x0;
	Yd = FVector::Zero();
}

FSecondOrderDynamics::~FSecondOrderDynamics()
{
}

FVector FSecondOrderDynamics::Update(float t, FVector x)
{
	// estimate velocity
	FVector xd = (x - Xp) / t;
	Xp = x;
	return Update(t, x, xd);
}

FVector FSecondOrderDynamics::Update(float t, FVector x, FVector xd)
{
	float k2Stable = FMath::Max(K2, 1.1f * (t * t / 4.0f + t * K1 / 2.0f));	// clamp K2 to guarantee stability
	Y = Y + t * Yd;																// integrate position by velocity
	Yd = Yd + t * (x + K3 * xd - Y - K1 * Yd) / k2Stable;						// integrate velocity by acceleration
	return Y;
}
