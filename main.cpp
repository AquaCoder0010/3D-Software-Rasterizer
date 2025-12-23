#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

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

struct Mesh{
  Triangle *tri_list;
  int tri_count = 0;
};

struct Mat4x4{
  float m[4][4] = {0.f};
};

// 1x4 by 4x4
Vec3 MatMul(Mat4x4 mat, Vec3 input){
  Vec3 output;

  output.x = mat.m[0][0] * input.x + mat.m[1][0] * input.y + mat.m[2][0] * input.z + mat.m[3][0];
  output.y = mat.m[0][1] * input.x + mat.m[1][1] * input.y + mat.m[2][1] * input.z + mat.m[3][1];
  output.z = mat.m[0][2] * input.x + mat.m[1][2] * input.y + mat.m[2][2] * input.z + mat.m[3][2];
  float w = mat.m[0][3] * input.x + mat.m[1][3] * input.y + mat.m[2][3] * input.z + mat.m[3][3];

  return w != 0.f ? Vec3(output.x / w, output.y / w, output.z / w) : Vec3(output.x, output.y, output.z);
}


void printMat4x4(const Mat4x4& mat) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            std::cout << mat.m[row][col] << " ";
        }
        std::cout << '\n';
    }
}
void printTriangle(const Triangle& tri) {
    std::cout << "Triangle vertices:\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "  Vertex " << i << ": ("
                  << tri.points[i].x << ", "
                  << tri.points[i].y << ", "
                  << tri.points[i].z << ")\n";
    }
}


Mesh create_cube_mesh(){
  Mesh currentObj;
  currentObj.tri_count = 12;
  currentObj.tri_list = new Triangle[currentObj.tri_count];

  currentObj.tri_list[0]  = { {{0,0,0}, {0,1,0}, {1,1,0}} };
  currentObj.tri_list[1]  = { {{0,0,0}, {1,1,0}, {1,0,0}} };
  currentObj.tri_list[2]  = { {{1,0,0}, {1,1,0}, {1,1,1}} };
  currentObj.tri_list[3]  = { {{1,0,0}, {1,1,1}, {1,0,1}} };
  currentObj.tri_list[4]  = { {{1,0,1}, {1,1,1}, {0,1,1}} };
  currentObj.tri_list[5]  = { {{1,0,1}, {0,1,1}, {0,0,1}} };
  currentObj.tri_list[6]  = { {{0,0,1}, {0,1,1}, {0,1,0}} };
  currentObj.tri_list[7]  = { {{0,0,1}, {0,1,0}, {0,0,0}} };
  currentObj.tri_list[8]  = { {{0,1,0}, {0,1,1}, {1,1,1}} };
  currentObj.tri_list[9]  = { {{0,1,0}, {1,1,1}, {1,1,0}} };
  currentObj.tri_list[10] = { {{1,0,1}, {0,0,1}, {0,0,0}} };
  currentObj.tri_list[11] = { {{1,0,1}, {0,0,0}, {1,0,0}} };
  
  return currentObj;
}



//
int main() {
  sf::RenderWindow window(sf::VideoMode(600, 400), "title"); 
  sf::Event event;

  float width = 600;
  float height = 400;

  auto cube = create_cube_mesh();
  std::cout << cube.tri_count << std::endl;

  Mat4x4 projection_matrix;
  float aspect_ratio = (float)height / width;
  float fov = 90 * 0.5 / 180 * PI;
  float fov_angle = 1/tan(fov);

  float zNear = 0.1f;
  float zFar = 1000.f;

  projection_matrix.m[0][0] = aspect_ratio * fov_angle;
  projection_matrix.m[1][1] = fov_angle;
  projection_matrix.m[2][2] = zFar /  (zFar - zNear);
  projection_matrix.m[3][2] = (zFar * zNear) /  (zFar - zNear);
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
    std::cout << theta << "  " << timer.asMilliseconds() << std::endl;
    if(timer.asMilliseconds() > 25.f){
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

    for(int i = 0; i < cube.tri_count; i++){
      Triangle curr_tri = cube.tri_list[i];
      Triangle proj_tri;
      float translation_dist = 5.f;

      for(int j = 0; j < 3; j++){
        curr_tri.points[j] = MatMul(rotation_z, curr_tri.points[j]);
        curr_tri.points[j] = MatMul(rotation_x, curr_tri.points[j]);
        
        curr_tri.points[j].z += translation_dist;
        
        proj_tri.points[j] = MatMul(projection_matrix, curr_tri.points[j]);
        proj_tri.points[j].x += 1.f; proj_tri.points[j].y += 1.f; 
        proj_tri.points[j].x *= 0.5f * width; proj_tri.points[j].y *= 0.5f * height;
      }
      sf::Vertex lines[6] = {
        sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y), sf::Color::White),
        sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y), sf::Color::White),
        sf::Vertex(sf::Vector2f(proj_tri.points[1].x, proj_tri.points[1].y), sf::Color::White),
        sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y), sf::Color::White),
        sf::Vertex(sf::Vector2f(proj_tri.points[2].x, proj_tri.points[2].y), sf::Color::White),
        sf::Vertex(sf::Vector2f(proj_tri.points[0].x, proj_tri.points[0].y), sf::Color::White)
      };
      window.draw(lines, 6, sf::Lines);
    }
    window.display();
  }
}