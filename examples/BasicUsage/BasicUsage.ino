#include <Arduino.h>
#include "MAX31790.h"

// I2C pins //
#define SDA 25
#define SDL 26

// Hallcount (in this case for all fans)
#define HALLCOUNT 2

// Library instance //
MAX31790 FanCon;

// create configuration struct instance //
max31790_master_config_t max31790_cfg;

void setup() 
{
  // setup Wire I2C with I2C pins //
  Wire.begin(SDA, SDL);
  
  // pass TwoWire reference to library for I2C protocol //
  FanCon.begin(Wire);
  
  // set global cfg //
  max31790_cfg.global_cfg = 0xFF & (MAX31790_GLO_RUN_STANDBY_RUN | MAX31790_GLO_RESET_NORMAL | MAX31790_GLO_BUS_TIMEOUT_EN | MAX31790_GLO_OSC_INT | MAX31790_GLO_I2C_WD_DIS); //summaries to 0x00! However, clearly shows the thought process
  
  // set fan failed sequencial start config //
  max31790_cfg.fan_failed_seq_start_cfg = 0xFF & (MAX31790_FAN_FAILED_SEQ_SSD_500 | MAX31790_FAN_FAILED_SEQ_FFO_CON | MAX31790_FAN_FAILED_SEQ_FFQ_2);

  // set fan config //
  max31790_cfg.fan_cfg[0] |= (0xFF & (MAX31790_FAN_CFG_SPIN_UP_0_5 |  MAX31790_FAN_CFG_TACH_INPUT));
  max31790_cfg.fan_cfg[1] |= (0xFF & (MAX31790_FAN_CFG_SPIN_UP_0_5 |  MAX31790_FAN_CFG_TACH_INPUT));
  max31790_cfg.fan_cfg[2] |= (0xFF & (MAX31790_FAN_CFG_SPIN_UP_0_5 |  MAX31790_FAN_CFG_TACH_INPUT));
  max31790_cfg.fan_cfg[3] |= (0xFF & (MAX31790_FAN_CFG_CON_MON_MON | MAX31790_FAN_CFG_TACH_INPUT | MAX31790_FAN_CFG_PWM_TACH_TACH));
  max31790_cfg.fan_cfg[4] |= (0xFF & (MAX31790_FAN_CFG_CON_MON_MON | MAX31790_FAN_CFG_TACH_INPUT | MAX31790_FAN_CFG_PWM_TACH_TACH));
  max31790_cfg.fan_cfg[5] |= (0xFF & (MAX31790_FAN_CFG_SPIN_UP_0_5 |  MAX31790_FAN_CFG_TACH_INPUT));
  
  // set fan dynamic profiles for all channels //
  for(uint8_t x = 0; x < NUM_CHANNEL; x++)
   max31790_cfg.fan_dyn[x] |= (0xFF & (MAX31790_FAN_DYN_SR_4 | MAX31790_FAN_DYN_PWM_ROC_7_8));
   
  // set hall counts for all fans //
  for(uint8_t x = 0; x < NUM_TACH_CHANNEL; x++)
   max31790_cfg.fan_hallcount[x] = HALLCOUNT;

  // disable faults for all fans //
  max31790_cfg.fault_mask_1 = 0xFF & (MAX31790_FAN_FAULT_1_7 | MAX31790_FAN_FAULT_2_8 | MAX31790_FAN_FAULT_3_9 | MAX31790_FAN_FAULT_4_10 | MAX31790_FAN_FAULT_5_11 | MAX31790_FAN_FAULT_6_12);
  max31790_cfg.fault_mask_2 = 0xFF & (MAX31790_FAN_FAULT_1_7 | MAX31790_FAN_FAULT_2_8 | MAX31790_FAN_FAULT_3_9 | MAX31790_FAN_FAULT_4_10 | MAX31790_FAN_FAULT_5_11 | MAX31790_FAN_FAULT_6_12);
  
  // pass to library as reference //
  FanCon.setMasterConfig(&max31790_cfg);
  
  // set channels 1-3 and 4-6 to 25kHz (note channels 4-6 resides in bits 7:4, hence << 4) //
  FanCon.setFrequencyByte(0xFF & (MAX31790_PWM_FREQ_25K | (MAX31790_PWM_FREQ_25K << 4)));
}

void loop()
{ 
  for(uint8_t x = 0; x < 100; x += 10)
  {
    for(uint8_t y = 0; y < 6; y++)
    {
      FanCon.setTargetDuty(y, x);
      ESP_LOGD("MAIN", "Channel: %d Duty: %d,", y, FanCon.getTargetDuty(y));
    }
    delay(1000);
  }
}