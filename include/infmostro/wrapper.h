/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include <infmostro/interfaces.h>
#include <infmostro/mostro.h>

extern "C" Mostro* createInstance(char* xsection_id, DataSource data_source) {
    return new Mostro(std::string(xsection_id), data_source);
}

extern "C" const char* suggestPlanFlexi(Mostro* mostro, FlexiData flexi_data[], uint size) {
    return (mostro->suggestedPlan(flexi_data, size)).c_str();
}

extern "C" const char* suggestPlanArs(Mostro* mostro, ArsData ars_data[], uint size) {
    return (mostro->suggestedPlan(ars_data, size)).c_str();
}