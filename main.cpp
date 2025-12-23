#include <filesystem>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <cmath>
#include <iostream>

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
  Triangle *tri_list;
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

void countObj(const char *path, int &vertexCount, int &faceCount) {
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
}

Mesh loadObj(const char *path) {
  int vertexCount = 0;
  int faceCount = 0;

  countObj(path, vertexCount, faceCount);

  Vec3 *vertices = new Vec3[vertexCount];
  Triangle *triangles = new Triangle[faceCount];

  std::ifstream file(path);
  std::string line;

  int vIndex = 0;
  int fIndex = 0;

  while (std::getline(file, line)) {
    if (line[0] == 'v' && line[1] == ' ') {
      std::istringstream iss(line);
      char type;

      iss >> type >> vertices[vIndex].x >> vertices[vIndex].y >>
          vertices[vIndex].z;

      ++vIndex;
    }

    else if (line[0] == 'f' && line[1] == ' ') {
      std::istringstream iss(line);
      char type;
      int a, b, c;

      iss >> type >> a >> b >> c;

      triangles[fIndex].points[0] = vertices[a - 1];
      triangles[fIndex].points[1] = vertices[b - 1];
      triangles[fIndex].points[2] = vertices[c - 1];

      ++fIndex;
    }
  }
  Vec3 min = vertices[0], max = vertices[0];
  
  for (int i = 1; i < vertexCount; i++) {
    if (vertices[i].x < min.x)
      min.x = vertices[i].x;
    if (vertices[i].y < min.y)
      min.y = vertices[i].y;
    if (vertices[i].z < min.z)
      min.z = vertices[i].z;

    if (vertices[i].x > max.x)
      max.x = vertices[i].x;
    if (vertices[i].y > max.y)
      max.y = vertices[i].y;
    if (vertices[i].z > max.z)
      max.z = vertices[i].z;
  }

  Vec3 center = {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f,
                 (min.z + max.z) * 0.5f};
  float scale = std::max({max.x - min.x, max.y - min.y, max.z - min.z});
  if (scale != 0.f)
    scale = 1.f / scale;

  for (int i = 0; i < vertexCount; i++) {
    vertices[i].x = (vertices[i].x - center.x) * scale;
    vertices[i].y = (vertices[i].y - center.y) * scale;
    vertices[i].z = (vertices[i].z - center.z) * scale;
  }

  // Update triangles with normalized vertices
  for (int i = 0; i < faceCount; i++) {
    triangles[i].points[0].x = (triangles[i].points[0].x - center.x) * scale;
    triangles[i].points[0].y = (triangles[i].points[0].y - center.y) * scale;
    triangles[i].points[0].z = (triangles[i].points[0].z - center.z) * scale;

    triangles[i].points[1].x = (triangles[i].points[1].x - center.x) * scale;
    triangles[i].points[1].y = (triangles[i].points[1].y - center.y) * scale;
    triangles[i].points[1].z = (triangles[i].points[1].z - center.z) * scale;

    triangles[i].points[2].x = (triangles[i].points[2].x - center.x) * scale;
    triangles[i].points[2].y = (triangles[i].points[2].y - center.y) * scale;
    triangles[i].points[2].z = (triangles[i].points[2].z - center.z) * scale;
  }

  delete[] vertices;

  Mesh mesh;
  mesh.tri_list = triangles;
  mesh.tri_count = faceCount;

  return mesh;
}




int main() {
  sf::RenderWindow window(sf::VideoMode(1000,1000), "title");
  sf::Event event;

  float width = window.getSize().x;
  float height = window.getSize().y;

  std::string path = std::filesystem::current_path().string() + "//rsrc//Lagtrain.obj";
  auto cube = loadObj(path.c_str());
  std::cout << cube.tri_count << std::endl;

  Mat4x4 projection_matrix;
  float aspect_ratio = (float)height / width;
  float fov = 90 * 0.5 / 180 * PI;
  float fov_angle = 1 / tan(fov);

  float zNear = 0.1f;
  float zFar = 1000.f;

  projection_matrix.m[0][0] = aspect_ratio * fov_angle;
  projection_matrix.m[1][1] = fov_angle;
  projection_matrix.m[2][2] = zFar / (zFar - zNear);
  projection_matrix.m[3][2] = (zFar * zNear) / (zFar - zNear);
  projection_matrix.m[2][3] = 1;

  Mat4x4 rotation_z;
  Mat4x4 rotation_x;

  sf::Clock clock;
  sf::Time timer;
  float theta = 0.f;

  while (window.isOpen() == true) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    timer += clock.restart();
    if (timer.asMilliseconds() > 25.f) {
      theta += 0.02;
      timer = sf::Time::Zero;
    }
    window.clear();

    rotation_z.m[0][0] = cosf(theta);
    rotation_z.m[0][1] = sinf(theta);
    rotation_z.m[1][0] = -sinf(theta);
    rotation_z.m[1][1] = cosf(theta);
    rotation_z.m[2][2] = 1.f;
    rotation_z.m[3][3] = 1.f;

    rotation_x.m[0][0] = 1.f;
    rotation_x.m[1][1] = cosf(theta * 0.5f);
    rotation_x.m[1][2] = sinf(theta * 0.5f);
    rotation_x.m[2][1] = -sinf(theta * 0.5f);
    rotation_x.m[2][2] = cosf(theta * 0.5f);
    rotation_x.m[3][3] = 1.f;

    for (int i = 0; i < cube.tri_count; i++) {
      Triangle curr_tri = cube.tri_list[i];
      Triangle proj_tri;
      float translation_dist = 1.f;

      for (int j = 0; j < 3; j++) {
        curr_tri.points[j] = MatMul(rotation_z, curr_tri.points[j]);
        curr_tri.points[j] = MatMul(rotation_x, curr_tri.points[j]);

        curr_tri.points[j].z += translation_dist;

        proj_tri.points[j] = MatMul(projection_matrix, curr_tri.points[j]);
        proj_tri.points[j].x += 1.f;
        proj_tri.points[j].y += 1.f;
        proj_tri.points[j].x *= 0.5f * width;
        proj_tri.points[j].y *= 0.5f * height;
      }
      sf::Vertex lines[6] = {
          sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y),
                     sf::Color::White)};
      window.draw(lines, 6, sf::Points);
    }
    window.display();
  }
}

