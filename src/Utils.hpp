#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary{

bool import_mesh(Polygonal_Mesh& mesh);

bool import_cell0Ds(Polygonal_Mesh& mesh);

bool import_cell1Ds(Polygonal_Mesh& mesh);

bool import_cell2Ds(Polygonal_Mesh& mesh);

}




