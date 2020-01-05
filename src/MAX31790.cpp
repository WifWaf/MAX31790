/****************************************************** 
  Arduino library for MAX31790 Fan Controler
  
  Author: Jonathan Dempsey JDWifWaf@gmail.com  
  Version: 1.0.1
  License: Apache 2.0
 *******************************************************/

#include "MAX31790.h"

MAX31790::MAX31790(uint8_t adr) : _adr(adr){}

void MAX31790::begin(TwoWire &inWire)
{
    this->myWire = &inWire;
    this->myWire->begin();
}

void MAX31790::setMasterConfig(max31790_master_config_t *cfg)
{
   /* Copy config to lib instance */
   memcpy(&this->max31790_config, cfg, sizeof(this->max31790_config));

   /* Begin writing to fancon */

   setGlobalConfig(max31790_config.global_cfg);
   setFailedFanSeqStart(max31790_config.fan_failed_seq_start_cfg);

   for(int x = 0; x < 6; x++)
   {
       setFanConfig(max31790_config.fan_cfg[x], x);
       setFanDynamic(max31790_config.fan_dyn[x], x);
   }

   write(MAX31790_REG_FAN_FAULT_MASK_1, max31790_config.fault_mask_1);
   write(MAX31790_REG_FAN_FAULT_MASK_2, max31790_config.fault_mask_2);

    /* End writing to fancon */ 
}

uint32_t MAX31790::getRPM(uint8_t fan_number, bool isTarget)
{
    uint32_t buff = 0;

    if(CHCK_TACH_CHAN(fan_number))
    {
        buff = read((isTarget ? MAX31790_REG_TARGET_COUNT(FAN_TO_CHAN(fan_number)) : MAX31790_REG_TACH_COUNT(fan_number)),  11);        
        buff = CALC_RPM_OR_BIT(buff, sr_map[(MAX31790_FAN_DYN_SR_MASK & max31790_config.fan_dyn[FAN_TO_CHAN(fan_number)]) >> 5], max31790_config.fan_hallcount[fan_number]); 
    }

    return buff;
}

void MAX31790::setTargetTach(uint8_t channel, uint32_t RPM)
{
    uint16_t buff = 0;

    RPM = constrain(RPM, RPM_MIN, RPM_MAX);

    if(CHCK_CHAN(channel))
    {
        buff = CALC_RPM_OR_BIT(RPM, sr_map[(MAX31790_FAN_DYN_SR_MASK & max31790_config.fan_dyn[channel]) >> 5], max31790_config.fan_hallcount[channel]);
        write(MAX31790_REG_TARGET_COUNT(channel), buff, 11);
    }
}

void MAX31790::setTargetDuty(uint8_t channel, float duty)
{
    uint16_t buff = 0;

    if(CHCK_CHAN(channel))
    {
        duty = constrain(duty, 0, 100);
        buff = map(duty, 0, 100, 0, 511);
        write(MAX31790_REG_TARGET_DUTY(channel), buff, 9);        
    }
}

void MAX31790::setTargetDutyBits(uint8_t channel, uint16_t duty)
{
    if(CHCK_CHAN(channel))
    {
        duty = constrain(duty, 0, 511);
        write(MAX31790_REG_TARGET_DUTY(channel), duty, 9);        
    }
}

void MAX31790::setFaultMask(uint8_t fan_number)
{ 
    if(fan_number > 5)
    {
        max31790_config.fault_mask_2 |= (0x01 << (6 - fan_number));
        write(MAX31790_REG_FAN_FAULT_MASK_2, max31790_config.fault_mask_2);        
    }
    else
    {
        max31790_config.fault_mask_1 |= (0x01 << (6 - fan_number));
        write(MAX31790_REG_FAN_FAULT_MASK_1, max31790_config.fault_mask_1);     
    } 
}

uint8_t MAX31790::getDuty(uint8_t channel, bool isTarget)
{
    float calc = 0;

    if(CHCK_CHAN(channel))
    {
        calc = read((isTarget ? MAX31790_REG_TARGET_DUTY(channel) : MAX31790_REG_PWM_DUTY(channel)), 9);
        calc = (uint16_t)map(calc, 0, 511, 0, 1000);
        calc /= 10;
        calc = round(calc);
    }

    return (uint8_t)calc;
}

void MAX31790::write(uint8_t r_adr, uint16_t payload, uint8_t ljst)
{
    uint8_t LSB = 0, MSB = 0;

    this->myWire->beginTransmission(_adr);
    this->myWire->write(r_adr);

    MSB |= 0xFF & (payload >> (ljst ? (ljst - 8) : 0));

    this->myWire->write(MSB);
    
    if(0xFF00 & payload)
    {
        LSB = 0xFF & (payload << (16 - ljst));
        this->myWire->write(LSB);
    }
    this->myWire->endTransmission();
}

uint16_t MAX31790::read(uint8_t r_adr, uint8_t ljst)
{
    uint16_t buff16 = 0;
    uint8_t buff8 = 0;

    this->myWire->beginTransmission(_adr);
    this->myWire->write(r_adr);
    this->myWire->endTransmission(false);

    this->myWire->requestFrom(_adr, (uint8_t)(ljst ? 2 : 1));

    if(this->myWire->available())
    {   
        buff8 = this->myWire->read();
        if(ljst)
        {
            buff16 |= (buff8 << (ljst - 8));
            buff8 = this->myWire->read();
            buff16 |= (buff8 >> (16 - ljst));
        }        
    }
    return ljst? buff16 : buff8;
}