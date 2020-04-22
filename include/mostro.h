#include "interfaces.h"
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

const std::string MOSTRO_HOME = std::getenv("MOSTRO_HOME");

class Mostro { 
private:
    std::string id;
    DataSource source;
    std::string cPath;
    bool isInit;

    uint nEdges, nArs, nEntries, nDrains, nSpace;
    vector<std::string> edges;
    matrix<double> pDrains, clusters, vh;
    vector<double> input_vector;

public: 
    Mostro(std::string xsection_id, DataSource data_source);
    bool parseConfig(std::string conf_path);
    std::string suggestedPlan(FlexiData flexi_data[]);
    std::string suggestedPlan(ArsData flexi_data[]);
};


