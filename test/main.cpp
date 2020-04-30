#include <infmostro/mostro.h>

int main()
{

	/******************************************** FLEXI EXAMPLES **************************************************************/
	FlexiData flexi_cll44_cr84_1 = { .edge = "Av. San Juan >Oriente", .flow = 80.2 * 12, .speed = 8.9, .queue = 140.8 };
	FlexiData flexi_cll44_cr84_2 = { .edge = "Av. San Juan >Occidente", .flow = 48.7 * 12, .speed = 11.0, .queue = 90.5 };
	FlexiData flexi_cll44_cr84_3 = { .edge = "Carrera 84 >(N)", .flow = 100.1 * 12, .speed = 7.5, .queue = 96.3 };
	FlexiData flexi_cll44_cr84_4 = { .edge = "Carrera 84 >(S)", .flow = 104.9 * 12, .speed = 6.7, .queue = 102.4 };
	FlexiData flexi_list_cll44_cr84[4] = { flexi_cll44_cr84_3,  flexi_cll44_cr84_2,  flexi_cll44_cr84_1, flexi_cll44_cr84_4 };

	Mostro cll44_cr84 = Mostro("cll44_cr84", DataSource::FLEXI);
	std::string Plan_flexi_cll44_cr84 = cll44_cr84.suggestedPlan(flexi_list_cll44_cr84, 4);
	std::cout << Plan_flexi_cll44_cr84 << std::endl;



	FlexiData flexi_av33_cr66a_1 = { .edge = "Av. 33 >Occidente", .flow = 40.8 * 12, .speed = 11.0, .queue = 352.6 };
	FlexiData flexi_av33_cr66a_2 = { .edge = "Carrera 65D >SurOccidente", .flow = 93.8 * 12, .speed = 6.7, .queue = 212.9 };
	FlexiData flexi_av33_cr66a_3 = { .edge = "Av. 33 >Oriente", .flow = 36.7 * 12, .speed = NAN, .queue = NAN };
	FlexiData flexi_av33_cr66a_4 = { .edge = "Carrera 65D >NorOriente", .flow = 68.4 * 12, .speed = NAN, .queue = NAN };
	FlexiData flexi_list_av33_cr66a[4] = { flexi_av33_cr66a_2,  flexi_av33_cr66a_4,  flexi_av33_cr66a_3, flexi_av33_cr66a_1 };

	Mostro av33_cr66a = Mostro("av33_cr66a", DataSource::FLEXI);
	std::string Plan_flexi_av33_cr66a = av33_cr66a.suggestedPlan(flexi_list_av33_cr66a, 4);
	std::cout << Plan_flexi_av33_cr66a << std::endl;

	/********************************************* ARS EXAMPLES ***************************************************************/

	ArsData ars_cll44_cr65_1  = { .id = "XC-1-JUAN-65-SAL:OCC-ORI 1", .flow1 = 8.2,  .flow2 = 2.6, .flow3 = 2.2, .speed = 4.7 };
	ArsData ars_cll44_cr65_2  = { .id = "XC-2-JUAN-65-SAL:OCC-ORI 1", .flow1 = 12.3, .flow2 = 3.9, .flow3 = 3.3, .speed = 6.2 };
	ArsData ars_cll44_cr65_3  = { .id = "XC-1-JUAN-65-SAL:OCC-ORI 2", .flow1 = 17.7, .flow2 = 5.7, .flow3 = 4.7, .speed = 12.8 };
	ArsData ars_cll44_cr65_4  = { .id = "XC-2-JUAN-65-SAL:OCC-ORI 2", .flow1 = 14.5, .flow2 = 4.7, .flow3 = 3.9, .speed = 13.1 };
	ArsData ars_cll44_cr65_5  = { .id = "XC-1-JUAN-65-ENT:SUR-NOR",   .flow1 = 13.2, .flow2 = 4.2, .flow3 = 3.5, .speed = 14.9 };
	ArsData ars_cll44_cr65_6  = { .id = "XC-2-JUAN-65-ENT:SUR-NOR",   .flow1 = 19.1, .flow2 = 6.1, .flow3 = 5.1, .speed = 15.3 };
	ArsData ars_cll44_cr65_7  = { .id = "XC-3-JUAN-65-ENT:SUR-NOR",   .flow1 = 9.1,  .flow2 = 2.9, .flow3 = 2.4, .speed = 13.3 };
	ArsData ars_cll44_cr65_9  = { .id = "XC-2-JUAN-65-ENT:OCC-ORI 1", .flow1 = 17.3, .flow2 = 5.5, .flow3 = 4.4, .speed = 19.0 };
	ArsData ars_cll44_cr65_8  = { .id = "XC-1-JUAN-65-ENT:OCC-ORI 1", .flow1 = 16.4, .flow2 = 5.2, .flow3 = 4.6, .speed = 16.4 };
	ArsData ars_cll44_cr65_10 = { .id = "XC-1-JUAN-65-ENT:OCC-ORI 2", .flow1 = 28.2, .flow2 = 9.0, .flow3 = 7.5, .speed = 27.2 };
	ArsData ars_cll44_cr65_11 = { .id = "XC-2-JUAN-65-ENT:OCC-ORI 2", .flow1 = 20.5, .flow2 = 6.5, .flow3 = 5.5, .speed = 29.1 };
	ArsData ars_cll44_cr65_12 = { .id = "XC-1-JUAN-65-SAL:ORI-OCC 1", .flow1 = 10.9, .flow2 = 3.5, .flow3 = 2.9, .speed = 10.3 };
	ArsData ars_cll44_cr65_13 = { .id = "XC-2-JUAN-65-SAL:ORI-OCC 1", .flow1 = 12.7, .flow2 = 4.1, .flow3 = 3.4, .speed = 8.9 };
	ArsData ars_cll44_cr65_14 = { .id = "XC-1-JUAN-65-SAL:ORI-OCC 2", .flow1 = 15.0, .flow2 = 4.8, .flow3 = 4.0, .speed = 12.7 };
	ArsData ars_cll44_cr65_15 = { .id = "XC-2-JUAN-65-SAL:ORI-OCC 2", .flow1 = 19.1, .flow2 = 6.1, .flow3 = 5.1, .speed = 16.3 };
	ArsData ars_cll44_cr65_16 = { .id = "XC-1-JUAN-65-SAL:SUR-NOR",   .flow1 = 16.8, .flow2 = 5.4, .flow3 = 4.5, .speed = 14.4 };
	ArsData ars_cll44_cr65_17 = { .id = "XC-2-JUAN-65-SAL:SUR-NOR",   .flow1 = 14.1, .flow2 = 4.5, .flow3 = 3.8, .speed = 14.8 };
	ArsData ars_cll44_cr65_18 = { .id = "XC-3-JUAN-65-SAL:SUR-NOR",   .flow1 = 15.9, .flow2 = 5.1, .flow3 = 4.2, .speed = 15.0 };
	ArsData ars_cll44_cr65_19 = { .id = "XC-1-JUAN-65-ENT:ORI-OCC 1", .flow1 = 17.7, .flow2 = 5.7, .flow3 = 4.7, .speed = 14.9 };
	ArsData ars_cll44_cr65_20 = { .id = "XC-2-JUAN-65-ENT:ORI-OCC 1", .flow1 = 14.5, .flow2 = 4.7, .flow3 = 3.9, .speed = 17.6 };
	ArsData ars_cll44_cr65_21 = { .id = "XC-1-JUAN-65-ENT:ORI-OCC 2", .flow1 = 23.6, .flow2 = 7.6, .flow3 = 6.3, .speed = 16.2 };
	ArsData ars_cll44_cr65_22 = { .id = "XC-2-JUAN-65-ENT:ORI-OCC 2", .flow1 = 19.5, .flow2 = 6.3, .flow3 = 5.2, .speed = 16.5 };

	ArsData ars_list_cll44_cr65[22] = { ars_cll44_cr65_20, ars_cll44_cr65_9, ars_cll44_cr65_13, ars_cll44_cr65_1,
		ars_cll44_cr65_19, ars_cll44_cr65_7, ars_cll44_cr65_5, ars_cll44_cr65_12, ars_cll44_cr65_17, ars_cll44_cr65_2,
		ars_cll44_cr65_16, ars_cll44_cr65_6, ars_cll44_cr65_14, ars_cll44_cr65_18, ars_cll44_cr65_3, ars_cll44_cr65_15,
		ars_cll44_cr65_21, ars_cll44_cr65_4, ars_cll44_cr65_8, ars_cll44_cr65_10, ars_cll44_cr65_22, ars_cll44_cr65_11
	};

	Mostro cll44_cr65 = Mostro("cll44_cr65", DataSource::ARS);
	std::string Plan_ars_cll44_cr65 = cll44_cr65.suggestedPlan(ars_list_cll44_cr65, 22);
	std::cout << Plan_ars_cll44_cr65 << std::endl;

	return 0;
}