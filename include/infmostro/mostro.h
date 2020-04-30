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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace boost::numeric::ublas;

#ifdef DEBUG
const std::string INFMOSTRO_HOME = "/vagrant/infMOSTRO/data/infmostro";
#else
const std::string INFMOSTRO_HOME = std::getenv("INFMOSTRO_HOME");
#endif

class Mostro { 
private:
    bool isInit;
    std::string id, cPath, cFile;
    DataSource source;
    double max_euclid;
    uint nClusters, nEdges, nArs, nSinks, nSpace, nIgnoreF, nIgnoreS, nIgnoreQ;
    vector<std::string> edges;
    std::vector<std::string> sIgnoreF, sIgnoreS, sIgnoreQ;
    matrix<double> pSinks, clusters, vh, input_vector;
    std::shared_ptr<spdlog::logger> xsection_logger, xsection_console;

    bool parseConfig(std::string json_path);
public: 
    Mostro(std::string xsection_id, DataSource data_source);
    std::string suggestedPlan(FlexiData flexi_data[], uint dsize);
    std::string suggestedPlan(ArsData ars_data[], uint dsize);
};

extern "C" Mostro * createInstance(const char* xsection_id, DataSource data_source);
extern "C" char* suggestPlanFlexi(Mostro * mostro, FlexiData flexi_data[], int size);
extern "C" char* suggestPlanArs(Mostro * mostro, ArsData ars_data[], int size);


