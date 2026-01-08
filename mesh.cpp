#include "mesh.hpp"

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
    std::cerr << "Returning a default Cube mesh .."  << std::endl;
    return create_cube_mesh();
  }
  int vertexCount, faceCount;
  countObj(filename, vertexCount, faceCount);

  Vec3 *vertices = new Vec3[vertexCount];
  Triangle *triangles = new Triangle[faceCount];

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

void save_obj(const Mesh &mesh, std::string filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cout << "Failed to save file!" << std::endl;
    return;
  }

  int global_v_index = 1;
  for (int i = 0; i < mesh.tri_count; i++) {
    for (int j = 0; j < 3; j++) {
      file << "v " << mesh.tri_list[i].points[j].x << " "
           << mesh.tri_list[i].points[j].y << " "
           << mesh.tri_list[i].points[j].z << "\n";
    }
  }

  // 2. Write faces
  file << "\n";
  for (int i = 0; i < mesh.tri_count; i++) {
    file << "f " << global_v_index << " " << global_v_index + 1 << " "
         << global_v_index + 2 << "\n";
    global_v_index += 3;
  }

  file.close();
  std::cout << "Mesh saved to " << filename << std::endl;
}

void delete_mesh(Mesh &mesh) {
  delete[] mesh.tri_list;
  mesh.tri_count = 0;
}
