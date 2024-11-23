//
//  PerspectiveCamera.hpp
//  SnowSoup
//
//  Created by Guilherme de Souza Barci on 27/08/24.
//

#ifndef PerspectiveCamera_hpp
#define PerspectiveCamera_hpp

#include "../Cameras/Camera.hpp"

class PerspectiveCamera : public Camera{
    public:
    inline PerspectiveCamera(){};
    float fieldOfView = 60.f;
    simd_float4x4 ProjectionMatrix() override{
        simd::float4x4 projection;

        
        float ys = 1 / tanf((fieldOfView * M_PI / 180.f) * 0.5);
        float xs = ys / aspectRatio;
        float zs = far / (far - near);

        simd::float4 X = { xs, 0, 0, 0 };
        simd::float4 Y = { 0, ys, 0, 0 };
        simd::float4 Z = { 0, 0, zs,  1};
        simd::float4 W = { 0, 0, -near * zs, 0 };

        projection.columns[0] = X;
        projection.columns[1] = Y;
        projection.columns[2] = Z;
        projection.columns[3] = W;

        return projection;
    };
};

#endif /* PerspectiveCamera_hpp */
