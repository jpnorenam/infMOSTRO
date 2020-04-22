#include <fstream>
#include <cmath>
#include <limits>
#include <chrono>
#include <ctime>
#include <string>
#include <iostream> 

#include "mostro.h"

using namespace std; 
  
Mostro::Mostro(string intersection_file_path) { 
    std::ifstream json_file(intersection_file_path, std::ifstream::binary);
    if (json_file.good()) {
        json_file >> xsection_json;
        xsection_id = xsection_json["id"].asString();
        flexi_len = xsection_json["flexi_entries"].size();
        processJson();
    }
    else
        cout << "MOSTRO: error. " + intersection_file_path + " no existe, por favor revise el directorio." << endl;
} 

string Mostro::suggestPlan(vector<double> flexi_data) {
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
    if (flexi_data.size() == flexi_len) {
        double dist = numeric_limits<double>::max();
        double accum = 0.0;
        double r = 0.0;

        string suggested;

        for (int k = 0; k < clusters_len; k++) {
            accum = 0.0;
            for (int i = 0; i < flexi_len; i++) {
                r = centroids[k][i] - flexi_data[i];
                accum += r * r;
            }
            if (sqrt(accum) <= dist) {
                dist = sqrt(accum);
                suggested = plans[k];
            }
        }
        cout << "MOSTRO: plan sugerido para " + xsection_id +  " >> " + suggested + " | " + ctime(&timenow) << endl;
        return suggested;
    }
    else {
        cout << "MOSTRO: error en " + xsection_id + ". Entrada de flexi." << endl;
        return "error entrada de flexi";
    }
}

int Mostro::processJson() {
    Json::Value arrClusters_json = xsection_json["clusters"];
    clusters_len = arrClusters_json.size();
    Json::Value cluster_json;
    Json::Value centroid_json;
    plans = new string[clusters_len];
    centroids = new vector<double>[clusters_len];

    for (int k = 0; k < clusters_len; k++) {
        cluster_json = arrClusters_json[k]; 
        plans[k] = cluster_json["plan"].asString();
        centroid_json = cluster_json["centroid"];

        if (centroid_json.size() == flexi_len){
            vector<double> cntrd = {};
            for (int i = 0; i < flexi_len; i++)
                cntrd.push_back(centroid_json[i].asDouble());
            centroids[k] = cntrd;
        }
        else {
            cout << "MOSTRO: error en " + xsection_id + ". El centroide no coincide con el numero de entradas de flexi." << endl;
            return -1;
        }
    }
    return 0;
}