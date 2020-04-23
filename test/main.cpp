#include "../include/mostro.h"

int main()
{
    FlexiData flexi_data1 = { .edge = "Carrera 65 >(N)", .flow = 65.0, .speed = 8.5, .queue = 90 };
    FlexiData flexi_data2 = { .edge = "Av. San Juan >Oriente", .flow = 70.0, .speed = 16.0, .queue = 110 };
    FlexiData flexi_data3 = { .edge = "Av. San Juan >Occidente", .flow = 100.0, .speed = 20.8, .queue = 360 };
    FlexiData flexi_list[3] = { flexi_data1,  flexi_data2,  flexi_data3 };

    Mostro cll44_cr65 = Mostro("cll44_cr65", DataSource::FLEXI);
    std::string Plan_flexi = cll44_cr65.suggestedPlan(flexi_list, 3);
    std::cout << Plan_flexi << std::endl;
    return 0;
}