#include "cubes.h"
#include "parser.h"
#include "scanner.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<float> interpolate(vector<float> const &p1, vector<float> const &p2,
                          float val1, float val2) {
  if (abs(0 - val1) < 1e-6)
    return p1;
  if (abs(0 - val2) < 1e-6)
    return p2;
  if (abs(val1 - val2) < 1e-6)
    return p1; // Avoid division by zero

  float mu = (0 - val1) / (val2 - val1);
  return {p1[0] + mu * (p2[0] - p1[0]), p1[1] + mu * (p2[1] - p1[1]),
          p1[2] + mu * (p2[2] - p1[2])};
}

bool draw_curve(node *root, float xmin, float ymin, float zmin, float xmax,
                float ymax, float zmax, float precision,
                vector<vector<float>> &triangles) {

  float current_dx = xmax - xmin;
  float current_dy = ymax - ymin;
  float current_dz = zmax - zmin;

  if (current_dx < precision || current_dy < precision ||
      current_dz < precision) {
    return true;
  }

  vector<vector<float>> vertex_coordinations = {
      {xmin, ymin, zmin}, {xmax, ymin, zmin}, {xmin, ymax, zmin},
      {xmax, ymax, zmin}, {xmin, ymin, zmax}, {xmax, ymin, zmax},
      {xmin, ymax, zmax}, {xmax, ymax, zmax}};

  vector<float> vertex_values(8);
  for (int i = 0; i < 8; ++i) {
    vertex_values[i] =
        root->evaluate(vertex_coordinations[i][0], vertex_coordinations[i][1],
                       vertex_coordinations[i][2]);
  }

  // Case
  // cout << "Inside" << endl;
  int cube_index = 0;
  for (int i = 0; i < 8; ++i) {
    // cout << "vertex " << i << ": " << vertex_values[i] << endl;
    if (vertex_values[i] < 0.0) {
      cube_index |= (1 << i); // Set the i-th bit if vertex is "inside"
    }
  }

  float midX = (xmin + xmax) / 2.0;
  float midY = (ymin + ymax) / 2.0;
  float midZ = (zmin + zmax) / 2.0;

  // Inserts the triangle if the case isn't the basic one
  if (cube_index != 0 && cube_index != 255) {
    vector<vector<float>> edge_intersection(12);
    int edgeMask = EdgeMasks[cube_index];

    for (int i = 0; i < 12; ++i) {
      if ((edgeMask >> i) & 1) { // Check if this edge is intersected
        int v0_idx = EdgeVertexIndices[i][0];
        int v1_idx = EdgeVertexIndices[i][1];
        edge_intersection[i] = interpolate(
            vertex_coordinations[v0_idx], vertex_coordinations[v1_idx],
            vertex_values[v0_idx], vertex_values[v1_idx]);
      }
    }

    const vector<int> &trianglesForCase = TriangleTable[cube_index];
    for (size_t i = 0; trianglesForCase[i] != -1; i += 3) {
      auto s = edge_intersection[trianglesForCase[i]];
      triangles.push_back(edge_intersection[trianglesForCase[i]]);
      triangles.push_back(edge_intersection[trianglesForCase[i + 1]]);
      triangles.push_back(edge_intersection[trianglesForCase[i + 2]]);
    }
  }

  // Recursively call for each of the 8 sub-cubes
  draw_curve(root, xmin, ymin, zmin, midX, midY, midZ, precision, triangles);
  draw_curve(root, midX, ymin, zmin, xmax, midY, midZ, precision, triangles);
  draw_curve(root, xmin, midY, zmin, midX, ymax, midZ, precision, triangles);
  draw_curve(root, midX, midY, zmin, xmax, ymax, midZ, precision, triangles);
  draw_curve(root, xmin, ymin, midZ, midX, midY, zmax, precision, triangles);
  draw_curve(root, midX, ymin, midZ, xmax, midY, zmax, precision, triangles);
  draw_curve(root, xmin, midY, midZ, midX, ymax, zmax, precision, triangles);
  draw_curve(root, midX, midY, midZ, xmax, ymax, zmax, precision, triangles);

  return false;
}

void marching_cubes(string const &json_object_describing_curve,
                    string const &output_filename, double x_min, double y_min,
                    double x_max, double y_max, double z_min, double z_max,
                    double precision) {

  // parsing
  Scanner scanner(json_object_describing_curve.c_str());
  Parser parser(&scanner);
  node *root;
  try {
    root = parser.parse();
    cout << "Parsing exitoso" << endl << endl;
  } catch (const exception &e) {
    cout << "Error durante la ejecución del parser: " << e.what() << endl;
    exit(0);
  }

  vector<vector<float>> mesh_triangles;

  draw_curve(root, x_min, y_min, z_min, x_max, y_max, z_max, precision,
             mesh_triangles);

  // Output
  ofstream ply_file(output_filename);
  if (!ply_file.is_open()) {
    cerr << "Error: Could not open output file " << json_object_describing_curve
         << endl;
    exit(0);
  }

  ply_file << "ply\n";
  ply_file << "format ascii 1.0\n";
  ply_file << "element vertex " << mesh_triangles.size() << "\n";
  ply_file << "property float x\n";
  ply_file << "property float y\n";
  ply_file << "property float z\n";
  ply_file << "element face " << mesh_triangles.size() / 3 << "\n";
  ply_file << "property list uchar int vertex_indices\n";
  ply_file << "end_header\n";

  for (const auto &v : mesh_triangles) {
    ply_file << v[0] << " " << v[1] << " " << v[2] << "\n";
  }

  for (size_t i = 0; i < mesh_triangles.size(); i += 3) {
    ply_file << "3 " << i << " " << i + 1 << " " << i + 2 << "\n";
  }

  ply_file.close();
  cout << "Generated " << mesh_triangles.size() / 3 << " triangles to "
       << json_object_describing_curve << endl;
}

int main() {
  // Honestly, it doesn't work that well. For union, the conections aren't very
  // precise. For intersection, there is a weird conversion of the biggest
  // sphere. For diff, it isn't very precise on the extremes
  string output_file = "out.ply";
  float xmin = -5, ymin = -5, zmin = -5;
  float xmax = 20, ymax = 20, zmax = 20;
  float precision = 0.05;

  string json_path = "example2.json";

  ifstream infile(json_path);
  string input;
  string line;
  while (getline(infile, line)) {
    input += line + '\n';
  }
  infile.close();

  marching_cubes(input, output_file, xmin, ymin, xmax, ymax, zmin, zmax,
                 precision);

  return 0;
}
