#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "matrix.hpp"

struct Triangle {
  Vec3 points[3];
};

struct Mesh {
  Triangle *tri_list = nullptr;
  int tri_count = 0;
};

// function to create a default cube mesh
Mesh create_cube_mesh();

// obj functions
void countObj(std::string path, int &vertexCount, int &faceCount);
Mesh load_obj(std::string filename);
void save_obj(const Mesh &mesh, std::string filename);

// function to delete Mesh object
void delete_mesh(Mesh &mesh);
