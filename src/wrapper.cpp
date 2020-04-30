/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 *
 * Licencia,
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

#include <infmostro/wrapper.h>

extern "C" {

    Mostro* createInstance(const char* xsection_id, DataSource data_source) {
        return new Mostro(std::string(xsection_id), data_source);
    }
    const char* suggestPlanFlexi(Mostro* mostro, FlexiData flexi_data[], uint size) {
        return (mostro->suggestedPlan(flexi_data, size)).c_str();
    }

    const char* suggestPlanArs(Mostro* mostro, ArsData ars_data[], uint size) {
        return (mostro->suggestedPlan(ars_data, size)).c_str();
    }
}