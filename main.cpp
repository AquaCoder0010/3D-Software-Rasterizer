#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
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

int main() {
  sf::RenderWindow window(sf::VideoMode(1000, 1000), "title");
  window.setVerticalSyncEnabled(true);

  sf::Event event;

  float width = window.getSize().x;
  float height = window.getSize().y;

  // mesh information
  Mesh cube = create_cube_mesh();
  sf::VertexArray mesh_vertex(sf::LinesStrip, cube.tri_count * 3);
  sf::CircleShape *mesh_points = new sf::CircleShape[cube.tri_count * 3];

  // projection matrix init
  float aspect_ratio = (float)height / width;
  float fov = 90 * 0.5 / 180 * PI;
  float zNear = 0.1f;
  float zFar = 1000.f;
  Mat4x4 projection_matrix =
      getProjectionMatrix(aspect_ratio, fov, zNear, zFar);

  // rotation matrix
  Mat4x4 rotation_y;
  float theta = 0.f;

  sf::Clock clock;
  sf::Time timer;

  float translation_dist = 3.f;
  float translation_y = 0.f;
  
  constexpr float input_update = 25.f;
  
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
      translation_dist -= 0.01;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
        timer.asMilliseconds() > input_update) {
      translation_dist += 0.01;
      timer = sf::Time::Zero;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
        timer.asMilliseconds() > input_update) {
      translation_y += 0.01;
      timer = sf::Time::Zero;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
        timer.asMilliseconds() > input_update) {
      translation_y -= 0.01;
      timer = sf::Time::Zero;
    }

    update_rotation_matrix_y(rotation_y, theta);
    
    int mesh_vertex_index = 0;
    for (int i = 0; i < cube.tri_count; i++) {
      Triangle curr_tri = cube.tri_list[i];
      Triangle proj_tri;

      for (int j = 0; j < 3; j++) {
        curr_tri.points[j] = MatMul(rotation_y, curr_tri.points[j]);

        curr_tri.points[j].z += translation_dist;
        proj_tri.points[j] = MatMul(projection_matrix, curr_tri.points[j]);

        // normalization
        proj_tri.points[j].x += 1.f;
        proj_tri.points[j].x *= 0.5f * width;

        proj_tri.points[j].y += 1.f;
        proj_tri.points[j].y *= 0.5f;
        proj_tri.points[j].y = (1 - proj_tri.points[j].y);
        proj_tri.points[j].y *= width;
      }
      float radius = 10.f;

      for (int i = 0; i < 3; ++i) {
        mesh_points[mesh_vertex_index + i].setRadius(radius);
        mesh_points[mesh_vertex_index + i].setOrigin(radius, radius);
      }
      mesh_points[mesh_vertex_index + 0].setPosition(proj_tri.points[0].x,
                                                     proj_tri.points[0].y);

      mesh_points[mesh_vertex_index + 1].setPosition(proj_tri.points[1].x,
                                                     proj_tri.points[1].y);

      mesh_points[mesh_vertex_index + 2].setPosition(proj_tri.points[2].x,
                                                     proj_tri.points[2].y);

      mesh_vertex[mesh_vertex_index] =
          sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y),
                     sf::Color::White);

      mesh_vertex[mesh_vertex_index + 1] =
          sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y),
                     sf::Color::White);

      mesh_vertex[mesh_vertex_index + 2] =
          sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y),
                     sf::Color::White);

      mesh_vertex_index += 3;
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

// int main_2() {
//   sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
//   window.setVerticalSyncEnabled(true);

//   sf::CircleShape red(20.f);
//   red.setFillColor(sf::Color::Red);
//   red.setPosition(window.getPosition().x / 2, window.getPosition().y / 2);
//   red.setOrigin(red.getRadius(), red.getRadius());

//   bool mouse_press = false;
//   bool moving = false;

//   while (window.isOpen()) {
//     sf::Event event;
//     while (window.pollEvent(event)) {
//       if (event.type == sf::Event::Closed)
//         window.close();

//       if (event.type == sf::Event::MouseButtonPressed) {
//         if (event.mouseButton.button == sf::Mouse::Left) {
//           mouse_press = true;
//         }
//       }

//       if (event.type == sf::Event::MouseButtonReleased) {
//         if (event.mouseButton.button == sf::Mouse::Left) {
//           mouse_press = false;
//           moving = false;
//         }
//       }
//     }
//     sf::Vector2f mousePos =
//         static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
//     if(red.getGlobalBounds().contains(mousePos) && mouse_press == true){
//       moving = true;
//     }
//     if(moving == true)
//       red.setPosition(mousePos);
//     std::cout << moving <<  "  " << mouse_press << std::endl;

//     window.clear(sf::Color::Black);
//     window.draw(red);
//     window.display();
//   }
// }