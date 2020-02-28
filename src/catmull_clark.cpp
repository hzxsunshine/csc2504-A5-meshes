#include "catmull_clark.h"
#include <unordered_map>
#include <utility>
#include <functional>
#include <vector>
#include <unordered_set>
#include <map>
void catmull_clark(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  const int num_iters,
  Eigen::MatrixXd & SV,
  Eigen::MatrixXi & SF)
{
  if (num_iters == 0)
      return;

  // barycentre
  Eigen::RowVector3d barycentre(
          int index_v,
          const Eigen::MatrixXd & V,
          const std::vector<std::pair<int, int>>& edge_maps,
          std::unordered_map<int, Eigen::RowVector3d> face_vertices,
          std::unordered_map<int, std::vector<int>> adjacent_faces);


  // build new V and F:
  void build_new(  const Eigen::MatrixXd & V,
                   const Eigen::MatrixXi & F,
                   std::vector<Eigen::RowVector3d> moved_vertices,
                   const std::vector<std::pair<int, int>>& edge_maps,
                   std::unordered_map<int, Eigen::RowVector3d> face_vertices,
                   std::unordered_map<int, Eigen::RowVector3d> edge_vertices,
                   Eigen::MatrixXd & SV,
                   Eigen::MatrixXi & SF);

  // add face point: F -> face_vertices
  std::unordered_map<int, Eigen::RowVector3d> face_vertices;

  // key is vertex index, vector<int> is to the face index:
  // vertex -> adjacent_faces -> face_vertices
  std::unordered_map<int, std::vector<int>> adjacent_faces;

//  // Edge points: vertex -> in which face -> 2 edge_index
//  std::unordered_map<int, std::vector<int>> edges;

  // pair< vertex, vertex> first vertex has smaller index
  // pair -> midpoint index in SV.
  std::vector<std::pair<int,int>> edge_maps;

  std::unordered_map<int, Eigen::RowVector3d> edge_vertices;

  // need information: edges in which 2 faces:
  std::vector<std::pair<int, int>> edge_faces;


  // Set the face point for each facet to be the average of its vertices
  for (int i = 0; i < F.rows(); ++i) {
      Eigen::RowVector3d face_v(0, 0, 0);
      for (int j = 0; j < F.cols(); ++j) {
          face_v += V.row(F(i, j));
          adjacent_faces[F(i,j)].push_back(i);
      }
      face_vertices[i] = face_v / F.cols();
  }

  // edge pairs and link each edge to faces
  for (int i = 0; i < F.rows(); i++){
      for(int j = 0; j < F.cols(); j++){
          std::pair<int, int> pair = std::make_pair(F(i, j % 4), F(i, (j + 1) % 4));
          std::pair<int, int> reverse_pair = std::make_pair(F(i, (j + 1) % 4), F(i, j % 4));

          auto found_1 = std::find(edge_maps.begin(), edge_maps.end(), pair);
          auto found_2 = std::find(edge_maps.begin(), edge_maps.end(), reverse_pair);

          int id;

          if(found_1 != edge_maps.end()){
              id = found_1 - edge_maps.begin();
              auto faces = edge_faces[id];
              edge_faces[id] = std::make_pair(faces.first, i);
          } else if (found_2 != edge_maps.end()){
              id = found_2 - edge_maps.begin();
              auto faces = edge_faces[id];
              edge_faces[id] = std::make_pair(faces.first, i);
          } else{
              // pair is not existed
              edge_maps.push_back(pair);
              edge_faces.emplace_back(i, -1);
          }
      }
  }

  // edge vertices:
  for (int i = 0; i < edge_maps.size(); ++i) {
        int face_1 = edge_faces[i].first;
        int face_2 = edge_faces[i].second;
        edge_vertices[i] = (face_vertices[face_1] + face_vertices[face_2] + V.row(edge_maps[i].first) + V.row(edge_maps[i].second))/4;
  }

  // calculate barycentre:
  std::vector<Eigen::RowVector3d> moved_vertices;
  moved_vertices.reserve(V.rows());
  for (int l = 0; l < V.rows(); ++l) {
      moved_vertices.push_back(barycentre(l, V, edge_maps, face_vertices, adjacent_faces));
  }

  // build SV and SF
  build_new(V, F, moved_vertices, edge_maps, face_vertices, edge_vertices, SV, SF);

  // recursively
  catmull_clark(SV, SF, num_iters - 1, SV, SF);

}



Eigen::RowVector3d barycentre(
  int index_v,
  const Eigen::MatrixXd & V,
  const std::vector<std::pair<int, int>>& edge_maps,
  std::unordered_map<int, Eigen::RowVector3d> face_vertices,
  std::unordered_map<int, std::vector<int>> adjacent_faces)
{
  // old_vertex
  Eigen::RowVector3d P = V.row(index_v);

  // Average of all created face points adjacent to P
  Eigen::RowVector3d F(0, 0, 0);
  // number of face points adjacent to P
  double n = adjacent_faces[index_v].size();

  for (int &itr: adjacent_faces[index_v]) {
      F += face_vertices[itr];
  }
  F = F / n;

  // Average of all original edge midpoints touching P
  Eigen::RowVector3d R(0, 0, 0);
  for (auto edge: edge_maps){
      if (index_v == edge.first || index_v == edge.second){
          R += (V.row(edge.first) + V.row(edge.second)) / 2.0;
      }
  }
  R = R / n;

  return (F + (2.0 * R) + (n - 3.0) * P) / n;
}



void build_new(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  std::vector<Eigen::RowVector3d> moved_vertices,
  const std::vector<std::pair<int, int>>& edge_maps,
  std::unordered_map<int, Eigen::RowVector3d> face_vertices,
  std::unordered_map<int, Eigen::RowVector3d> edge_vertices,
  Eigen::MatrixXd & SV,
  Eigen::MatrixXi & SF)
{
  // build new Vertices and facets:
  // Always two for convex polyhedra, but!!!!! meshes might not be convex!!!!
  // it can have a hole
  int edge_num = edge_maps.size();
  // int edge_num = edge_maps.size();
  Eigen::MatrixXd new_SV;
  Eigen::MatrixXi new_SF;
  new_SV.resize(V.rows() + F.rows() + edge_num, 3);
  new_SF.resize(F.rows() * F.cols(), 4);


  // fill SV
  for (int i = 0; i < V.rows(); ++i) {
      new_SV.row(i) = moved_vertices.at(i);
  }

  for (int j = V.rows(), i =0; j < V.rows() + F.rows(); ++j, ++i) {
      new_SV.row(j) = face_vertices[i];
  }

  for (int m = V.rows() + F.rows(), i = 0;
        m < V.rows() + F.rows() + edge_num; ++i, ++m) {
      new_SV.row(m) = edge_vertices[i];
  }


  // fill SF:
  int face_index = 0;
  for (int l = 0; l < F.rows(); ++l) {
      for (int i = 0; i < F.cols(); ++i) {
          Eigen::RowVector4i face(0, 0, 0, 0);

          std::pair<int, int> pair_1 = std::make_pair(F(l, i % 4), F(l, (i + 1) % 4));
          std::pair<int, int> pair_reverse_1 = std::make_pair(F(l, (i + 1) % 4), F(l, i % 4));

          std::pair<int, int> pair_2 = std::make_pair(F(l, i % 4), F(l, (i + 3) % 4));
          std::pair<int, int> pair_reverse_2 = std::make_pair(F(l, (i + 3) % 4), F(l, i % 4));

          int position_1 = 0;
          int position_2 = 0;
          for (int k = 0; k < edge_maps.size(); k++){
              if(edge_maps[k] == pair_1 || edge_maps[k] == pair_reverse_1){
                  position_1 = k;
              }
              if(edge_maps[k] == pair_2 || edge_maps[k] == pair_reverse_2){
                  position_2 = k;
              }
          }
          face.x() = F(l, i);

          face.y() = V.rows() + F.rows() + position_1;
          face.z() = V.rows() + l;
          // edge: previous vertex
          face.w() = V.rows() + F.rows() + position_2;

          new_SF.row(face_index) = face;
          face_index++;
      }
  }
  SV = new_SV;
  SF = new_SF;
}

