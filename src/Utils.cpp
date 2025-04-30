#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <cmath>


namespace PolygonalLibrary{

bool import_mesh(Polygonal_Mesh& mesh)
{
    if(!import_cell0Ds(mesh)) return false;

    if(!import_cell1Ds(mesh)) return false;

    if(!import_cell2Ds(mesh)) return false;

    return true;
}

bool import_cell0Ds(Polygonal_Mesh& mesh)
{
    ifstream ifile("./Cell0Ds.csv");

    if(ifile.fail()) return false;

    list<string> list_lines;

    string line;
    while(getline(ifile, line)) list_lines.push_back(line);

    ifile.close();

    list_lines.pop_front();
    mesh.num_cell0Ds = list_lines.size();

    if (mesh.num_cell0Ds == 0)
    {
        cerr<<"There is no cell 0D"<<endl;
        return false;
    }

    mesh.cell0Ds_id.reserve(mesh.num_cell0Ds);
    mesh.cell0Ds_coordinates = Eigen::MatrixXd::Zero(3,mesh.num_cell0Ds);

    for(const string& line : list_lines)
    {
        istringstream is_line(line);

        unsigned int id;
        unsigned int marker;
        char delimiter; //to storage the ';' of the csv

        is_line>>id>>delimiter>>marker>>delimiter>>mesh.cell0Ds_coordinates(0, id)>>delimiter>>mesh.cell0Ds_coordinates(1, id);

        mesh.cell0Ds_id.push_back(id);

        map<unsigned int, list<unsigned int>>& m = mesh.cell0Ds_markers;
        if(marker != 0)
        {
            auto [itor, bool_val] = m.try_emplace(marker);
            itor -> second.push_back(id);   
        }
        
    }
    
    return true;
}


bool import_cell1Ds(Polygonal_Mesh& mesh)
{
    ifstream ifile("./Cell1Ds.csv");

    if(ifile.fail()) return false;

    list<string> list_lines;

    string line;
    while(getline(ifile, line)) list_lines.push_back(line);

    ifile.close();

    

    list_lines.pop_front();
    mesh.num_cell1Ds = list_lines.size();

    if (mesh.num_cell1Ds == 0)
    {
        cerr<<"There is no cell 1D"<<endl;
        return false;
    }

    mesh.cell1Ds_id.reserve(mesh.num_cell1Ds);
    mesh.cell1Ds_extrema = Eigen::MatrixXi::Zero(2, mesh.num_cell1Ds);

    for(const string& line : list_lines)
    {
        istringstream is_line(line);

        unsigned int indice;
        unsigned int marker;
        char delimiter;

        is_line>>indice>>delimiter>>marker>>delimiter>>mesh.cell1Ds_extrema(0, indice)>>delimiter>>mesh.cell1Ds_extrema(1, indice);
        mesh.cell1Ds_id.push_back(indice);

        map<unsigned int, list<unsigned int>>& m = mesh.cell1Ds_markers;
        if(marker !=0)
        {
            auto [itor, bool_val] = m.try_emplace(marker);
            itor -> second.push_back(indice);   
        }

        //Lunghezza nulla?
        int& origin = mesh.cell1Ds_extrema(0, indice);
        int& end = mesh.cell1Ds_extrema(1, indice);
        const MatrixXd coord = mesh.cell0Ds_coordinates;
        const double X_vertice_corrente = coord(0, origin); //Estraggo le coordinate del vertice corrente
        const double Y_vertice_corrente = coord(1, origin);
        const double X_vertice_successivo = coord(0, end); //Estraggo le coordinate del vertice successivo
        const double Y_vertice_successivo = coord(1, end);
        double distanza = sqrt(((X_vertice_successivo - X_vertice_corrente)*(X_vertice_successivo - X_vertice_corrente)) + (Y_vertice_successivo - Y_vertice_corrente)*(Y_vertice_successivo - Y_vertice_corrente));
        
        if(distanza < 1e-16)
        { 
            cerr<<"Stop: l'arco "<<indice<<" ha lunghezza nulla"<<endl;
            return false;
        }

       
    }

    return true;
}

bool import_cell2Ds(Polygonal_Mesh& mesh)
{
    ifstream ifile("./Cell2Ds.csv");

    if(ifile.fail()) return false;

    list<string> list_lines;

    string line;
    while(getline(ifile, line)) list_lines.push_back(line);

    list_lines.pop_front();
    mesh.num_cell2Ds = list_lines.size();
    if(mesh.num_cell2Ds == 0)
    {
        cerr<<"There is no cell 2D"<<endl;
        return false;
    }

    mesh.cell2Ds_id.reserve(mesh.num_cell2Ds);
    mesh.cell2Ds_vertices.reserve(mesh.num_cell2Ds);
    mesh.cell2Ds_edges.reserve(mesh.num_cell2Ds);
    for(const string& line : list_lines)
    {
        istringstream is_line(line);

        unsigned int indice;
        unsigned int marker;
        unsigned int numero_vertici;
        unsigned int numero_archi;
        char delimiter;

        is_line>>indice>>delimiter>>marker>>delimiter>>numero_vertici;

        vector<unsigned int> vettore_v;
        vettore_v.reserve(numero_vertici);
        for(unsigned int i=0; i<numero_vertici; i++)
        {  
            unsigned int vertice;
            is_line>>delimiter>>vertice;
            vettore_v.push_back(vertice);
        }
        mesh.cell2Ds_vertices.push_back(vettore_v);


        is_line>>delimiter>>numero_archi;

        vector<unsigned int> vec_e;
        vec_e.reserve(numero_archi);
        for(unsigned int j=0; j<numero_archi; j++)
        {
            unsigned int edge;
            is_line>>delimiter>>edge;
            vec_e.push_back(edge);
        }
        mesh.cell2Ds_edges.push_back(vec_e);

        mesh.cell2Ds_id.push_back(indice);

        map<unsigned int, list<unsigned int>>& m = mesh.cell2Ds_markers;
        if(marker != 0)
        {
            auto [itor, bool_val] = m.try_emplace(marker);
            itor -> second.push_back(indice);   
        }

        //Area
        vector<unsigned int>& Size_area = mesh.cell2Ds_vertices[indice];
        const unsigned int n = Size_area.size();

        double area = 0.0;
        for(size_t i=0; i<n; i++)
        {   
            const unsigned int vertice_corrente= Size_area[i];
            const unsigned int vertice_successivo = Size_area[(i+1)%n]; //To close the polygon and connect the first vertice and the last vertice I use j=(i+1)%n

            const MatrixXd coord = mesh.cell0Ds_coordinates;
            const double X_vertice_corrente = coord(0, vertice_corrente); //Estraggo le coordinate del vertice corrente
            const double Y_vertice_corrente = coord(1, vertice_corrente);
            const double X_vertice_successivo = coord(0, vertice_successivo); //Estraggo le coordinate del vertice successivo
            const double Y_vertice_successivo = coord(1, vertice_successivo);
           
            area += (X_vertice_corrente * Y_vertice_successivo) - (X_vertice_successivo * Y_vertice_corrente);
        }
        area = abs(area)*0.5; 

        if(area <= 1e-16)
        {
            cerr<<"Stop: il poligono "<<indice<<" ha area zero"<<endl;
            return false;
        }
        

    }

    return true;
}}