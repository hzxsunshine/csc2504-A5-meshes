#include "per_vertex_normals.h"
#include "triangle_area_normal.h"

void per_vertex_normals(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  Eigen::MatrixXd & N)
{
  N = Eigen::MatrixXd::Zero(V.rows(),3);

  for (int j = 0; j < V.rows(); ++j) {
      Eigen::RowVector3d normal(0, 0, 0);
      for (int i = 0; i < F.rows(); ++i) {

          if((j == F.row(i).x())||(j == F.row(i).y())||(j == F.row(i).z())){

              Eigen::RowVector3d v1 = V.row(F(i, 0));
              Eigen::RowVector3d v2 = V.row(F(i, 1));
              Eigen::RowVector3d v3 = V.row(F(i, 2));

              normal += triangle_area_normal(v1, v2, v3).normalized();

          }
      }
      N.row(j) = normal.normalized();
  }
}
