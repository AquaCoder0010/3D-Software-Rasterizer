#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <fstream>


constexpr float PI = 3.141592654;

struct Vec3 {
  float x = 0, y = 0, z = 0;
};

struct Vec4 {
  float x = 0, y = 0, z = 0, w = 0;
};

struct Triangle {
  Vec3 points[3];
};

struct Mesh {
  Triangle *tri_list = nullptr;
  int tri_count = 0;
};

struct Mat4x4 {
  float m[4][4] = {0.f};
};

// 1x4 by 4x4
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

void printMat4x4(const Mat4x4 &mat) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      std::cout << mat.m[row][col] << " ";
    }
    std::cout << '\n';
  }
}
void printTriangle(const Triangle &tri) {
  std::cout << "Triangle vertices:\n";
  for (int i = 0; i < 3; i++) {
    std::cout << "  Vertex " << i << ": (" << tri.points[i].x << ", "
              << tri.points[i].y << ", " << tri.points[i].z << ")\n";
  }
}

Mesh create_cube_mesh() {
  Mesh currentObj;
  currentObj.tri_count = 12;
  currentObj.tri_list = new Triangle[currentObj.tri_count];

  currentObj.tri_list[0] = {{{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}};
  currentObj.tri_list[1] = {{{0, 0, 0}, {1, 1, 0}, {1, 0, 0}}};
  currentObj.tri_list[2] = {{{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}};
  currentObj.tri_list[3] = {{{1, 0, 0}, {1, 1, 1}, {1, 0, 1}}};
  currentObj.tri_list[4] = {{{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}};
  currentObj.tri_list[5] = {{{1, 0, 1}, {0, 1, 1}, {0, 0, 1}}};
  currentObj.tri_list[6] = {{{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}};
  currentObj.tri_list[7] = {{{0, 0, 1}, {0, 1, 0}, {0, 0, 0}}};
  currentObj.tri_list[8] = {{{0, 1, 0}, {0, 1, 1}, {1, 1, 1}}};
  currentObj.tri_list[9] = {{{0, 1, 0}, {1, 1, 1}, {1, 1, 0}}};
  currentObj.tri_list[10] = {{{1, 0, 1}, {0, 0, 1}, {0, 0, 0}}};
  currentObj.tri_list[11] = {{{1, 0, 1}, {0, 0, 0}, {1, 0, 0}}};

  return currentObj;
}

void countObj(std::string path, int &vertexCount, int &faceCount) {
  std::ifstream file(path);
  std::string line;

  vertexCount = 0;
  faceCount = 0;

  while (std::getline(file, line)) {
    if (line[0] == 'v' && line[1] == ' ')
      vertexCount++;
    else if (line[0] == 'f' && line[1] == ' ')
      faceCount++;
  }
  file.close();
}

Mesh load_obj(std::string filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    Mesh empty_mesh;
    return empty_mesh;
  }
  int vertexCount, faceCount;
  countObj(filename, vertexCount, faceCount);

  Vec3 *vertices = new Vec3[vertexCount];
  Triangle *triangles = new Triangle[faceCount];

  std::vector<Vec3> temp_vertices;
  std::vector<Triangle> temp_triangles;

  int v_i = 0, t_i = 0;
  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (line[0] == 'v' && line[1] == ' ') {
      Vec3 v;
      ss >> v.x >> v.y >> v.z;

      vertices[v_i] = v;
      v_i++;
    }

    else if (prefix == "f") {
      int vertexIndex[3];
      for (int i = 0; i < 3; i++) {
        std::string vertexData;
        ss >> vertexData;
        vertexIndex[i] = std::stoi(vertexData) - 1;
      }
      Triangle newTri;
      newTri.points[0] = vertices[vertexIndex[0]];
      newTri.points[1] = vertices[vertexIndex[1]];
      newTri.points[2] = vertices[vertexIndex[2]];

      triangles[t_i] = newTri;
      t_i++;
    }
  }

  Mesh resultMesh;
  resultMesh.tri_count = faceCount;
  resultMesh.tri_list = new Triangle[resultMesh.tri_count];

  for (int i = 0; i < resultMesh.tri_count; i++) {
    resultMesh.tri_list[i] = triangles[i];
  }
  return resultMesh;
}

void delete_mesh(Mesh &mesh) {
  delete[] mesh.tri_list;
  mesh.tri_count = 0;
}

Mat4x4 getProjectionMatrix(float aspect_ratio, float fov, float zNear,
                           float zFar) {
  Mat4x4 projection_matrix;
  float fov_angle = 1 / tan(fov);

  projection_matrix.m[0][0] = aspect_ratio * fov_angle;
  projection_matrix.m[1][1] = fov_angle;
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

int main() {
  sf::RenderWindow window(sf::VideoMode(1000, 1000), "title");
  window.setVerticalSyncEnabled(true);

  sf::Event event;

  float width = window.getSize().x;
  float height = window.getSize().y;

  // mesh information
  std::string path =
      std::filesystem::current_path().string() + "//rsrc//cube.obj";
  Mesh cube = load_obj(path);

  sf::VertexArray mesh_vertex(sf::Lines, cube.tri_count * 6);
  sf::CircleShape *mesh_points = new sf::CircleShape[cube.tri_count * 3];
  // bool *mesh_points_state = new bool[cube.tri_count * 3];

  // projection matrix init
  float aspect_ratio = (float)height / width;
  float fov = 90 * 0.5 / 180 * PI;
  float zNear = 0.1f;
  float zFar = 1000.f;
  Mat4x4 projection_matrix =
      getProjectionMatrix(aspect_ratio, fov, zNear, zFar);

  // rotation matrix
  Mat4x4 rotation_y;
  Mat4x4 rotation_x;
  float theta = 0.f;
  float theta_2 = 0.f;

  sf::Clock clock;
  sf::Time timer;

  float translation_dist = 6.f;
  float translation_y = 0.f;

  constexpr float input_update = 1.f;
  float radius = 3.f;
  while (window.isOpen() == true) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    timer += clock.restart();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) &&
        timer.asMilliseconds() > input_update) {
      theta += 0.01;
      timer = sf::Time::Zero;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
        timer.asMilliseconds() > input_update) {
      theta -= 0.01;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&
        timer.asMilliseconds() > input_update) {
      translation_dist -= 0.1;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
        timer.asMilliseconds() > input_update) {
      translation_dist += 0.1;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
        timer.asMilliseconds() > input_update) {
      theta_2 += 0.1;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
        timer.asMilliseconds() > input_update) {
      theta_2 -= 0.1;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
        timer.asMilliseconds() > input_update) {
      translation_y += 0.1;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
        timer.asMilliseconds() > input_update) {
      translation_y -= 0.1;
      timer = sf::Time::Zero;
    }
    update_rotation_matrix_y(rotation_y, theta);
    update_rotation_matrix_x(rotation_x, theta_2 * 0.9);

    int line_index = 0;
    int point_index = 0;
    for (int i = 0; i < cube.tri_count; i++) {
      Triangle curr_tri = cube.tri_list[i];
      Triangle proj_tri;

      for (int j = 0; j < 3; j++) {
        curr_tri.points[j] = MatMul(rotation_y, curr_tri.points[j]);
        curr_tri.points[j] = MatMul(rotation_x, curr_tri.points[j]);

        // translation
        curr_tri.points[j].z += translation_dist;
        curr_tri.points[j].y += translation_y;
        
        // projection
        proj_tri.points[j] = MatMul(projection_matrix, curr_tri.points[j]);

        // normalization and conversion to screen space.
        proj_tri.points[j].x += 1.f;
        proj_tri.points[j].x *= 0.5f * width;

        proj_tri.points[j].y += 1.f;
        proj_tri.points[j].y *= 0.5f;
        proj_tri.points[j].y = (1 - proj_tri.points[j].y);
        proj_tri.points[j].y *= width;
      }

      for (int i = 0; i < 3; ++i) {
        mesh_points[point_index + i].setRadius(radius);
        mesh_points[point_index + i].setOrigin(radius, radius);
      }
      mesh_points[point_index + 0].setPosition(proj_tri.points[0].x,
                                               proj_tri.points[0].y);
      mesh_points[point_index + 1].setPosition(proj_tri.points[1].x,
                                               proj_tri.points[1].y);
      mesh_points[point_index + 2].setPosition(proj_tri.points[2].x,
                                               proj_tri.points[2].y);
      point_index += 3;

      mesh_vertex[line_index + 0] =
          sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y),
                     sf::Color::White);
      mesh_vertex[line_index + 1] =
          sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y),
                     sf::Color::White);

      mesh_vertex[line_index + 2] =
          sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y),
                     sf::Color::White);
      mesh_vertex[line_index + 3] =
          sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y),
                     sf::Color::White);

      mesh_vertex[line_index + 4] =
          sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y),
                     sf::Color::White);
      mesh_vertex[line_index + 5] =
          sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y),
                     sf::Color::White);
      line_index += 6;
    }

    window.clear();
    window.draw(mesh_vertex);
    for (int i = 0; i < cube.tri_count * 3; i++)
      window.draw(mesh_points[i]);
    window.display();
  }
  delete_mesh(cube);
  delete[] mesh_points;
}