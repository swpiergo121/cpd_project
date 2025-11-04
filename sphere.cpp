#include "sphere.h"

float Fy_sphere(float px, float py, float pz, float center_x, float center_y,
                float center_z, double radius) {
  // Center of the sphere

  float dx = px - center_x;
  float dy = py - center_y;
  float dz = pz - center_z;

  // Return a value where negative is inside the sphere, positive is outside
  return (dx * dx + dy * dy + dz * dz) - (radius * radius);
}
