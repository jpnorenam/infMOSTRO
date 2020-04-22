/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gestión de Sistemas en Tiempo Real
 * 
 * Licencia, 
 * otorgada a Secretaría de Movilidad de Medellín
 ****************************************************/

enum DataSource { FLEXI, ARS };

struct FlexiData {
    char* edge;
    double flow;
    double speed;
    double queue;
};

struct ArsData {
    char* ars_id;
    double flow1, flow2, flow3;
    double speed;
};

