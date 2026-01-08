#include <SFML/Graphics.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <filesystem>

#include "matrix.hpp"
#include "mesh.hpp"
#include "ops.hpp"

int main() {
  std::cout << "Please enter obj path directory " << std::endl;
  std::string relative_dir;
  std::getline(std::cin, relative_dir);

  // mesh information
  std::string path =
      std::filesystem::current_path().string() + "//" + relative_dir;
  Mesh cube = load_obj(path);

  std::cout << "Controls:\n";
  std::cout << "W/S - Move forward/back\n";
  std::cout << "A/D - Rotate X axis\n";
  std::cout << "Q/E - Rotate Y axis\n";
  std::cout << "Arrow keys - Move vertically\n";
  std::cout << "Press ENTER to start...\n";
  std::cin.get();

  sf::RenderWindow window(sf::VideoMode(800, 600), "title", sf::Style::Default);
  window.setVerticalSyncEnabled(true);

  sf::Event event;

  float width = window.getSize().x;
  float height = window.getSize().y;

  // objects for view
  sf::VertexArray mesh_vertex(sf::Lines, cube.tri_count * 6);
  sf::CircleShape *mesh_points = new sf::CircleShape[cube.tri_count * 3];
  float radius = 3.f;

  // projection matrix parameters
  float aspect_ratio = (float)height / width;
  float fov = 90 * 0.5 / 180 * PI;
  float zNear = 0.1f;
  float zFar = 1000.f;
  Mat4x4 projection_matrix =
      getProjectionMatrix(aspect_ratio, fov, zNear, zFar);

  // rotation matrix
  Mat4x4 rotation_y;
  Mat4x4 rotation_x;

  float theta_y = 0.f;
  float theta_x = 0.f;

  sf::Clock clock;
  sf::Time timer;

  float translation_dist = 6.f;
  float translation_y = 0.f;

  constexpr float input_update = 1.f;

  IndexPair *pair_list = new IndexPair[cube.tri_count * 3];
  int pair_list_total_size = 0;

  bool isDragging = false;
  sf::Vector2f prevMousePos;

  while (window.isOpen() == true) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::MouseButtonPressed &&
          event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        isDragging = false;

        int clickedIndex = -1;
        for (int i = 0; i < cube.tri_count * 3; i++) {
          if (mesh_points[i].getGlobalBounds().contains(mousePos)) {
            clickedIndex = i;
            break;
          }
        }

        if (clickedIndex != -1) {
          isDragging = true;
          prevMousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

          int targetTriIndex = clickedIndex / 3;
          int targetVertIndex = clickedIndex % 3;
          Vec3 targetPos =
              cube.tri_list[targetTriIndex].points[targetVertIndex];

          int pair_list_index = 0;
          for (int t = 0; t < cube.tri_count; t++) {
            for (int v = 0; v < 3; v++) {
              Vec3 p = cube.tri_list[t].points[v];

              // distance calculation
              if (DistSq(p.x, p.y, targetPos.x, targetPos.y) +
                      DistSq(p.z, 0, targetPos.z, 0) <
                  0.0001f) {
                pair_list[pair_list_index].first = t;
                pair_list[pair_list_index].second = v;
                pair_list_index += 1;
              }
            }
          }
          pair_list_total_size = pair_list_index;
        }
      }
      if (event.type == sf::Event::MouseButtonReleased &&
          event.mouseButton.button == sf::Mouse::Left) {
        isDragging = false;
      }

      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::P) {
        std::string filename = "output_mesh.obj";
        save_obj(cube, filename);
        std::cout << "Saved " << filename << " on current working directory";
      }
    }
    timer += clock.restart();

    if (isDragging && pair_list_total_size != 0) {
      sf::Vector2f currentMousePos =
          (sf::Vector2f)sf::Mouse::getPosition(window);
      float dx = (float)(currentMousePos.x - prevMousePos.x);
      float dy =
          (float)(currentMousePos.y - prevMousePos.y); // Screen Y is down

      prevMousePos = currentMousePos;

      // sensitivity factor
      float sens = 0.01f;

      Vec3 moveDelta = {dx * sens, -dy * sens, 0.0f};

      // inverse of a rotation matrix is it's transpose.

      Mat4x4 invRotX = Transpose(rotation_x);
      Mat4x4 invRotY = Transpose(rotation_y);

      Vec3 modelDelta = MatMul(invRotX, moveDelta);
      modelDelta = MatMul(invRotY, modelDelta);

      // Apply to all selected vertices
      for (int i = 0; i < pair_list_total_size; i++) {
        int t = pair_list[i].first;
        int v = pair_list[i].second;
        cube.tri_list[t].points[v].x += modelDelta.x;
        cube.tri_list[t].points[v].y += modelDelta.y;
        cube.tri_list[t].points[v].z += modelDelta.z;
      }
    }

    // keyboard logic
    if (timer.asMilliseconds() > input_update) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        theta_y += 0.02;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        theta_y -= 0.02;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        translation_dist -= 0.1;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        translation_dist += 0.1;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        theta_x += 0.02;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        theta_x -= 0.02;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        translation_y += 0.1;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        translation_y -= 0.1;
      timer = sf::Time::Zero;
    }

    update_rotation_matrix_y(rotation_y, theta_y);
    // multiply by 0.9 to avoid gimbal lock
    update_rotation_matrix_x(rotation_x, theta_x * 0.9);

    int line_index = 0;
    int point_index = 0;
    for (int i = 0; i < cube.tri_count; i++) {
      Triangle curr_tri = cube.tri_list[i];
      Triangle proj_tri;

      for (int j = 0; j < 3; j++) {
        // 3D space rotation
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

        // we use the formula 1 - (c + 1)*0.5 because SFML uses a matrix
        // coordinate space (Y increases to the left)
        proj_tri.points[j].y += 1.f;
        proj_tri.points[j].y *= 0.5f;
        proj_tri.points[j].y = (1 - proj_tri.points[j].y);
        proj_tri.points[j].y *= width;
      }

      for (int i = 0; i < 3; ++i) {
        mesh_points[point_index + i].setRadius(radius);
        mesh_points[point_index + i].setOrigin(radius, radius);
        mesh_points[point_index + i].setPosition(proj_tri.points[i].x,
                                                 proj_tri.points[i].y);
      }
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
  delete[] pair_list;
}