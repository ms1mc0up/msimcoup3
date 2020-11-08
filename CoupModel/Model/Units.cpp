
#include "Units.h"




 Units::Units() {  
	 string unit, m2;
	SetUnitMap(WATERMASS_UNIT, "mm");
	SetUnitMap(WATERFLOW_UNIT,"mm/day");
	SetUnitMap(INVERSE_WATERFLOW_UNIT, "day/mm");
	
	SetUnitMap(WATERCONTENT_UNIT, "Vol %");
	SetUnitMap(PRESSUREHEAD_UNIT,"cm water");
	unit = "J/m"; unit += char(178); unit += "/day";
	SetUnitMap(HEATFLOW_UNIT,unit);
	unit = "J/m"; unit += char(178); unit += "/day";
	SetUnitMap(HEATFLOW_UNIT, unit);
	SetUnitMap(HEATSOURCE_UNIT, unit);
	unit = "MJ/m"; unit += char(178); unit += "/day";
	SetUnitMap(MEGA_HEATFLOW_UNIT, unit);

	
	unit=char(176);unit+='C';
	SetUnitMap(TEMP_UNIT, unit);
	unit += "day";
	SetUnitMap(TEMPSUM_UNIT, unit);

	unit = "W/m/";	unit += char(176); unit += 'C';
	SetUnitMap(THERMAL_CONDUCTIVY_UNIT, unit);


	m2 = "m"; m2 += char(178);
	string str;
	str = "day";
	str+=char(178);
	str += "m6/g";
	str+=char(178);
	SetUnitMap(INVERSE_SQUARED_FLUX_PER_VOLUME, str);
	SetUnitMap(MASS_RATIO_UNIT, "g/g");
	SetUnitMap(VOLUME_RATIO_UNIT, "m3/m3");

	SetUnitMap(HEATSTORE_UNIT, "J/" + m2);
	SetUnitMap(MEGA_HEATSTORE_VOL_UNIT, "MJ/m3");
	unit = "W/m/"; unit += char(176); unit += "C";
	SetUnitMap(HEATCOND_UNIT, unit);
	unit = "Kg/m", unit += char(179);
	SetUnitMap(KILO_MASSDENSITY_UNIT, unit);
	
	unit = "g/m", unit += char(179);
	SetUnitMap(MASSDENSITY_UNIT,unit);
	SetUnitMap(AREA_UNIT,m2);
	
	SetUnitMap(MASS_UNIT,"g/"+m2);
	SetUnitMap(INVERSE_MASS_UNIT, m2+"/g");
	SetUnitMap(CARBON_MASS_UNIT, "gC/" + m2);
	SetUnitMap(NITROGEN_MASS_UNIT, "gN/" + m2);


	unit = "g/" + m2 + "/day";
	SetUnitMap(MASSFLOW_UNIT, unit);
	unit = "gC/" + m2 + "/day";
	SetUnitMap(CARBON_MASSFLOW_UNIT, unit);
	unit = "gN/" + m2 + "/day";
	SetUnitMap(NITROGEN_MASSFLOW_UNIT, unit);

	
	unit = "g/m", unit += char(178);
	SetUnitMap(GAS_MASS_UNIT, unit);
	SetUnitMap(GAS_CONC_UNIT,"PPM");
	
	SetUnitMap(YEAR_AGE_UNIT ,"yr");
	
	unit = "m", unit += char(178); unit += "/day";
	SetUnitMap(AREA_PER_TIME_UNIT, unit);
	unit = "s/m";
	SetUnitMap(AERODYNAMIC_RESISTANCE_UNIT, unit);
	unit = "m/s";
	SetUnitMap(WIND_SPEED_UNIT, unit);

	unit = "cm", unit += char(178); unit += "/s";
	SetUnitMap(CM_AREA_PER_SEC_TIME_UNIT, unit);

	
	unit = "mg/m"; unit += char(178);
	SetUnitMap(mMASS_UNIT, unit);
	unit+= "/day";
	SetUnitMap(mMASSFLOW_UNIT, unit);
	unit = char(181);
	unit += "g/m"; unit += char(178); unit += "/day"; 
	SetUnitMap(MIKRO_MASS_FLOW_UNIT, unit);



	unit = char(181); unit += "mol/m"; unit += char(178); unit += "/s";
	SetUnitMap(MIKRO_MOL_SEC_RATE, unit);
	SetUnitMap(PRESSURE_UNIT, "Pa");
	SetUnitMap(LENGTH_UNIT, "m");
	SetUnitMap(LENGTH_PER_TIME_UNIT, "m/s");
	SetUnitMap(RATECOEF_UNIT, "1/day");

	SetUnitMap(NO_UNIT, "-");
	SetUnitMap(PERCENTAGE, "%");
	SetUnitMap(ENERGY_MOLE, "J/mole");
	SetUnitMap(mMASS_LITER_UNIT, "mg/l");
	unit = char(181); unit += "g/l";
	SetUnitMap(mikro_MASS_LITER_UNIT, unit);
	unit = char(181); unit += "g/g";
	SetUnitMap(MIKRO_MASS_RATIO, unit);
	unit = char(181); unit += "g/day";
	SetUnitMap(MIKRO_MASS_RATE, unit);
	unit = "day";
	SetUnitMap(TIME_UNIT, unit);
	unit = "minutes";
	SetUnitMap(TIME_MINUTE_UNIT, unit);
	unit = "rad";
	SetUnitMap(ANGLE_UNIT, unit);

	auto koll = GetAllStringBasedUnits();
 }

 Units::~Units()
 {
 }
	
