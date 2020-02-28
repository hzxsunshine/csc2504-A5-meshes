#include "sphere.h"
#include <iostream>
#include <math.h>

void sphere(
  const int num_faces_u,
  const int num_faces_v,
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F,
  Eigen::MatrixXd & UV,
  Eigen::MatrixXi & UF,
  Eigen::MatrixXd & NV,
  Eigen::MatrixXi & NF)
{

  int total_faces = num_faces_u * num_faces_v;
  int total_vertices = (num_faces_u + 1) * (num_faces_v + 1);
  double pi = std::atan(1) * 4;
  double theta = pi / num_faces_u;
  double phi = 2 * pi / num_faces_v;
  double r = 1.0;

  // V:
  V.resize(total_vertices , 3);
  NV.resize(total_vertices, 3);
  UV.resize(total_vertices, 2);
  for (int i = 0; i <= num_faces_u; ++i) {
      for (int j = 0; j <= num_faces_v; ++j) {

          double x = r * cos(j*phi) * sin(i*theta);
          double y = r * sin(j*phi) * sin(i*theta);
          double z = r * cos(i*theta);

          double para_y = (double) i / (num_faces_u + 1);
          double para_x = (double) j / (num_faces_v + 1);

          int row_index = i*(num_faces_v +1) + j;

          V.row(row_index) = Eigen::RowVector3d(x, y, z);
          NV.row(row_index) = Eigen::RowVector3d(x, y, z);
          UV.row(row_index) = Eigen::RowVector2d(para_x, para_y);
      }
  }

  // F:
  F.resize(total_faces, 4);
  NF.resize(total_faces, 4);
  UF.resize(total_faces, 4);
  for (int k = 0; k < num_faces_u; ++k) {
      for (int i = 0; i < num_faces_v; ++i) {

          int x = k * (num_faces_v + 1) + i;
          int y =  (k + 1) * (num_faces_v + 1) + i ;
          int z =  y + 1;
          int w =  x + 1;

          int row_index = k * num_faces_v + i;

          F.row(row_index) = Eigen::Vector4i(x, y, z, w);
          NF.row(row_index) = Eigen::Vector4i(x, y, z, w);
          UF.row(row_index) = Eigen::Vector4i(x, y, z, w);

      }
  }

}
