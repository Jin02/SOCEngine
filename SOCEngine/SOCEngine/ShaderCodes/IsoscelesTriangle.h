#ifndef __ISOSCELES_TRIANGLE_H__
#define __ISOSCELES_TRIANGLE_H__

float IsoscelesTriangleOpposite(float adjacentLength, float coneTheta)
{
	// 간단한 삼각함수 공식들
	// tan(theta) = opp / adj고
	// opp = tan(theta) * adj니까, 그리고 이등변 삼각형이니 2를 곱해주어야 함.

	return 2.0f * tan(coneTheta) * adjacentLength;
}

float IsoscelesTriangleInRadius(float a, float h)
{
	float a2	= a * a;
	float four_h2	= 4.0f * h * h;

	return (a * (sqrt(a2 + four_h2) - a)) / (4.0f * h);
}

float IsosclesTriangleNextAdjacent(float adjLength, float inRadius)
{
	return adjLength - (inRadius * 2.0f);
}


#endif
