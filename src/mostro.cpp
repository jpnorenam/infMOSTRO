/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include <infmostro/mostro.h>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <unistd.h>

Mostro::Mostro(std::string xsection_id, DataSource data_source=DataSource::FLEXI) {
	id = xsection_id;
    source = data_source;
    isInit = false;

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

    nSpace = json_value["singular_vectors"][0].size();
    nSinks = json_value["sinks"].size();
    nEdges = json_value["edges"].size();

    nIgnoreS = json_value["ignore"]["speed"].size();
    nIgnoreQ = json_value["ignore"]["queue"].size();

    max_euclid = json_value["avg_distance"].asDouble();

    pSinks.resize(nSinks, nEdges);
    for (uint s = 0; s < nSinks; s++) {
        for (uint e = 0; e < nEdges; e++)
            pSinks(s,e) = json_value["sinks"][s][e].asDouble(); //revisar
    }

    edges.resize(nEdges);
    for (uint e = 0; e < nEdges; e++)
        edges(e) = json_value["edges"][e].asString(); //revisar

    clusters.resize(8,3);
    for (uint c = 0; c < 8; c++) {
        for (uint d = 0; d < 3; d++)
            clusters(c,d) = json_value["clusters"][c]["centroid"][d].asDouble();
    }

    vh.resize(nSpace, 3);
    for (uint d = 0; d < 3; d++) {
        for (uint c = 0; c < nSpace; c++)
            vh(c,d) = json_value["singular_vectors"][d][c].asDouble();
    }

    for (uint i = 0; i < nIgnoreS; i++)
        sIgnoreS.push_back(json_value["ignore"]["speed"][i].asString());
    for (uint i = 0; i < nIgnoreQ; i++)
        sIgnoreQ.push_back(json_value["ignore"]["queue"][i].asString());

    int nIgnore = nIgnoreS + nIgnoreQ;
    if (source == FLEXI)
        input_vector.resize(1, nEdges * 3 + pSinks.size1() - nIgnore);
    else if (source == ARS)
        input_vector.resize(1, nEdges * 3 + pSinks.size1() - nIgnore);

    return true;
}
 
std::string Mostro::suggestedPlan(FlexiData flexi_data[], uint dsize){
    if (isInit)
        goto suggest;
    else
        return "[infMOSTRO." + id + "]error: no fue inicializado correctamente.";

suggest:
    if (dsize != nEdges) {
        std::cout << "[infMOSTRO." + id + "] datos provenientes de " + std::to_string(source) + " están incompletos.";
        return "error: " + std::to_string(source) + "_data";
    }
    else {
        for (uint e = 0; e < nEdges; e++) {
            for (uint d = 0; d < nEdges; d++) {
                if (flexi_data[d].edge == edges[e]) {
                    input_vector(0, e) = flexi_data[d].flow;
                    if (std::find(sIgnoreS.begin(), sIgnoreS.end(), edges[e]) == sIgnoreS.end()) 
                        input_vector(0, 1 * nEdges + nSinks + e) = flexi_data[d].speed;
                    if (std::find(sIgnoreQ.begin(), sIgnoreQ.end(), edges[e]) == sIgnoreQ.end())
                        input_vector(0, 2 * nEdges + nSinks - nIgnoreS + e) = flexi_data[d].queue;
                    break;
                }
            }
        }
        for (uint s = 0; s < nSinks; s++) {
            double s_val = 0.0;
            for (uint e = 0; e < nEdges; e++)
                s_val += pSinks(s,e) * input_vector(0, e); //revisar
            input_vector(0, nEdges + s) = s_val;
        }
        matrix<double> aPoint = prod(input_vector, vh); //vh is unitary vh.T = vh^-1
        int plan = -1;
        double cdist = max_euclid;
        for (uint c = 0; c < 8; c++) {
            vector<double> diff(3);
            for (int d = 0; d < 3; d++)
                diff(d) = clusters(c, d) - aPoint(0, d);
            double cnorm2 = norm_2(diff);
            if (cnorm2 < cdist) {
                cdist = cnorm2;
                plan = c;
            }   
        }
        if (plan >= 0)
            return id + ".plan" + std::to_string(plan);
        else
            return "[infMOSTRO." + id + "] error: el estado estimado de la intersección se aleja demasiado de los estados conocidos.";
    }
}

// std::string Mostro::suggestedPlan(ArsData ars_data[]) {
//     return "";
// }
