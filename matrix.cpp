#include "matrix.hpp"

Mat4x4 Transpose(const Mat4x4 &mat) {
  Mat4x4 out;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      out.m[i][j] = mat.m[j][i];
  return out;
}

Vec3 MatMul(Mat4x4 mat, Vec3 input) {
  Vec3 output;

  output.x = mat.m[0][0] * input.x + mat.m[1][0] * input.y +
             mat.m[2][0] * input.z + mat.m[3][0];
  output.y = mat.m[0][1] * input.x + mat.m[1][1] * input.y +
             mat.m[2][1] * input.z + mat.m[3][1];
  output.z = mat.m[0][2] * input.x + mat.m[1][2] * input.y +
             mat.m[2][2] * input.z + mat.m[3][2];
  float w = mat.m[0][3] * input.x + mat.m[1][3] * input.y +
            mat.m[2][3] * input.z + mat.m[3][3];

  return w != 0.f ? Vec3(output.x / w, output.y / w, output.z / w)
                  : Vec3(output.x, output.y, output.z);
}

Mat4x4 getProjectionMatrix(float aspect_ratio, float fov_angle, float zNear,
                           float zFar) {
  Mat4x4 projection_matrix;
  float fov = 1 / tan(fov_angle);

  projection_matrix.m[0][0] = aspect_ratio * fov;
  projection_matrix.m[1][1] = fov;
  projection_matrix.m[2][2] = zFar / (zFar - zNear);
  projection_matrix.m[3][2] = (zFar * zNear) / (zFar - zNear);
  projection_matrix.m[2][3] = 1;

  return projection_matrix;
}

void update_rotation_matrix_y(Mat4x4 &rotation_y, float theta) {
  rotation_y.m[0][0] = cosf(theta);
  rotation_y.m[0][2] = -sinf(theta);
  rotation_y.m[1][1] = 1.f;
  rotation_y.m[2][0] = sinf(theta);
  rotation_y.m[2][2] = cosf(theta);
  rotation_y.m[3][3] = 1.f;
}

void update_rotation_matrix_x(Mat4x4 &rotation_x, float theta) {
  rotation_x.m[0][0] = 1.f;
  rotation_x.m[1][1] = cosf(theta * 0.5f);
  rotation_x.m[1][2] = sinf(theta * 0.5f);
  rotation_x.m[2][1] = -sinf(theta * 0.5f);
  rotation_x.m[2][2] = cosf(theta * 0.5f);
  rotation_x.m[3][3] = 1.f;
}