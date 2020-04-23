/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include "../include/mostro.h"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <fstream>
#include <iostream>
#include <unistd.h>

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

    nSpace = json_value["singular_vectors"][0]["sv1"].size();
    nSinks = json_value["sinks"].size();
    nEntries = json_value["edges"].size();
    nEdges = nEntries - nSinks;

    max_euclid = json_value["max_euclidian"].asDouble();

    pSinks.resize(nSinks, nEdges);
    for (uint s = 0; s < nSinks; s++) {
        for (uint e = 0; e < 3; e++)
            pSinks(s,e) = json_value["sinks"][s][e].asDouble(); //revisar
    }

    edges.resize(nEdges);
    for (uint e = 0; e < nEdges; e++) {
        edges(e) = json_value["edges"][e].asString(); //revisar
    }

    clusters.resize(8,3);
    for (uint c = 0; c < 8; c++) {
        for (uint d = 0; d < 3; d++)
            clusters(c,d) = json_value["clusters"][c]["centroid"][d].asDouble();
    }

    vh.resize(3, nSpace);
    for (uint d = 0; d < 3; d++) {
        for (uint c = 0; c < nSpace; c++) {
            std::string sv = "sv" + std::to_string(d+1);
            vh(d,c) = json_value["singular_vectors"][0][sv][c].asDouble();
        }
    }

    if (source == FLEXI) {
        input_vector.resize(nEdges * 3 + pSinks.size1(), 1);
    }
    else if (source == ARS) {
        input_vector.resize(nEdges * 3 + pSinks.size1(), 1);
    }
    return true;
}
 
std::string Mostro::suggestedPlan(FlexiData flexi_data[], uint dsize){
    if (isInit)
        goto suggest;
    else
        return "[infMOSTRO." + id + "]error: not initialized";

suggest:
    if (dsize != nEdges) {
        std::cout << "[infMOSTRO." + id + "] datos provenientes de " + std::to_string(source) + " están incompletos.";
        return "error: " + std::to_string(source) + "_data";
    }
    else {
        for (uint e = 0; e < nEdges; e++) {
            for (uint d = 0; d < nEdges; d++) {
                if (flexi_data[d].edge == edges[e]) {
                    input_vector(0 * nEdges + e, 0) = flexi_data[d].flow;
                    input_vector(1 * nEdges + nSinks + e, 0) = flexi_data[d].speed;
                    input_vector(2 * nEdges + nSinks + e, 0) = flexi_data[d].queue;
                    break;
                }
            }
        }
        for (uint s = 0; s < nSinks; s++) {
            double s_val = 0.0;
            for (uint e = 0; e < 3; e++) {
                s_val += pSinks(s,e) * input_vector(e, 0); //revisar
            }
            input_vector(nEdges + s, 0) = s_val;
        }
        matrix<double> aPoint = prod(vh, input_vector);
        int plan = -1;
        double cdist = 1E10; //parametro de distancia minima admisible
        for (uint c = 0; c < 8; c++) {
            vector<double> diff(3);
            for (int d = 0; d < 3; d++) {
                diff(d) = clusters(c, d) - aPoint(d, 0);
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
            return "[infMOSTRO." + id + "] error: el estado medido de la intersección se aleja demasiado de los estados conocidos.";
    }
}

//std::string Mostro::suggestedPlan(ArsData ars_data[]) {
//    return "";
//}
