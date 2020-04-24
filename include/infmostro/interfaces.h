/* copyright 2020 Universidad Nacional de Colombia,
 * Laboratorio de Gesti�n de Sistemas en Tiempo Real
 * 
 * Licencia, 
 * otorgada a Secretar�a de Movilidad de Medell�n
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

