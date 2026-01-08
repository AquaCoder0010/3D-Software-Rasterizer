#pragma once
#include <cmath>

//
struct Vec3 {
  float x = 0, y = 0, z = 0;
};

struct Mat4x4 {
  float m[4][4] = {{0.f}};
};

Mat4x4 Transpose(const Mat4x4 &mat);
Vec3 MatMul(Mat4x4 mat, Vec3 input);

Mat4x4 getProjectionMatrix(float aspect_ratio, float fov_angle, float zNear,
                           float zFar);

void update_rotation_matrix_y(Mat4x4 &rotation_y, float theta);

void update_rotation_matrix_x(Mat4x4 &rotation_x, float theta);
