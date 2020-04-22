#include "../include/mostro.h"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <fstream>
#include <iostream>
#include <unistd.h>

using namespace boost::numeric::ublas;

Mostro::Mostro(std::string xsection_id, DataSource data_source=DataSource::FLEXI) {
	id = xsection_id;
    source = data_source;
    bool isInit = false;

    if(source == FLEXI)
        cPath = MOSTRO_HOME + "/" + id + "/flexi/" + id + ".json";
    else if(source == ARS)
        cPath = MOSTRO_HOME + "/" + id + "/ars/" + id + ".json";

    if(access(cPath.c_str(), F_OK) != -1)
        isInit = parseConfig(cPath);
    else
        std::cout  << "[infMOSTRO." + id + "] no existe un archivo de configuración."  << std::endl;
}

bool Mostro::parseConfig(std::string conf_path){
    std::ifstream jsonf(conf_path, std::ifstream::binary);
    Json::Value json_value;
    jsonf >> json_value;

    nSpace = json_value["singular_vectors"]["sv0"].size();
    nDrains = json_value["drains"].size();
    nEntries = json_value["entries"].size();
    nEdges = nEntries - nDrains;

    pDrains.resize(nDrains, nEdges);
    for (int s = 0; s < nDrains; s++) {
        for (int e = 0; e < 3; e++)
            pDrains(s,e) = json_value["drains"][s][e].asDouble(); //revisar
    }

    edges.resize(nEdges);
    for (int e = 0; e < nEdges; e++) {
        edges(e) = json_value["entries"][e].asString; //revisar
    }

    clusters.resize(8,3);
    for (int c = 0; c < 8; c++) {
        for (int d = 0; d < 3; d++)
            clusters(c,d) = json_value["clusters"][c]["centroid"][d].asDouble();
    }

    vh.resize(3, nSpace);
    for (int d = 0; d < 3; d++) {
        for (int c = 0; c < nSpace; c++) {
            std::string sv = "sv" + std::to_string(d);
            vh(d,c) = json_value["singular_vectors"][sv][c].asDouble();
        }
    }

    if (source == FLEXI) {
        input_vector.resize(nEdges * 3 + pDrains.size1);
    }
    else if (source == ARS) {
        input_vector.resize(nEdges * 3 + pDrains.size1);
    }
    return true;
}
 
std::string Mostro::suggestedPlan(FlexiData flexi_data[]){
    if (isInit)
        goto suggest;
    else
        return "error: not initialized";

suggest:
    if ((sizeof(flexi_data) / sizeof(*flexi_data)) != nEdges) {
        std::cout << "[infMOSTRO." + id + "] datos provenientes de " + std::to_string(source) + " están incompletos.";
        return "error: " + std::to_string(source) + "_data";
    }
    else {
        for (int e = 0; e < nEdges; e++) {
            for (int d = 0; d < nEdges; d++) {
                if (flexi_data[d].edge == edges[e]) {
                    input_vector(0 * nEdges + e) = flexi_data[d].flow;
                    input_vector(1 * nEdges + nDrains + e) = flexi_data[d].speed;
                    input_vector(2 * nEdges + nDrains + e) = flexi_data[d].queue;
                    break;
                }
            }
        }
        for (int s = 0; s < nDrains; s++) {
            double s_val = 0.0;
            for (int e = 0; e < 3; e++) {
                s_val += pDrains(s,e) * input_vector(e); //revisar
            }
            input_vector(nEdges + s) = s_val;
        }
        vector<double> aPoint = prod(vh, input_vector);
        int plan = -1;
        double cdist = 1E10; //parametro de distancia minima admisible
        for (int c = 0; c < 8; c++) {
            vector<double> diff(3);
            for (int d = 0; d < 3; d++) {
                diff(d) = clusters(c, d) - aPoint(d);
            }
            double cnorm2 = norm_2(diff);
            if (cnorm2 < cdist) {
                cdist = cnorm2;
                plan = c;
            }   
        }
        if (plan >= 0)
            return id + ".plan" + std::to_string(plan);
        else
            return "error: el estado medido de la intersección se aleja demasiado de los estados conocidos.";
    }
}

std::string Mostro::suggestedPlan(ArsData ars_data[]) {

}
