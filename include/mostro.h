/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include "interfaces.h"
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

using namespace boost::numeric::ublas;

const std::string MOSTRO_HOME = "/vagrant/infMOSTRO/data";
//const std::string MOSTRO_HOME = std::getenv("MOSTRO_HOME");

class Mostro { 
private:
    std::string id;
    DataSource source;
    std::string cPath;
    bool isInit;

    double max_euclid;
    uint nEdges, nArs, nEntries, nSinks, nSpace;
    vector<std::string> edges;
    matrix<double> pSinks, clusters;
    matrix<double> vh, input_vector;

public: 
    Mostro(std::string xsection_id, DataSource data_source);
    bool parseConfig(std::string conf_path);
    std::string suggestedPlan(FlexiData flexi_data[], uint dsize);
    //std::string suggestedPlan(ArsData flexi_data[], uint dsize);
};


