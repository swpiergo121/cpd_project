#include "cubes.h"
#include "parser.h"
#include "scanner.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
#define NUM_THREADS 8

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

void draw_curve(node *root, float xmin, float ymin, float zmin, float xmax,
                float ymax, float zmax, vector<vector<float>> &triangles) {
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
      triangles.push_back(edge_intersection[trianglesForCase[i]]);
      triangles.push_back(edge_intersection[trianglesForCase[i + 1]]);
      triangles.push_back(edge_intersection[trianglesForCase[i + 2]]);
    }
  }
}

void marching_cubes(string const &json_object_describing_curve,
                    string const &output_filename, double x_min, double y_min,
                    double x_max, double y_max, double z_min, double z_max,
                    double precision) {

  // parsing
  Scanner *scanner = new Scanner(json_object_describing_curve.c_str());
  Parser *parser = new Parser(scanner);
  node *root;
  try {
    root = parser->parse();
    cout << "Parsing exitoso" << endl << endl;
  } catch (const exception &e) {
    cout << "Error durante la ejecución del parser: " << e.what() << endl;
    exit(0);
  }

  vector<vector<float>> mesh_triangles;

  // Calculate the step for each direction
  int num_cells_x = static_cast<int>((x_max - x_min) / precision);
  int num_cells_y = static_cast<int>((y_max - y_min) / precision);
  int num_cells_z = static_cast<int>((z_max - z_min) / precision);

  double step_x = (x_max - x_min) / num_cells_x;
  double step_y = (y_max - y_min) / num_cells_y;
  double step_z = (z_max - z_min) / num_cells_z;

  // Diferent vectors to join them later
  vector<vector<vector<float>>> thread_local_meshes(NUM_THREADS);

  double t0 = omp_get_wtime();
#pragma omp parallel num_threads(NUM_THREADS)
  {
    int thread_id = omp_get_thread_num();

// Triple loop over the range for the
#pragma omp for schedule(dynamic)
    for (int i = 0; i < num_cells_x; i++) {
      for (int j = 0; j < num_cells_y; j++) {
        for (int k = 0; k < num_cells_z; k++) {
          // Calculate current cell boundaries
          double cell_x_min = x_min + i * step_x;
          double cell_y_min = y_min + j * step_y;
          double cell_z_min = z_min + k * step_z;
          double cell_x_max = cell_x_min + step_x;
          double cell_y_max = cell_y_min + step_y;
          double cell_z_max = cell_z_min + step_z;
          draw_curve(root, cell_x_min, cell_y_min, cell_z_min, cell_x_max,
                     cell_y_max, cell_z_max, thread_local_meshes[thread_id]);
        }
      }
    }
  }
  double t1 = omp_get_wtime();

  for (int i = 0; i < NUM_THREADS; i++) {
    mesh_triangles.insert(mesh_triangles.end(), thread_local_meshes[i].begin(),
                          thread_local_meshes[i].end());
  }

  double t2 = omp_get_wtime();
  double secs_comp = t1 - t0;
  double secs_com = t2 - t1;
  cout << "Tiempo computacional (s)      : " << secs_comp << "\n";
  cout << "Tiempo reduce (s)      : " << secs_com << "\n\n";

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
  float precision = 0.10;

  string json_path = "examples/example1.json";

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
