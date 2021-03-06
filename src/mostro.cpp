/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gesti�n de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretar�a de Movilidad de Medell�n
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
    prev_plan = "";
    prev_dsize = 0;
    xsection_console = spdlog::stdout_color_mt(id + std::to_string(source) +"-console");
    spdlog::set_pattern("*** [%Y-%m-%d %H:%M:%S] [infMOSTRO." + id + "] %v ***");
    xsection_console->info("initializing infMOSTRO.");

    if (source == FLEXI) {
        cPath = INFMOSTRO_HOME + "confs/" + id + "/flexi/";
        cFile = cPath + id + ".json";
    }
    else if (source == ARS) {
        cPath = INFMOSTRO_HOME + "confs/" + id + "/ars/";
        cFile = cPath + id + ".json";
    }

    xsection_logger = spdlog::basic_logger_mt(id + std::to_string(source) + "-logger",
        INFMOSTRO_HOME + "logs/" + id + "-" + std::to_string(source) + "-log.txt");

    if (access(cFile.c_str(), F_OK) != -1)
        isInit = parseConfig(cFile);
    else {
        xsection_console->error("{0} doesn't exist [ERROR-1].", cFile);
        xsection_logger->error("{0} doesn't exist [ERROR-1].", cFile);
        xsection_logger->flush();
    }
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
        xsection_console->error(cFile + " couldn't be correctly initialized, problem with file keys.");
        xsection_logger->error(cFile + " couldn't be correctly initialized, problem with file keys.");
        xsection_logger->flush();
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
    if (source == FLEXI) {
        input_vector.resize(1, nEdges * 3 + pSinks.size1() - nIgnore);
        if (input_vector.size2() != nSpace) {
            xsection_console->error(cFile + " couldn't be correctly initialized, dimensionality error.");
            xsection_logger->error(cFile + " couldn't be correctly initialized, dimensionality error.");
            xsection_logger->flush();
            return false;
        }
    }

    else if (source == ARS) {
        input_vector.resize(1, nEdges * 2 + pSinks.size1() - nIgnore);
        if (input_vector.size2() != nSpace) {
            xsection_console->error(cFile + " couldn't be correctly initialized, dimensionality error.");
            xsection_logger->error(cFile + " couldn't be correctly initialized, dimensionality error.");
            xsection_logger->flush();
            return false;
        }
    }

    xsection_console->info(cFile + " was correctly initialized.");
    xsection_logger->info(cFile + " was correctly initialized.");
    return true;
}
 
std::string Mostro::suggestedPlan(FlexiData flexi_data[], uint dsize){
    std::string rec_plan = "";
    spdlog::set_pattern("*** [%Y-%m-%d %H:%M:%S] [infMOSTRO." + id + ".flexi] %v ***");
    if (isInit)
        goto suggest;
    else {
        xsection_console->error("can't suggest a plan without a correctly initialization [ERROR-2].");
        rec_plan = id + "/flexi/error-2";
        goto out;
    }

suggest:
    if (dsize != nEdges) {
        prev_dsize = dsize;
        xsection_console->error("the number of recived inputs ({0:d}) from flexi differs from the expected ({1:d}) [ERROR-3].", dsize, nEdges);
        if (dsize != prev_dsize){
            xsection_logger->error("the number of recived inputs ({0:d}) from flexi differs from the expected ({1:d}) [ERROR-3].", dsize, nEdges);
            xsection_logger->flush();
        }
        rec_plan = id + "/flexi/error-3";
        goto out;
    }
    else {
        uint s = 0; uint q = 0;
        for (uint e = 0; e < nEdges; e++) {
            for (uint d = 0; d < nEdges; d++) {
                if (flexi_data[d].edge == edges[e]) {
                    xsection_console->info("{0}.flow->{1:f} (v/h)", edges[e], flexi_data[d].flow);
                    input_vector(0, e) = flexi_data[d].flow;
                    if (std::find(sIgnoreS.begin(), sIgnoreS.end(), edges[e]) == sIgnoreS.end()) {
                        double fspeed  = flexi_data[d].speed == -1 ? 35.0 : flexi_data[d].speed;
                        xsection_console->info("{0}.speed->{1:f} (km/h)", edges[e], fspeed);
                        input_vector(0, 1 * nEdges + nSinks + s) = fspeed;
                        s++;
                    }
                    if (std::find(sIgnoreQ.begin(), sIgnoreQ.end(), edges[e]) == sIgnoreQ.end()) {
                        double fqueue = flexi_data[d].queue == -1 ? 15.0 : flexi_data[d].queue;
                        xsection_console->info("{0}.queue->{1:f} (m)", edges[e], fqueue);
                        input_vector(0, 2 * nEdges + nSinks - nIgnoreS + q) = fqueue;
                        q++;
                    }
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
            rec_plan = id + "/flexi/planC" + std::to_string(plan);
            xsection_console->info("recommends planC{0:d}", plan);
            goto out;

        }
        else if(cdist > max_euclid && cdist < max_euclid*3) {
            rec_plan = id + "/flexi/planC" + std::to_string(plan);
            xsection_console->warn("uncertainly recommends planC{0:d}", plan);
            goto out;
        }
        else {
            rec_plan = id + "/flexi/error-4";
            xsection_console->error("the estimated fluxes by flexi are too far away from the known records [ERROR-4].");
            goto out;
        }
    }
out:
    if (prev_plan != rec_plan) {
        prev_plan = rec_plan;
        if (rec_plan == id + "/flexi/error-4")
            xsection_logger->error("the estimated fluxes by flexi are too far away from the known records [ERROR-4].");
        xsection_logger->info("recommends {}", rec_plan);
        xsection_logger->flush();
    }
    return rec_plan;
}

std::string Mostro::suggestedPlan(ArsData ars_data[], uint dsize) {
    std::string rec_plan = "";
    spdlog::set_pattern("*** [%Y-%m-%d %H:%M:%S] [infMOSTRO." + id + ".ars] %v ***");
    if (isInit)
        goto suggest;

    else {
        xsection_console->error("can't suggest a plan without a correctly initialization [ERROR-2].");
        rec_plan = id + "/ars/error-2";
        goto out;
    }

suggest:
    if (dsize != nEdges) {
        xsection_console->error("the number of recived inputs ({0:d}) from ars differs from the expected ({1:d}) [ERROR-3].", dsize, nEdges);
        if (dsize != prev_dsize) {
            prev_dsize = dsize;
            xsection_logger->error("the number of recived inputs ({0:d}) from ars differs from the expected ({1:d}) [ERROR-3].", dsize, nEdges);
            xsection_logger->flush();
        }
        rec_plan = id + "/flexi/error-3";
        goto out;
    }
    else {
        for (uint e = 0; e < nEdges; e++) {
            for (uint d = 0; d < nEdges; d++) {
                if (ars_data[d].id == edges[e]) {
                    if (std::find(sIgnoreF.begin(), sIgnoreF.end(), edges[e]) == sIgnoreF.end()) {
                        xsection_console->info("{0}.flow1->{1:f} (v1/5min)", edges[e], ars_data[d].flow1);
                        xsection_console->info("{0}.flow2->{1:f} (v2/5min)", edges[e], ars_data[d].flow2);
                        xsection_console->info("{0}.flow3->{1:f} (v3/5min)", edges[e], ars_data[d].flow2);
                        input_vector(0, e) = ars_data[d].flow1 + 2.0 * ars_data[d].flow2 + 2.5 * ars_data[d].flow3;
                        
                    }                       
                    if (std::find(sIgnoreS.begin(), sIgnoreS.end(), edges[e]) == sIgnoreS.end()) {
                        xsection_console->info("{0}.speed->{1:f} (km/h)", edges[e], ars_data[d].speed);
                        input_vector(0, nEdges - nIgnoreF + e) = ars_data[d].speed;
                    }
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
            rec_plan = id + "/ars/planC" + std::to_string(plan);
            xsection_console->info("recommends planC{0:d}", plan);
            goto out;

        }
        else if (cdist > max_euclid && cdist < max_euclid * 3) {
            rec_plan = id + "/ars/planC" + std::to_string(plan);
            xsection_console->warn("uncertainly recommends planC{0:d}", plan);
            goto out;
        }
        else {
            rec_plan = id + "/ars/error-4";
            xsection_console->error("the estimated fluxes by flexi are too far away from the known records [ERROR-4].");
            goto out;
        }
    }
out:
    if (prev_plan != rec_plan) {
        prev_plan = rec_plan;
        if (rec_plan == id + "/ars/error-4")
            xsection_logger->error("the measured fluxes by ars are too far away from the known records [ERROR-4].");
        xsection_logger->info("recommends {}", rec_plan);
        xsection_logger->flush();
    }
    return rec_plan;
}

extern "C" {

    Mostro* createInstance(const char* xsection_id, DataSource data_source) {
        return new Mostro(std::string(xsection_id), data_source);
    }
    char* suggestPlanFlexi(Mostro* mostro, FlexiData flexi_data[], int size) {
        char* res;
        res = (char*)malloc(sizeof(char) * 40);
        strcpy(res, (mostro->suggestedPlan(flexi_data, (uint)size)).c_str());
        return res;
    }
    char* suggestPlanArs(Mostro* mostro, ArsData ars_data[], int size) {
        char* res;
        res = (char*)malloc(sizeof(char) * 40);
        strcpy(res, (mostro->suggestedPlan(ars_data, (uint)size)).c_str());
        return res;
    }
}