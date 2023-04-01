// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MathUtil.h"
#include "Components/PrimitiveComponent.h"
#include "BSplineComponent.generated.h"

/**
 * @brief
 * https://mathworld.wolfram.com/B-Spline.html
 * https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.BSpline.html
 * https://web.mit.edu/hyperbook/Patrikalakis-Maekawa-Cho/node17.html
 * https://ocw.snu.ac.kr/sites/default/files/NOTE/223.pdf
 */
class FMyBSpline
{
public:

	int32 Degree;

	TArray<FVector> ControlPoints;
	TArray<int32> Knots;

	FMyBSpline() {}

	FMyBSpline(const TArray<FVector>& ControlPoints, const int Degree = 3)
	{
		this->Degree = Degree;
		this->ControlPoints = TArray(ControlPoints);

		if (this->ControlPoints.Num() < this->Degree)
		{
			// incorrect
		}
		
		CreateNonPeriodicKnots();
	}

	TArray<FVector> GetPoints()
	{
		TArray<FVector> Result;
		
		for (float x = Knots[Degree]; x < Knots.Last(); x += 0.1f)
		{
			Result.Emplace(GetPoint(x));
		}

		Result.Emplace(GetPoint(Knots.Last()));
		return Result;
	}
	
private:
	
	FVector GetPoint(float x)
	{
		FVector Result = FVector::ZeroVector;

		int32 StartIndex = GetKnotsIndexFromValue(x) - Degree;
		int32 EndIndex = FMath::Min(StartIndex + Degree, ControlPoints.Num() - 1);

		TArray<TArray<float>> CoefficientCaches;
		CoefficientCaches.Init(TArray<float>{}, Degree + 1);
		for (auto& Row : CoefficientCaches)
		{
			Row.Init(-1, 2 * Degree + 1);
		}

		for (int32 i = StartIndex; i <= EndIndex; ++i)
		{
			Result += ControlPoints[i] * BasisFunc(x, StartIndex, Degree, i, Knots, CoefficientCaches);
			UE_LOG(LogTemp, Log, TEXT("Next i"));
		}

		return Result;
	}

	void CreateNonPeriodicKnots()
	{
		int32 KnotsNum = Degree + ControlPoints.Num() + 1;
		Knots.Empty(KnotsNum);
		
		for (int32 i = 0; i < KnotsNum; ++i)
		{
			if (i <= Degree)
				Knots.Emplace(0);
			else if (i >= KnotsNum - Degree - 1)
				Knots.Emplace(KnotsNum - 2 * Degree - 1);
			else
				Knots.Emplace(i - Degree);

			UE_LOG(LogTemp, Log, TEXT("knots %d: %d"), i, Knots.Last());
		}
	}

	int32 GetKnotsIndexFromValue(float KnotValue)
	{
		int32 KnotFloor = FMath::FloorToInt32(KnotValue);
		if (KnotFloor <= 0)
			return Degree;

		if (KnotFloor >= Knots.Num() - 2 * Degree - 1)
			return Knots.Num() - Degree - 1;

		return KnotFloor + Degree;
	}

	float BasisFunc(float x, int32 start, int32 k, int32 i, const TArray<int32>& t, TArray<TArray<float>>& Coefficients)
	{
		int32 remappedI = i - start;
		if (Coefficients[k][remappedI] >= 0.0f)
			return Coefficients[k][remappedI];
		
		if (k == 0)
		{
			if (t[i] <= x && x <= t[i + 1])
			{
				UE_LOG(LogTemp, Log, TEXT("x:%f k:%d i:%d C1+C2=1"), x, k, i);
				Coefficients[k][remappedI] = 1;
				return 1;
			}

			UE_LOG(LogTemp, Log, TEXT("x:%f k:%d i:%d C1+C2=0"), x, k, i);
			Coefficients[k][remappedI] = 0;
			return 0;
		}

		float C1, C2;
		
		if (t[i + k] == t[i])
			C1 = 0;
		else
			C1 = (x - t[i]) / (t[i + k] - t[i]) * BasisFunc(x, start, k - 1, i, t, Coefficients);

		if (t[i + k + 1] == t[i + 1])
			C2 = 0;
		else
			C2 = (t[i + k + 1] - x) / (t[i + k + 1] - t[i + 1]) * BasisFunc(x, start, k - 1, i + 1, t, Coefficients);

		UE_LOG(LogTemp, Log, TEXT("x:%f k:%d i:%d C1+C2=%f"), x, k, i, C1 + C2);
		Coefficients[k][remappedI] = C1 + C2;
		return C1 + C2;
	}
};