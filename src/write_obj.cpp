#include "write_obj.h"
#include <fstream>
#include <cassert>
#include <iostream>

bool write_obj(
  const std::string & filename,
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const Eigen::MatrixXd & UV,
  const Eigen::MatrixXi & UF,
  const Eigen::MatrixXd & NV,
  const Eigen::MatrixXi & NF)
{
  assert((F.size() == 0 || F.cols() == 3 || F.cols() == 4) && "F must have 3 or 4 columns");

  std::ofstream file(filename);
  if(!file){
      std::cout << "Fail to open files" << std::endl;
      return false;
  }

  // v:
  for (int i = 0; i < V.rows(); ++i) {
      file << "v" << " " << V.row(i).x() << " " << V.row(i).y() << " " << V.row(i).z() << std::endl;
  }

  // vertex texture: UV
  for (int j = 0; j < UV.rows(); ++j) {
      file << "vt" << " " << UV.row(j).x() << " " << UV.row(j).y() << std::endl;
  }

  // vertex normal NV:
  for (int k = 0; k < NV.rows(); ++k) {
      file << "vn" << " " << NV.row(k).x() << " " << NV.row(k).y() << " " << NV.row(k).z() << std::endl;
  }

  // Face: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
  for (int l = 0; l < F.rows(); ++l) {
      file << "f" << " ";
      for (int i = 0; i < F.cols(); ++i) {
          file << 1 + F.row(l)[i] << "/" << 1 + UF.row(l)[i] << "/" << 1 + NF.row(l)[i] << " ";
      }
      file << std::endl;
  }

  return true;
}
