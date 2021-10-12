/*
***************************************************************************  
**  Program  : DSMRloggerAPI.h - definitions for DSMRloggerAPI
**  Version  : v2.3.3
**
**  Copyright (c) 2021 Willem Aandewiel / Martijn Hendriks
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/  
#ifndef DSMRloggerAPI_h
#define DSMRloggerAPI_h

#include "version.h"
#include <TimeLib.h>            // https://github.com/PaulStoffregen/Time
#include <TelnetStream.h>       // https://github.com/jandrassy/TelnetStream/commit/1294a9ee5cc9b1f7e51005091e351d60c8cddecf
#include "safeTimers.h"
#include <ArduinoJson.h>
#include "Debug.h"
#include "Network.h"

#if defined( USE_BELGIUM_PROTOCOL )                           //Belgium
  //  https://github.com/mrWheel/arduino-dsmr-be.git            //Belgium
  #include <dsmr-be.h>                                          //Belgium
#else                                                           //else
  //  https://github.com/matthijskooijman/arduino-dsmr
  #include <dsmr.h>               // Version 0.1 - Commit f79c906 on 18 Sep 2018
#endif

#define _DEFAULT_HOSTNAME   "DSMR-API" 
#ifndef MQTT_CORE
  #define _DEFAULT_HOMEPAGE   "/DSMRindexEDGE.html"
#else
  #define _DEFAULT_HOMEPAGE   "/DSMRindexEDGE_core.html"
#endif
#define SETTINGS_FILE       "/DSMRsettings.json"
#define DTR_ENABLE           14
#define FLASH_BUTTON          0
#define JSON_BUFF_MAX       255
#define MQTT_BUFF_MAX       200

P1Reader    slimmeMeter(&Serial, DTR_ENABLE);

#ifndef MQTT_CORE
enum { PERIOD_UNKNOWN, HOURS, DAYS, MONTHS, YEARS };
enum E_ringfiletype {RINGHOURS, RINGDAYS, RINGMONTHS};

typedef struct {
    char filename[17];
    int8_t slots;
    unsigned int seconds;
  } S_ringfile;

//+1 voor de vergelijking, laatste record wordt niet getoond 
//onderstaande struct kan niet in PROGMEM opgenomen worden. gaat stuk bij SPIFF.open functie

const S_ringfile RingFiles[3] = {{"/RINGhours.json", 48+1,SECS_PER_HOUR}, {"/RINGdays.json",14+1,SECS_PER_DAY},{"/RINGmonths.json",24+1,0}}; 

#define DATA_FORMAT       "{\"date\":\"%-8.8s\",\"values\":[%10.3f,%10.3f,%10.3f,%10.3f,%10.3f]}"
#define DATA_RECLEN       87  //total length incl comma and new line
#define JSON_HEADER_LEN   23  //total length incl new line
#define DATA_CLOSE        2   //length last row of datafile

/**
 * Define the DSMRdata we're interested in, as well as the DSMRdatastructure to
 * hold the parsed DSMRdata. This list shows all supported fields, remove
 * any fields you are not using from the below list to make the parsing
 * and printing code smaller.
 * Each template argument below results in a field of the same name.
 */
#endif
 
using MyData = ParsedData<
  /* String */         identification
  /* String */        ,p1_version
  /* String */        ,timestamp
  /* String */        ,equipment_id
  /* FixedValue */    ,energy_delivered_tariff1
  /* FixedValue */    ,energy_delivered_tariff2
  /* FixedValue */    ,energy_returned_tariff1
  /* FixedValue */    ,energy_returned_tariff2
  /* String */        ,electricity_tariff
  /* FixedValue */    ,power_delivered
  /* FixedValue */    ,power_returned
  /* FixedValue */    ,electricity_threshold
  /* uint8_t */       ,electricity_switch_position
  /* uint32_t */      ,electricity_failures
  /* uint32_t */      ,electricity_long_failures
  /* String */        ,electricity_failure_log
  /* uint32_t */      ,electricity_sags_l1
  /* uint32_t */      ,electricity_sags_l2
  /* uint32_t */      ,electricity_sags_l3
  /* uint32_t */      ,electricity_swells_l1
  /* uint32_t */      ,electricity_swells_l2
  /* uint32_t */      ,electricity_swells_l3
  /* String */        ,message_short
  /* String */        ,message_long
  /* uint16_t */      ,fuse_treshold_l1
  /* FixedValue */    ,voltage_l1
  /* FixedValue */    ,voltage_l2
  /* FixedValue */    ,voltage_l3
  /* FixedValue */    ,current_l1
  /* FixedValue */    ,current_l2
  /* FixedValue */    ,current_l3
  /* FixedValue */    ,power_delivered_l1
  /* FixedValue */    ,power_delivered_l2
  /* FixedValue */    ,power_delivered_l3
  /* FixedValue */    ,power_returned_l1
  /* FixedValue */    ,power_returned_l2
  /* FixedValue */    ,power_returned_l3
  /* uint16_t */      ,gas_device_type
  /* String */        ,gas_equipment_id
  /* uint8_t */       ,gas_valve_position
  /* TimestampedFixedValue */ ,gas_delivered
//  /* uint16_t */      ,thermal_device_type
//  /* String */        ,thermal_equipment_id
//  /* uint8_t */       ,thermal_valve_position
//  /* TimestampedFixedValue */ ,thermal_delivered
//  /* uint16_t */      ,water_device_type
//  /* String */        ,water_equipment_id
//  /* uint8_t */       ,water_valve_position
//  /* TimestampedFixedValue */ ,water_delivered
//  /* uint16_t */      ,slave_device_type
//  /* String */        ,slave_equipment_id
//  /* uint8_t */       ,slave_valve_position
//  /* TimestampedFixedValue */ ,slave_delivered
>;

//enum    { TAB_UNKNOWN, TAB_ACTUEEL, TAB_LAST24HOURS, TAB_LAST7DAYS, TAB_LAST24MONTHS, TAB_GRAPHICS, TAB_SYSINFO, TAB_EDITOR };

const PROGMEM char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "Unknown" };

//===========================prototype's=======================================
int strcicmp(const char *a, const char *b);
void delayms(unsigned long);

//===========================GLOBAL VAR'S======================================
  WiFiClient  wifiClient;
  #include <PubSubClient.h>           // MQTT client publish and subscribe functionality
  static PubSubClient MQTTclient(wifiClient);
  
  MyData      DSMRdata;
  uint32_t    readTimer;
  time_t      actT, newT;
  char        actTimestamp[20] = "";
  char        newTimestamp[20] = "";
  uint32_t    slotErrors = 0;
  uint32_t    nrReboots  = 0;
  uint32_t    telegramCount = 0, telegramErrors = 0;
  bool        showRaw = false;
  char        BaseOTAurl[75] = "http://smart-stuff.nl/ota/";
  char        cMsg[150];
  char        lastReset[30];
  bool        spiffsNotPopulated  = false;
  bool        mqttIsConnected     = false;
  bool        Verbose1 = false, Verbose2 = false;
  int8_t      thisHour = -1, prevNtpHour = 0, thisDay = -1, thisMonth = -1, lastMonth, thisYear = 15;
  uint32_t    unixTimestamp;
  uint64_t    upTimeSeconds;
  IPAddress   ipDNS, ipGateWay, ipSubnet;
  uint8_t     settingTelegramInterval = 10; //seconden
  uint8_t     settingSmHasFaseInfo = 1;
  char        settingHostname[30] = _DEFAULT_HOSTNAME;
  char        settingIndexPage[50] = _DEFAULT_HOMEPAGE;
  char        settingMQTTbroker[101], settingMQTTuser[40], settingMQTTpasswd[30], settingMQTTtopTopic[21] = _DEFAULT_HOSTNAME;
  int32_t     settingMQTTinterval = 0, settingMQTTbrokerPort = 1883;
 #ifndef MQTT_CORE
  float     settingEDT1 = 0.1, settingEDT2 = 0.2, settingERT1 = 0.3, settingERT2 = 0.4, settingGDT = 0.5;
  float     settingENBK = 15.15, settingGNBK = 11.11;
 #endif

#if defined(HAS_NO_SLIMMEMETER)
  bool        forceBuildRingFiles = false;
  enum runStates { SInit, SMonth, SDay, SHour, SNormal };
  enum runStates runMode = SNormal;
#endif

//===========================================================================================
// setup timers 
DECLARE_TIMER_SEC(updateSeconds,       1, CATCH_UP_MISSED_TICKS);
DECLARE_TIMER_SEC(reconnectWiFi,      10);
DECLARE_TIMER_SEC(nextTelegram,       10);
DECLARE_TIMER_SEC(reconnectMQTTtimer,  5); // try reconnecting cyclus timer
DECLARE_TIMER_SEC(publishMQTTtimer,   60, SKIP_MISSED_TICKS); // interval time between MQTT messages  
DECLARE_TIMER_MIN(antiWearRing,       25); 
DECLARE_TIMER_MIN(antiWearStatus,     15); 

//DECLARE_TIMER_SEC(synchrNTP,          30);

#endif
/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
