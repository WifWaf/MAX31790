/****************************************************** 
  Arduino library for MAX31790 Fan Controler
  
  Author: Jonathan Dempsey JDWifWaf@gmail.com  
  Version: 1.0.0
  License: Apache 2.0
 *******************************************************/

#ifndef MAX31790_H
#define MAX31790_H

#define TAG MAX31790

#include <Arduino.h>
#include <Wire.h>

/* MAX31790 Addresses ---------------------- */
#define MAX31790_DEF_ADDR               0x20    // 0x40 >> 1  ADD0: GND ADD1: GND

/* MAX31790 Options --------------------------------- */ 
// Global Config Bits //
#define MAX31790_GLO_RUN_STANDBY_RUN        0x00
#define MAX31790_GLO_RUN_STANDBY_STANDBY    0x80
#define MAX31790_GLO_RESET_NORMAL           0x00
#define MAX31790_GLO_RESET_RESET            0x40
#define MAX31790_GLO_BUS_TIMEOUT_DIS        0x20
#define MAX31790_GLO_OSC_INT                0x00
#define MAX31790_GLO_OSC_EXT                0x08
#define MAX31790_GLO_I2C_WD                 0x04
#define MAX31790_GLO_I2C_WD_STATUS          0x01

// Fan Config Bits //
#define MAX31790_FAN_CFG_MODE_PWM           0x00
#define MAX31790_FAN_CFG_MODE_RPM           0x80
#define MAX31790_FAN_CFG_SPIN_UP_0_5        0x20
#define MAX31790_FAN_CFG_SPIN_UP_1          0x30
#define MAX31790_FAN_CFG_SPIN_UP_2          0x40
#define MAX31790_FAN_CFG_CON_MON_CON        0x00
#define MAX31790_FAN_CFG_CON_MON_MON        0x10
#define MAX31790_FAN_CFG_TACH_INPUT  	    0x08
#define MAX31790_FAN_CFG_TACH_LOCK_TACH	    0x00
#define MAX31790_FAN_CFG_TACH_LOCK_LOCK	    0x04
#define MAX31790_FAN_CFG_PWM_TACH_PWM       0x00
#define MAX31790_FAN_CFG_PWM_TACH_TACH      0x01

// Fan Dynamic Bits //
#define MAX31790_FAN_DYN_SR_1               0x00
#define MAX31790_FAN_DYN_SR_2               0x20
#define MAX31790_FAN_DYN_SR_4               0x40
#define MAX31790_FAN_DYN_SR_8               0xC0
#define MAX31790_FAN_DYN_SR_16              0x80
#define MAX31790_FAN_DYN_SR_32	            0xA0
#define MAX31790_FAN_DYN_SR_MASK	        0xE0

#define MAX31790_FAN_DYN_PWM_ROC_0	        0x00
#define MAX31790_FAN_DYN_PWM_ROC_1_9	    0x04
#define MAX31790_FAN_DYN_PWM_ROC_3_9	    0x08
#define MAX31790_FAN_DYN_PWM_ROC_7_8	    0x0C
#define MAX31790_FAN_DYN_PWM_ROC_15_6	    0x10
#define MAX31790_FAN_DYN_PWM_ROC_31_2	    0x14
#define MAX31790_FAN_DYN_PWM_ROC_62_5	    0x18
#define MAX31790_FAN_DYN_PWM_ROC_125        0x1C
#define MAX31790_FAN_DYN_PWM_ROC_MASK       0x1C

#define MAX31790_FAN_DYN_ASYM_ROC           0x02

// Fan Failed / Sequetional Start //
#define MAX31790_FAN_FAILED_SEQ_SSD_0       0x00
#define MAX31790_FAN_FAILED_SEQ_SSD_250     0x20
#define MAX31790_FAN_FAILED_SEQ_SSD_500     0x40
#define MAX31790_FAN_FAILED_SEQ_SSD_1000    0xC0
#define MAX31790_FAN_FAILED_SEQ_SSD_2000    0x80
#define MAX31790_FAN_FAILED_SEQ_SSD_4000    0xA0
#define MAX31790_FAN_FAILED_SEQ_SSD_MASK    0xE0

#define MAX31790_FAN_FAILED_SEQ_FFO_0       0xE0
#define MAX31790_FAN_FAILED_SEQ_FFO_CON     0x04
#define MAX31790_FAN_FAILED_SEQ_FFO_100     0x08
#define MAX31790_FAN_FAILED_SEQ_FFO_A100    0x0C
#define MAX31790_FAN_FAILED_SEQ_FFO_MASK    0x0C

#define MAX31790_FAN_FAILED_SEQ_FFQ_1       0x00
#define MAX31790_FAN_FAILED_SEQ_FFQ_2       0x01
#define MAX31790_FAN_FAILED_SEQ_FFQ_4       0x02
#define MAX31790_FAN_FAILED_SEQ_FFQ_6       0x04
#define MAX31790_FAN_FAILED_SEQ_FFQ_MASK    0x04

// PWM Freq Bits // 
#define MAX31790_PWM_FREQ_25                0x00
#define MAX31790_PWM_FREQ_30                0x01
#define MAX31790_PWM_FREQ_35                0x02
#define MAX31790_PWM_FREQ_100               0x03
#define MAX31790_PWM_FREQ_125               0x04
#define MAX31790_PWM_FREQ_149_7             0x05
#define MAX31790_PWM_FREQ_1_25K             0x06
#define MAX31790_PWM_FREQ_1_47K             0x07
#define MAX31790_PWM_FREQ_3_57K             0x08
#define MAX31790_PWM_FREQ_5K                0x09
#define MAX31790_PWM_FREQ_12_5K             0x10
#define MAX31790_PWM_FREQ_25K               0x11

/* MAX31790 Register -------------------------------- */
// Core Bits //
#define MAX31790_REG_TACH_COUNT(C)	        (0x18 + ((C) * 2))
#define MAX31790_REG_PWM_DUTY(C)	        (0x30 + ((C) * 2))

// Config Bits //
#define MAX31790_REG_GLOBAL_CONFIG	        0x00
#define MAX31790_REG_FAN_CONFIG(C)	        (0x02 + (C))
#define MAX31790_REG_FAN_DYNAMIC(C)	        (0x08 + (C))
#define MAX31790_REG_SEQ_START_CONFIG       0x14
#define MAX31790_REG_FAILED_FAN_CONFIG      0x14
#define MAX31790_REG_TARGET_DUTY(C)         (0x40 + ((C) * 2))
#define MAX31790_REG_TARGET_COUNT(C)	    (0x50 + ((C) * 2))
#define MAX31790_REG_WINDOW(C)              (0x60 + (C))

// Status Bits //
#define MAX31790_REG_FREQ_START  	        0x01
#define MAX31790_REG_FAN_FAULT_STATUS_2	    0x10
#define MAX31790_REG_FAN_FAULT_STATUS_1	    0x11
#define MAX31790_REG_FAN_FAULT_MASK_2	    0x12
#define MAX31790_REG_FAN_FAULT_MASK_1	    0x13

// User Bits //
#define MAX31790_REG_USER_BYTE_0            0xE
#define MAX31790_REG_USER_BYTE_1            0xF
#define MAX31790_REG_USER_BYTE_2            0x15
#define MAX31790_REG_USER_BYTE_3            0x16
#define MAX31790_REG_USER_BYTE_4            0x17
#define MAX31790_REG_USER_BYTE_5            0x4C
#define MAX31790_REG_USER_BYTE_6            0x4D
#define MAX31790_REG_USER_BYTE_7            0x4E
#define MAX31790_REG_USER_BYTE_8            0x4F
#define MAX31790_REG_USER_BYTE_9            0x5C
#define MAX31790_REG_USER_BYTE_10           0x5D
#define MAX31790_REG_USER_BYTE_11           0x5E
#define MAX31790_REG_USER_BYTE_12           0x5F
#define MAX31790_REG_USER_BYTE_13           0x66
#define MAX31790_REG_USER_BYTE_14           0x67

#define NUM_CHANNEL         6
#define NUM_TACH_CHANNEL    12
#define RPM_MIN			    120
#define RPM_MAX			    7864320

#define CHCK_TACH_CHAN(C)           (((C) < NUM_TACH_CHANNEL) ? 1 : 0)
#define CHCK_CHAN(C)                (((C) < NUM_CHANNEL) ? 1 : 0)
#define CALC_RPM_OR_BIT(X,SR,NP)    ((60 * (SR) * 8192)/((X) * (NP)))
#define FAN_TO_CHAN(F)              (((F) > 5) ? (F - 6) : F)

typedef struct
{
   uint8_t global_cfg = 0x00;
   uint8_t fan_failed_seq_start_cfg = 0x45;
   uint8_t fan_cfg[NUM_CHANNEL] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   uint8_t fan_dyn[NUM_CHANNEL] = {0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C,};
   uint8_t fan_hallcount[NUM_TACH_CHANNEL] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
   uint8_t fault_mask_1 = 0x3f;
   uint8_t fault_mask_2 = 0x3f;
} max31790_master_config_t;

class MAX31790
{
    public:
        MAX31790(uint8_t adr = 0x20);

        void begin(TwoWire &inWire = Wire);     

        /* Set ------------------------------------------------------------------------------------ */

        void setMasterConfig(max31790_master_config_t *cfg);

        void setTargetDutyBits(uint8_t channel, uint16_t duty);

        void setTargetDuty(uint8_t channel, uint8_t duty);
        
        void setTargetTach(uint8_t channel, uint32_t RPM); //When changing from PWM mode to RPM mode, best results are obtained by loading this register with the current TACH count before changing to RPM mode.
        
        void setFaultMask(uint8_t fan_number);

        void inline setWindow(uint8_t cfg, uint8_t channel) {
            if(CHCK_CHAN(channel))
                write(MAX31790_REG_WINDOW(channel), cfg);            
        };

        void inline setFailedFanSeqStart(uint8_t cfg) {
            write(MAX31790_REG_SEQ_START_CONFIG, cfg);
        };

        void inline setGlobalConfig(uint8_t cfg) {
            write(MAX31790_REG_GLOBAL_CONFIG, cfg);
        };
        
        void inline setPWMFreq(uint8_t group, uint8_t bit_freq) {
            write(MAX31790_REG_FREQ_START, (uint8_t)(group ? (bit_freq << 4) : bit_freq));
        };

        void inline setFanConfig(uint8_t cfg, uint8_t channel) {
            if(CHCK_CHAN(channel))
                write(MAX31790_REG_FAN_CONFIG(channel), cfg);
        };

        void inline setFanDynamic(uint8_t cfg, uint8_t channel) {
            if(CHCK_CHAN(channel))
                write(MAX31790_REG_FAN_DYNAMIC(channel), cfg);
        };

        /* Get ------------------------------------------------------------------------------------ */
        uint8_t getDuty(uint8_t channel, bool isTarget = false);

        uint32_t getRPM(uint8_t fan_number, bool isTarget = false);

        uint8_t inline getGlobalConfig() {
           return read(MAX31790_REG_GLOBAL_CONFIG);
        };

        uint8_t getTargetDuty(uint8_t channel) {
            return getDuty(channel, true);    
        };

        uint32_t getTargetTach(uint8_t channel){
            return getRPM(channel, true);
        };

        uint8_t inline getPWMFreq() {
            return read(MAX31790_REG_FREQ_START);
        };

        uint8_t inline getFailedFanSeqStart() {
            return read(MAX31790_REG_SEQ_START_CONFIG);
        };

        uint8_t inline getFanConfig(uint8_t channel) {
            if(CHCK_CHAN(channel))
                return read(MAX31790_REG_FAN_CONFIG(channel));
            else
                return 0xFF;
        };

        uint8_t inline getFanDynamic(uint8_t channel) {
            if(CHCK_CHAN(channel))
                return read(MAX31790_REG_FAN_DYNAMIC(channel));
            else
                return 0xFF;
        };

        uint8_t inline getFaultMask(uint8_t num)
        { 
            if(CHCK_TACH_CHAN(num))
                return ((num < 5) ? read(MAX31790_REG_FAN_FAULT_MASK_1) : read(MAX31790_REG_FAN_FAULT_MASK_2));
            else
                return 0xFF;           
        };

        uint8_t inline getFaultStatus(uint8_t num)
        { 
            if(CHCK_TACH_CHAN(num))
                return ((num < 5) ? read(MAX31790_REG_FAN_FAULT_STATUS_1) : read(MAX31790_REG_FAN_FAULT_STATUS_2));
            else
                return 0xFF;           
        };

        uint8_t inline getWindow(uint8_t cfg, uint8_t channel) {
            if(CHCK_CHAN(channel))
               return read(MAX31790_REG_WINDOW(channel));
            else
                return 0xFF;          
        };

    protected:
    private:
        TwoWire *myWire;
        
         uint8_t _adr;
        uint8_t sr_map[6] = {1, 2, 4, 8, 16, 32};

        max31790_master_config_t max31790_config;  

        uint16_t read(uint8_t r_adr, uint8_t ljst = 0);
        void write(uint8_t r_adr, uint16_t payload, uint8_t ljst = 0);      
};
#endif