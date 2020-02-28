#include "per_corner_normals.h"
#include "triangle_area_normal.h"
// Hint:
#include "vertex_triangle_adjacency.h"
#include <iostream>
#include <math.h>

void per_corner_normals(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const double corner_threshold,
  Eigen::MatrixXd & N)
{
  N = Eigen::MatrixXd::Zero(F.rows()*3,3);

  std::vector<std::vector<int> > VF;
  vertex_triangle_adjacency(F, V.rows(), VF);

  for (int i = 0; i < F.rows(); ++i) {

      Eigen::RowVector3d curr_normal = triangle_area_normal(
              V.row(F(i, 0)),
              V.row(F(i, 1)),
              V.row(F(i, 2))).normalized();

      for (int j = 0; j < 3; ++j) {

          Eigen::RowVector3d normal(0,0,0);
          std::vector<int> indexes = VF[F(i, j)];

          for(int index: indexes){

              Eigen::RowVector3d temp_normal(0, 0, 0);
              Eigen::RowVector3d v1 = V.row(F(index, 0));
              Eigen::RowVector3d v2 = V.row(F(index, 1));
              Eigen::RowVector3d v3 = V.row(F(index, 2));

              temp_normal = triangle_area_normal(v1, v2, v3).normalized();

              if((curr_normal.dot(temp_normal) > cos((M_PI * corner_threshold) / 180))){
                  normal += temp_normal;
              }
          }

          N.row(i * 3 + j) = normal.normalized();
      }
  }
}
