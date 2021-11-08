#include "pb.h"
#include "pb_common.h"
#include "pb_encode.h"
#include <Arduino.h>
#include <esp32-hal.h>
#include <secrets.h>

#include <kwBoiler.h>
#include <kwHeltecWifikit32.h>
#include <kwSimpletimer.h>
#include <packet.pb.h>

#define SENSOR_TYPE       "energy"
#define FIRMWARE_VERSION  "3.0.0"
#define BOILER_SENSOR_PIN 27
#define PROTO_BUF_LENGTH  100

struct HeltecConfig config = { .ap1 = accessPoint{ WIFI_SSID1, WIFI_PASSWORD1 },
                               .ap2 = accessPoint{ WIFI_SSID2, WIFI_PASSWORD2 },
                               .ap3 = accessPoint{ WIFI_SSID3, WIFI_PASSWORD3 },
                               .rotateDisplay = true,
                               .firmwareVersion = FIRMWARE_VERSION };

kwHeltecWifikit32 heltec{ config };
kwBoiler          boiler{ BOILER_SENSOR_PIN };
kwSimpletimer     timer;
AsyncWebServer    server( 80 );
AsyncWebSocket    ws( "/ws" );

uint32_t packetID = 0;
uint8_t  stateFieldID;
uint8_t  cumulativeSecsFieldID;

bool encodeField( pb_ostream_t *ostream, const pb_field_iter_t *field,
                  SensorField *sensorField )
{
  if ( !( pb_encode_tag_for_field( ostream, field ) &&
          pb_encode_submessage( ostream, SensorField_fields, sensorField ) ) )
  {
    log_e( "Failed to encode field: %s ", ostream->errmsg );
    return false;
  }
  return true;
}

bool encodeFields( pb_ostream_t *ostream, const pb_field_iter_t *field,
                   void *const *arg )
{
  bool isOK = false;

  SensorField sensorField = SensorField_init_default;
  sensorField.sensor_name = SensorName_BOILERLIGHT;

  sensorField.which_type = SensorField_boiler_state_tag;
  sensorField.type.boiler_state = boiler.readState();
  isOK |= encodeField( ostream, field, &sensorField );

  sensorField.which_type = SensorField_boiler_on_time_tag;
  sensorField.type.boiler_on_time = boiler.activeSeconds();
  isOK |= encodeField( ostream, field, &sensorField );

  return isOK;
}

void publishEvent()
{
  uint8_t      buffer[PROTO_BUF_LENGTH] = { 0 };
  pb_ostream_t ostream = pb_ostream_from_buffer( buffer, sizeof( buffer ) );

  Packet packet = Packet_init_default;
  Meta   sensorMeta = Meta_init_default;
  strcpy( sensorMeta.device_id, heltec.deviceID );
  strcpy( sensorMeta.firmware, FIRMWARE_VERSION );
  packet.has_meta = true;
  packet.measurement_type = MeasurementType_energy;
  packet.meta = sensorMeta;
  packet.packet_id = packetID++;

  packet.sensorFields.funcs.encode = encodeFields;

  if ( !pb_encode( &ostream, Packet_fields, &packet ) )
  {
    log_e( "Failed to encode: %s", ostream.errmsg );
    return;
  }

  // for ( int i = 0; i < ostream.bytes_written; i++ )
  // {
  //   Serial.printf( "%02X", buffer[i] );
  // }

  ws.binaryAll( buffer, ostream.bytes_written );

  heltec.update( stateFieldID, boiler.readState() ? "ON" : "OFF" );
  heltec.update( cumulativeSecsFieldID, boiler.activeSeconds() );

  log_i( "State: %s, Cumulative time: %d", boiler.readState() ? "ON" : "OFF",
         boiler.activeSeconds() );

  if ( heltec.isMidnight() ) { boiler.resetActiveSeconds(); };
}

void setup()
{
  Serial.begin( 115200 );

  stateFieldID = heltec.addDisplayField( displayField{ "State", "" } );
  cumulativeSecsFieldID =
      heltec.addDisplayField( displayField{ "On", "secs" } );
  heltec.init();

  ws.onEvent( onWsEvent );
  server.addHandler( &ws );
  server.begin();

  timer.setInterval( 1000L, publishEvent );
}

void loop()
{
  heltec.run();
  timer.run();
}
