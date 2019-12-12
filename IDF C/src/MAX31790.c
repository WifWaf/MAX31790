#include "MAX31790.h"
#include "I2CManager.h"
#include <driver/i2c.h>
#include <math.h>

#include "esp_log.h"
#define TAG "MAX31790"

max31790_master_config_t *max31790_config;

uint8_t sr_map[6] = {1, 2, 4, 8, 16, 32};

//static float bits_to_fduty(uint16_t bits);
//static uint16_t fduty_to_bits(float duty);

void MAX31790_initiate(max31790_master_config_t *cfg)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}  

void MAX31790_set_master_config(max31790_master_config_t *cfg, uint8_t *w_buff)
{
    /* End writing to fancon */ 
}

uint32_t MAX31790_get_rpm(uint8_t fan_number, bool isTarget, uint8_t *r_buff)
{
    uint32_t buff = 0;

    if(!CHCK_TACH_CHAN(fan_number))
        return 0;

    MAX31790_read((isTarget ? MAX31790_REG_TARGET_COUNT(FAN_TO_CHAN(fan_number)) : MAX31790_REG_TACH_COUNT(fan_number)), r_buff, 2);
    buff = REG_TO_LFTJST(11, r_buff[0], r_buff[1]);        
    
    buff = CALC_RPM_OR_BIT(buff, sr_map[(MAX31790_FAN_DYN_SR_MASK & max31790_config->fan_dyn[FAN_TO_CHAN(fan_number)]) >> 5], max31790_config->fan_hallcount[fan_number]); 

    return buff;
}

void MAX31790_set_target_rpm(uint8_t channel, uint32_t RPM)
{
    uint16_t calc = 0;
    uint8_t w_buff[2] = {0, 0};

    if(!CHCK_CHAN(channel))
        return;

    RPM = CONSTRAIN(RPM, RPM_MIN, RPM_MAX);

    calc = CALC_RPM_OR_BIT(RPM, sr_map[(MAX31790_FAN_DYN_SR_MASK & max31790_config->fan_dyn[channel]) >> 5], max31790_config->fan_hallcount[channel]);

    w_buff[0] = LFTJST_TO_MSB(calc, 11);
    w_buff[1] = LFTJST_TO_LSB(calc, 11);

    MAX31790_write(MAX31790_REG_TARGET_COUNT(channel), w_buff, 2);
}

void MAX31790_set_target_dutybits(uint8_t channel, uint16_t dutybits)
{
    uint8_t w_buff[2] = {0, 0};

    if(!CHCK_CHAN(channel))
        return; 

    dutybits = CONSTRAIN(dutybits, 0, 511);

    w_buff[0] = LFTJST_TO_MSB(dutybits, 9);
    w_buff[1] = LFTJST_TO_LSB(dutybits, 9);

    MAX31790_write(MAX31790_REG_TARGET_DUTY(channel), w_buff, 2); 
}

void MAX31790_set_fault_mask(uint8_t fan_number)
{ 
    uint8_t u8fmask, *pr_u8fmask = &u8fmask;

    if(!CHCK_TACH_CHAN(fan_number))
        return;

    if(fan_number > 5)
    {
        max31790_config->fault_mask_2 |= (0x01 << (6 - fan_number));
        *pr_u8fmask = max31790_config->fault_mask_2;
    }
    else
    {
        max31790_config->fault_mask_1 |= (0x01 << (6 - fan_number));
        *pr_u8fmask = max31790_config->fault_mask_1;
    } 

    MAX31790_write((fan_number > 5) ? MAX31790_REG_FAN_FAULT_MASK_2 : MAX31790_REG_FAN_FAULT_MASK_1, pr_u8fmask, 1);
}

uint16_t MAX31790_get_dutybits(uint8_t channel, bool isTarget)
{
    uint16_t calc = 0;
    uint8_t r_buff[2] = {0, 0};

    if(CHCK_CHAN(channel))
    {
        MAX31790_read((isTarget ? MAX31790_REG_TARGET_DUTY(channel) : MAX31790_REG_PWM_DUTY(channel)), r_buff, 2);
        calc = REG_TO_LFTJST(9, r_buff[0], r_buff[1]);
    }

    return calc;
}

void MAX31790_write(uint8_t w_adr, uint8_t *w_data, uint8_t w_len)
{
    if(!xSemaphoreTake(xI2CBinary, pdMS_TO_TICKS(1000)))
        ESP_LOGE(TAG,"Failed to take xI2CBinary token");  
    else
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);    

        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (0x20 << 1) | I2C_MASTER_WRITE, true));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, w_adr, true));

        ESP_ERROR_CHECK(i2c_master_write(cmd, w_data, w_len, true));

        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(500)));

        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(xI2CBinary);
    }
}

void MAX31790_read(uint8_t r_adr, uint8_t *r_data, uint8_t r_len)
{
    if(!xSemaphoreTake(xI2CBinary, pdMS_TO_TICKS(1000)))
        ESP_LOGE(TAG,"Failed to take xI2CBinary token");        
    else
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (0x20 << 1) | I2C_MASTER_WRITE, true));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, r_adr, true));

        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (0x20 << 1) | I2C_MASTER_READ, true));

        if (r_len > 1)
            ESP_ERROR_CHECK(i2c_master_read(cmd, r_data, r_len-1, I2C_MASTER_ACK));
   
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, r_data + r_len-1, I2C_MASTER_NACK));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));

        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(500)));

        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(xI2CBinary);
    }    
}

float MAX31790_bits_to_fduty(uint16_t bits)
{
    float calc = bits;   
    calc = round(MAP((calc*10), 0, 511,0, 100));
    return calc /= 10;
}

uint16_t fduty_to_bits(float duty)
{
    float calc = duty;   
    return round(MAP(calc, 0, 100, 0, 511)); 
}