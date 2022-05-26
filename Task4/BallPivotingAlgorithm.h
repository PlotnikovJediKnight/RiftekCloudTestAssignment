#ifndef BALLPIVOTINGALGORITHM_H
#define BALLPIVOTINGALGORITHM_H

#include <array>
#include <vector>
#include <cmath>
#include <numbers>
#include "DataStructures.h"

struct Triangle : std::array<GeneratedPoint, 3> {
public:

    GeneratedPoint GetNormUnitVector() const {
        float x1 = (*this)[0].x - (*this)[1].x,
              y1 = (*this)[0].y - (*this)[1].y,
              z1 = (*this)[0].z - (*this)[1].z;

        float x2 = (*this)[0].x - (*this)[2].x,
              y2 = (*this)[0].y - (*this)[2].y,
              z2 = (*this)[0].z - (*this)[2].z;

        float xNorm = y1 * z2 - z1 * y2,
              yNorm = z1 * x2 - x1 * z2,
              zNorm = x1 * y2 - y1 * x2;

        float vectorMagnitude = sqrt(xNorm * xNorm + yNorm * yNorm + zNorm * zNorm);

        float xUnit = xNorm / vectorMagnitude,
              yUnit = yNorm / vectorMagnitude,
              zUnit = zNorm / vectorMagnitude;

        return { xUnit,
                 yUnit,
                 zUnit,
                 -1.0,
                 -1.0,
                 -1.0};
    }

};

std::vector<Triangle> DoBallPivotingAlgorithm(const std::vector<GeneratedPoint>& points, float radius);

#endif // BALLPIVOTINGALGORITHM_H
