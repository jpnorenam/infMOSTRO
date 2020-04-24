#include <infmostro/mostro.h>

int main()
{
	FlexiData flexi_cll44_cr65_1 = { .edge = "Av. San Juan >Oriente", .flow = 80.2 * 12, .speed = 8.9, .queue = 140.8 };
	FlexiData flexi_cll44_cr65_2 = { .edge = "Av. San Juan >Occidente", .flow = 48.7 * 12, .speed = 11.0, .queue = 90.5 };
	FlexiData flexi_cll44_cr65_3 = { .edge = "Carrera 84 >(N)", .flow = 100.1 * 12, .speed = 7.5, .queue = 96.3 };
	FlexiData flexi_cll44_cr65_4 = { .edge = "Carrera 84 >(S)", .flow = 104.9 * 12, .speed = 6.7, .queue = 102.4 };
	FlexiData flexi_list_cll44_cr65[4] = { flexi_cll44_cr65_3,  flexi_cll44_cr65_2,  flexi_cll44_cr65_1, flexi_cll44_cr65_4 };

	Mostro cll44_cr65 = Mostro("cll44_cr84", DataSource::FLEXI);
	std::string Plan_flexi_cll44_cr65 = cll44_cr65.suggestedPlan(flexi_list_cll44_cr65, 4);
	std::cout << Plan_flexi_cll44_cr65 << std::endl;


	FlexiData flexi_av33_cr66a_1 = { .edge = "Av. 33 >Occidente", .flow = 40.8 * 12, .speed = 11.0, .queue = 352.6 };
	FlexiData flexi_av33_cr66a_2 = { .edge = "Carrera 65D >SurOccidente", .flow = 93.8 * 12, .speed = 6.7, .queue = 212.9 };
	FlexiData flexi_av33_cr66a_3 = { .edge = "Av. 33 >Oriente", .flow = 36.7 * 12, .speed = NAN, .queue = NAN };
	FlexiData flexi_av33_cr66a_4 = { .edge = "Carrera 65D >NorOriente", .flow = 68.4 * 12, .speed = NAN, .queue = NAN };
	FlexiData flexi_list_av33_cr66a[4] = { flexi_av33_cr66a_2,  flexi_av33_cr66a_4,  flexi_av33_cr66a_3, flexi_av33_cr66a_1 };

	Mostro av33_cr66a_3 = Mostro("av33_cr66a", DataSource::FLEXI);
	std::string Plan_flexi_av33_cr66a = av33_cr66a_3.suggestedPlan(flexi_list_av33_cr66a, 4);
	std::cout << Plan_flexi_av33_cr66a << std::endl;

	return 0;
}