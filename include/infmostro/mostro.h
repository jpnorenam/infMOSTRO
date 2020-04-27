/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include <infmostro/interfaces.h>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

using namespace boost::numeric::ublas;

//const std::string INFMOSTRO_HOME = "/vagrant/infMOSTRO/data/infmostro"; //for degugging
const std::string INFMOSTRO_HOME = std::getenv("INFMOSTRO_HOME");

class Mostro { 
private:
    std::string id;
    DataSource source;
    std::string cPath;
    bool isInit;

    double max_euclid;
    uint nClusters;
    uint nEdges, nArs, nSinks, nSpace;
    uint nIgnoreF, nIgnoreS, nIgnoreQ;
    vector<std::string> edges;
    std::vector<std::string> sIgnoreF, sIgnoreS, sIgnoreQ;
    matrix<double> pSinks, clusters;
    matrix<double> vh, input_vector;
    bool parseConfig(std::string conf_path);

public: 
    Mostro(std::string xsection_id, DataSource data_source);
    std::string suggestedPlan(FlexiData flexi_data[], uint dsize);
    std::string suggestedPlan(ArsData ars_data[], uint dsize);
};


