#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;


int main()
{   
    Polygonal_Mesh mesh;
    
    if(!import_mesh(mesh))
    {
        cerr<<"Error: file not found"<<endl;
        return 1;
    }

    //markers well stored?
     map<unsigned int, list<unsigned int>>& m_0Ds_markers = mesh.cell0Ds_markers;
     map<unsigned int, list<unsigned int>>& m_1Ds_markers = mesh.cell1Ds_markers;
     map<unsigned int, list<unsigned int>>& m_2Ds_markers = mesh.cell2Ds_markers;

     for(auto& [marker, lista_indici] : m_0Ds_markers)
     {
        cout<<"Marker0D: "<<marker<<"\tIDs = [";
        for(auto& indice : lista_indici)
            cout<<' '<<indice;
        cout<<" ]"<<endl;
     }

     cout<<endl;
     for(auto& [marker, lista_indici] : m_1Ds_markers)
     {
        cout<<"Marker1D: "<<marker<<"  IDs = [";
        for(auto& indice : lista_indici)
            cout<<' '<<indice;
        cout<<" ]"<<endl;
     }

     cout<<endl;
     for(auto& [marker, lista_indici] : m_2Ds_markers)
     {
        cout<<"Marker2D: "<<marker<<"  IDs = [";
        for(auto& indice : lista_indici)
            cout<<' '<<indice;
        cout<<" ]"<<endl;
     }


    //Per visualizzare online le mesh
    Gedim::UCDUtilities utilities;
    utilities.ExportPoints("./Cell0Ds.inp",
                           mesh.cell0Ds_coordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
                             mesh.cell0Ds_coordinates,
                             mesh.cell1Ds_extrema);
    


    return 0;
}