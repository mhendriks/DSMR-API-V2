/*
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * Example that shows how to parse a P1 message and automatically print
 * the result.
*/

#include "dsmr-be.h"

// Data to parse
const char raw1[] =
  "/FLU5\\253770234_A\r\n"
  "\r\n"
  "0-0:96.1.4(50213)\r\n"
  "0-0:96.1.1(3153414731313030303236393436)\r\n"
  "0-0:1.0.0(190919165430S)\r\n"
  "1-0:1.8.1(000031.250*kWh)\r\n"
  "1-0:1.8.2(000040.792*kWh)\r\n"
  "1-0:2.8.1(000061.957*kWh)\r\n"
  "1-0:2.8.2(000023.548*kWh)\r\n"
  "0-0:96.14.0(0001)\r\n"
  "1-0:1.7.0(00.342*kW)\r\n"
  "1-0:2.7.0(00.000*kW)\r\n"
  "1-0:32.7.0(236.7*V)\r\n"
  "1-0:31.7.0(002*A)\r\n"
  "0-0:96.3.10(1)\r\n"
  "0-0:17.0.0(999.9*kW)\r\n"
  "1-0:31.4.0(999*A)\r\n"
  "0-0:96.13.0()\r\n"
  "0-1:24.1.0(003)\r\n"
  "0-1:96.1.1(37464C4F32313139303035343533)\r\n"
  "0-1:24.4.0(1)\r\n"
  "0-1:24.2.3(190919165007S)(00001.315*m3)\r\n"
  "!BFE7\r\n";

const char raw2[] =
  "/FLU5\\253770234_A\r\n"
  "\r\n"
  "0-0:96.1.4(50213)\r\n"
  "0-0:96.1.1(3153414731313030303331373636)\r\n"
  "0-0:1.0.0(200104202329W)\r\n"
  "1-0:1.8.1(000180.802*kWh)\r\n"
  "1-0:1.8.2(000161.920*kWh)\r\n"
  "1-0:2.8.1(000000.001*kWh)\r\n"
  "1-0:2.8.2(000000.025*kWh)\r\n"
  "0-0:96.14.0(0002)\r\n"
  "1-0:1.7.0(00.364*kW)\r\n"
  "1-0:2.7.0(00.000*kW)\r\n"
  "1-0:32.7.0(240.9*V)\r\n"
  "1-0:31.7.0(001*A)\r\n"
  "0-0:96.3.10(1)\r\n"
  "0-0:17.0.0(999.9*kW)\r\n"
  "1-0:31.4.0(999*A)\r\n"
  "0-0:96.13.0()\r\n"
  "0-1:24.1.0(003)\r\n"
  "0-1:96.1.1(37464C4F32313139303935333131)\r\n"
  "0-1:24.4.0(1)\r\n"
  "0-1:24.2.3(200104202004W)(00385.264*m3)\r\n"
  "!A2CF\r\n";
  
const char raw3[] =
  "/FLU5\\253770234_A\r\n"
  "\r\n"
  "0-0:96.1.4(50213)\r\n"
  "0-0:96.1.1(3153414731313333333333333333)\r\n"
  "0-0:1.0.0(191219105454W)\r\n"
  "1-0:1.8.1(000000.915*kWh)\r\n"
  "1-0:1.8.2(000001.955*kWh)\r\n"
  "1-0:2.8.1(000000.000*kWh)\r\n"
  "1-0:2.8.2(000000.030*kWh)\r\n"
  "0-0:96.14.0(0001)\r\n"
  "1-0:1.7.0(00.000*kW)\r\n"
  "1-0:2.7.0(00.000*kW)\r\n"
  "1-0:32.7.0(228.3*V)\r\n"
  "1-0:31.7.0(000*A)\r\n"
  "0-0:96.3.10(1)\r\n"
  "0-0:17.0.0(999.9*kW)\r\n"
  "1-0:31.4.0(999*A)\r\n"
  "0-0:96.13.0(466C7373697373)\r\n"
  "0-1:24.1.0(003)\r\n"
  "0-1:96.1.1(37464C4F37373737373737373737)\r\n"
  "0-1:24.4.0(1)\r\n"
  "0-1:24.2.3(191219105038W)(00000.003*m3)\r\n"
  "!0D91\r\n";


  
/**
 * Define the data we're interested in, as well as the datastructure to
 * hold the parsed data. This list shows all supported fields, remove
 * any fields you are not using from the below list to make the parsing
 * and printing code smaller.
 * Each template argument below results in a field of the same name.
 */
using MyData = ParsedData<
  /* String */ identification,
  /* String */ p1_version,
  /* String */ timestamp,
  /* String */ equipment_id,
  /* FixedValue */ energy_delivered_tariff1,
  /* FixedValue */ energy_delivered_tariff2,
  /* FixedValue */ energy_returned_tariff1,
  /* FixedValue */ energy_returned_tariff2,
  /* String */ electricity_tariff,
  /* FixedValue */ power_delivered,
  /* FixedValue */ power_returned,
  /* FixedValue */ electricity_threshold,
  /* uint8_t */ electricity_switch_position,
  /* uint32_t */ electricity_failures,
  /* uint32_t */ electricity_long_failures,
  /* String */ electricity_failure_log,
  /* uint32_t */ electricity_sags_l1,
  /* uint32_t */ electricity_sags_l2,
  /* uint32_t */ electricity_sags_l3,
  /* uint32_t */ electricity_swells_l1,
  /* uint32_t */ electricity_swells_l2,
  /* uint32_t */ electricity_swells_l3,
  /* String */ message_short,
  /* String */ message_long,
  /* FixedValue */ voltage_l1,
  /* FixedValue */ voltage_l2,
  /* FixedValue */ voltage_l3,
  /* FixedValue */ current_l1,
  /* FixedValue */ current_l2,
  /* FixedValue */ current_l3,
  /* FixedValue */ fuse_treshold_l1,
  /* FixedValue */ fuse_treshold_l2,
  /* FixedValue */ fuse_treshold_l3,
  /* FixedValue */ power_delivered_l1,
  /* FixedValue */ power_delivered_l2,
  /* FixedValue */ power_delivered_l3,
  /* FixedValue */ power_returned_l1,
  /* FixedValue */ power_returned_l2,
  /* FixedValue */ power_returned_l3,
  /* uint16_t */ gas_device_type,
  /* String */ gas_equipment_id,
  /* uint8_t */ gas_valve_position,
  /* TimestampedFixedValue */ gas_delivered,
  /* uint16_t */ thermal_device_type,
  /* String */ thermal_equipment_id,
  /* uint8_t */ thermal_valve_position,
  /* TimestampedFixedValue */ thermal_delivered,
  /* uint16_t */ water_device_type,
  /* String */ water_equipment_id,
  /* uint8_t */ water_valve_position,
  /* TimestampedFixedValue */ water_delivered,
  /* uint16_t */ slave_device_type,
  /* String */ slave_equipment_id,
  /* uint8_t */ slave_valve_position,
  /* TimestampedFixedValue */ slave_delivered
>;

/**
 * This illustrates looping over all parsed fields using the
 * ParsedData::applyEach method.
 *
 * When passed an instance of this Printer object, applyEach will loop
 * over each field and call Printer::apply, passing a reference to each
 * field in turn. This passes the actual field object, not the field
 * value, so each call to Printer::apply will have a differently typed
 * parameter.
 *
 * For this reason, Printer::apply is a template, resulting in one
 * distinct apply method for each field used. This allows looking up
 * things like Item::name, which is different for every field type,
 * without having to resort to virtual method calls (which result in
 * extra storage usage). The tradeoff is here that there is more code
 * generated (but due to compiler inlining, it's pretty much the same as
 * if you just manually printed all field names and values (with no
 * cost at all if you don't use the Printer).
 */
struct Printer {
  template<typename Item>
  void apply(Item &i) {
    if (i.present()) {
      Serial.print(Item::name);
      Serial.print(F(": "));
      Serial.print(i.val());
      Serial.print(Item::unit());
      Serial.println();
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("\r\nand then it begins ...\r\n\n");

  MyData data;
  ParseResult<void> res;
  
  Serial.println("\r\n=============== raw1 Telegram ==============================\r\n");
  Serial.print(raw1);
  Serial.println("\r\n--------------- parsed -------------------------------------\r\n");
  res = P1Parser::parse(&data, raw1, lengthof(raw1), true);
  if (res.err) {
    // Parsing error, show it
    Serial.println(res.fullError(raw1, raw1 + lengthof(raw1)));
  } else {
    // Parsed succesfully, print all values
    data.applyEach(Printer());
  }
  
  data = {};
  Serial.println("\r\n=============== raw2 Telegram ==============================\r\n");
  Serial.print(raw2);
  Serial.println("\r\n--------------- parsed -------------------------------------\r\n");
  res = P1Parser::parse(&data, raw2, lengthof(raw2), true);
  if (res.err) {
    // Parsing error, show it
    Serial.println(res.fullError(raw2, raw2 + lengthof(raw2)));
  } else {
    // Parsed succesfully, print all values
    data.applyEach(Printer());
  }
  
  data = {};
  Serial.println("\r\n=============== raw3 Telegram ==============================\r\n");
  Serial.print(raw3);
  Serial.println("\r\n--------------- parsed -------------------------------------\r\n");
  res = P1Parser::parse(&data, raw3, lengthof(raw3), true);
  if (res.err) {
    // Parsing error, show it
    Serial.println(res.fullError(raw3, raw3 + lengthof(raw3)));
  } else {
    // Parsed succesfully, print all values
    data.applyEach(Printer());
  }
  
} // setup()

void loop () {
}
