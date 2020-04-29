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

Mostro::Mostro(char* xsection_id, DataSource data_source=DataSource::FLEXI) {
	id = std::string(xsection_id);
    source = data_source;
    isInit = false;
    xsection_console = spdlog::stdout_color_mt(id+".console");
    spdlog::set_pattern("*** [%H:%M:%S] [infMOSTRO." + id + "] %v ***");
    xsection_console->info("initializing infMOSTRO.");

    if (source == FLEXI) {
        cPath = INFMOSTRO_HOME + "/" + id + "/flexi/";
        cFile = cPath + id + ".json";
    }
    else if (source == ARS) {
        cPath = INFMOSTRO_HOME + "/" + id + "/ars/";
        cFile = cPath + id + ".json";
    }

    if (access(cFile.c_str(), F_OK) != -1) {
        xsection_logger = spdlog::basic_logger_mt(id + ".logger", cPath + id + ".log.txt");
        isInit = parseConfig(cFile);
    }
    else
        xsection_console->error("{0} doesn't exist.", cFile);
}

bool Mostro::parseConfig(std::string json_path){
    std::ifstream jsonf(json_path, std::ifstream::binary);
    Json::Value json_value;

    try {
        jsonf >> json_value;
        nSpace = json_value["singular_vectors"][0].size();
        nSinks = json_value["sinks"].size();
        nEdges = json_value["edges"].size();
        nClusters = json_value["clusters"].size();
        nIgnoreF = json_value["ignore"]["flow"].size();
        nIgnoreS = json_value["ignore"]["speed"].size();
        nIgnoreQ = json_value["ignore"]["queue"].size();
        max_euclid = json_value["avg_distance"].asDouble();
    }
    catch (int e) {
        xsection_console->error(cFile + " couldn't be correctly initialized.");
        xsection_logger->error(cFile + " couldn't be correctly initialized.");
        return false;
    }

    pSinks.resize(nSinks, nEdges);
    for (uint s = 0; s < nSinks; s++) {
        for (uint e = 0; e < nEdges; e++)
            pSinks(s,e) = json_value["sinks"][s][e].asDouble();
    }

    edges.resize(nEdges);
    for (uint e = 0; e < nEdges; e++)
        edges(e) = json_value["edges"][e].asString();

    clusters.resize(nClusters,3);
    for (uint c = 0; c < nClusters; c++) {
        for (uint d = 0; d < 3; d++)
            clusters(c,d) = json_value["clusters"][c]["centroid"][d].asDouble();
    }

    vh.resize(nSpace, 3);
    for (uint d = 0; d < 3; d++) {
        for (uint c = 0; c < nSpace; c++)
            vh(c,d) = json_value["singular_vectors"][d][c].asDouble();
    }

    for (uint i = 0; i < nIgnoreF; i++)
        sIgnoreF.push_back(json_value["ignore"]["flow"][i].asString());
    for (uint i = 0; i < nIgnoreS; i++)
        sIgnoreS.push_back(json_value["ignore"]["speed"][i].asString());
    for (uint i = 0; i < nIgnoreQ; i++)
        sIgnoreQ.push_back(json_value["ignore"]["queue"][i].asString());

    int nIgnore = nIgnoreF + nIgnoreS + nIgnoreQ;
    if (source == FLEXI)
        input_vector.resize(1, nEdges * 3 + pSinks.size1() - nIgnore);
    else if (source == ARS)
        input_vector.resize(1, nEdges * 2 + pSinks.size1() - nIgnore);

    xsection_console->info(cFile + " was correctly initialized.");
    xsection_logger->info(cFile + " was correctly initialized.");
    return true;
}
 
const char* Mostro::suggestedPlan(FlexiData flexi_data[], uint dsize){
    spdlog::set_pattern("*** [%H:%M:%S] [infMOSTRO." + id + ".flexi] %v ***");
    if (isInit)
        goto suggest;
    else {
        xsection_console->error("can't suggest a plan without a correctly initialization.");
        xsection_logger->error("can't suggest a plan without a correctly initialization.");
        return (id + "/flexi/error-1").c_str();
    }

suggest:
    if (dsize != nEdges) {
        xsection_console->error("the number of expected inputs ({0:d}) from flexi is incomplete.", nEdges);
        xsection_logger->error("the number of expected inputs ({0:d}) from flexi is incomplete.", nEdges);
        return (id + "/flexi/error-2").c_str();
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
                s_val += pSinks(s,e) * input_vector(0, e);
            input_vector(0, nEdges + s) = s_val;
        }
        matrix<double> aPoint = prod(input_vector, vh); //vh is unitary vh.T = vh^-1
        int plan = -1;
        double cdist = 10E10;
        for (uint c = 0; c < nClusters; c++) {
            vector<double> diff(3);
            for (int d = 0; d < 3; d++)
                diff(d) = clusters(c, d) - aPoint(0, d);
            double cnorm2 = norm_2(diff);
            if (cnorm2 < cdist) {
                cdist = cnorm2;
                plan = c;
            }   
        }
        if (cdist < max_euclid) {
            xsection_console->info("recommends planC{0:d}", plan);
            xsection_logger->info("recommends planC{0:d}", plan);
            return (id + "/flexi/planC" + std::to_string(plan)).c_str();
        }
        else if(cdist > max_euclid && cdist < max_euclid*3) {
            xsection_console->warn("uncertain recommendation of plan{0:d}", plan);
            xsection_logger->warn("uncertain recommendation of plan{0:d}", plan);
            return (id + "/flexi/planC" + std::to_string(plan)).c_str();
        }
        else {
            xsection_console->error("the estimated fluxes by flexi are too far away from the known records");
            xsection_logger->error("the estimated fluxes by flexi are too far away from the known records");
            return (id + "/flexi/error-3").c_str();
        }
    }
}

const char* Mostro::suggestedPlan(ArsData ars_data[], uint dsize) {
    spdlog::set_pattern("*** [%H:%M:%S] [infMOSTRO." + id + ".ars] %v ***");
    if (isInit)
        goto suggest;
    else {
        xsection_console->error("can't suggest a plan without a correctly initialization.");
        xsection_logger->error("can't suggest a plan without a correctly initialization.");
        return (id + "/ars/error-1").c_str();
    }

suggest:
    if (dsize != nEdges) {
        xsection_console->error("the number of expected inputs ({0:d}) from ars is incomplete.", nEdges);
        xsection_logger->error("the number of expected inputs ({0:d}) from ars is incomplete.", nEdges);
        return (id + "/ars/error-2").c_str();
    }
    else {
        for (uint e = 0; e < nEdges; e++) {
            for (uint d = 0; d < nEdges; d++) {
                if (ars_data[d].id == edges[e]) {
                    if (std::find(sIgnoreF.begin(), sIgnoreF.end(), edges[e]) == sIgnoreF.end())
                        input_vector(0, e) = ars_data[d].flow1 + 2.0 * ars_data[d].flow2 + 2.5 * ars_data[d].flow3;
                    if (std::find(sIgnoreS.begin(), sIgnoreS.end(), edges[e]) == sIgnoreS.end())
                        input_vector(0, nEdges - nIgnoreF + e) = ars_data[d].speed;
                    break;
                }
            }
        }
        for (uint s = 0; s < nSinks; s++) {
            double s_val = 0.0;
            for (uint e = 0; e < nEdges; e++)
                s_val += pSinks(s, e) * input_vector(0, e);
            input_vector(0, nEdges + s) = s_val;
        }
        matrix<double> aPoint = prod(input_vector, vh); //vh is unitary vh.T = vh^-1
        int plan = -1;
        double cdist = 10E10;
        for (uint c = 0; c < nClusters; c++) {
            vector<double> diff(3);
            for (int d = 0; d < 3; d++)
                diff(d) = clusters(c, d) - aPoint(0, d);
            double cnorm2 = norm_2(diff);
            if (cnorm2 < cdist) {
                cdist = cnorm2;
                plan = c;
            }
        }
        if (cdist < max_euclid) {
            xsection_console->info("recommends planC{0:d}", plan);
            xsection_logger->info("recommends planC{0:d}", plan);
            return (id + "/ars/planC" + std::to_string(plan)).c_str();
        }
        else if (cdist > max_euclid && cdist < max_euclid * 3) {
            xsection_console->warn("uncertain recommendation of planC{0:d}", plan);
            xsection_logger->warn("uncertain recommendation of planC{0:d}", plan);
            return (id + "/ars/planC" + std::to_string(plan)).c_str();
        }
        else {
            xsection_console->error("the measured fluxes by ars are too far away from the known records");
            xsection_logger->error("the measured fluxes by ars are too far away from the known records");
            return (id + "/ars/error-3").c_str();
        }
    }
}
