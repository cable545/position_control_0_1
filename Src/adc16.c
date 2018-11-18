/*
 * File:		a16dc.c
 * Purpose:		Simple Driver or API for AdC16/PGA
 * This file contains the following two functions:
 *  1. ADC_Cal: calibrates the ADC
 *  2. ADC_Config_Alt: Simply configures an ADC per a structure
 *
 * Typical usage: Fill the structure with the ADC and PGA register contents 
 * needed for the ADC usage.

 * a) Call the ADC_Config_Alt function to configure an ADC, (ADC0 or ADC1)
 * b) Call the ADC_Cal function to calibrate that ADC
 * c) Call the ADC_Config_Alt function again to restore desired configuation
 *    after a calibration
 *
 *
 */

#include "common.h"
#include "adc16.h"


uint16 adc_isrv_result;

/******************************************************************************
Function 1. Name	AUTO CAL ROUTINE   

Parameters		ADC module pointer points to adc0 or adc1 register map 
                         base address.
Returns			Zero indicates success.
Notes         		Calibrates the ADC16. Required to meet specifications 
                        after reset and before a conversion is initiated.
******************************************************************************/
uint8 ADC_Cal(ADC_MemMapPtr adcmap)
{
  unsigned short cal_var;
  
  ADC_SC2_REG(adcmap) &=  ~ADC_SC2_ADTRG_MASK ; // Enable Software Conversion Trigger for Calibration Process    - ADC0_SC2 = ADC0_SC2 | ADC_SC2_ADTRGW(0);   
  ADC_SC3_REG(adcmap) &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
  ADC_SC3_REG(adcmap) |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(AVGS_32) );  // Turn averaging ON and set at max value ( 32 )
  
  
  ADC_SC3_REG(adcmap) |= ADC_SC3_CAL_MASK ;      // Start CAL
  while ( (ADC_SC1_REG(adcmap,A) & ADC_SC1_COCO_MASK ) == COCO_NOT ); // Wait calibration end
  	
  if ((ADC_SC3_REG(adcmap)& ADC_SC3_CALF_MASK) == CALF_FAIL )
  {  
   return(1);    // Check for Calibration fail error and return 
  }
  // Calculate plus-side calibration
  cal_var = 0x00;
  
  cal_var =  ADC_CLP0_REG(adcmap); 
  cal_var += ADC_CLP1_REG(adcmap);
  cal_var += ADC_CLP2_REG(adcmap);
  cal_var += ADC_CLP3_REG(adcmap);
  cal_var += ADC_CLP4_REG(adcmap);
  cal_var += ADC_CLPS_REG(adcmap);

  cal_var = cal_var/2;
  cal_var |= 0x8000; // Set MSB

  ADC_PG_REG(adcmap) = ADC_PG_PG(cal_var);
 

  // Calculate minus-side calibration
  cal_var = 0x00;

  cal_var =  ADC_CLM0_REG(adcmap); 
  cal_var += ADC_CLM1_REG(adcmap);
  cal_var += ADC_CLM2_REG(adcmap);
  cal_var += ADC_CLM3_REG(adcmap);
  cal_var += ADC_CLM4_REG(adcmap);
  cal_var += ADC_CLMS_REG(adcmap);

  cal_var = cal_var/2;

  cal_var |= 0x8000; // Set MSB

  ADC_MG_REG(adcmap) = ADC_MG_MG(cal_var); 
  
  ADC_SC3_REG(adcmap) &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */

  return(0);
}




/******************************************************************************
Function 2 Name 	ADC_Config_Alt 
Parameters		the register values to be set in the adc in a structure
Returns			NONE
Notes         		Configures ADC0 or ADC1 depending on adcmap
                        Prior to calling this function populate the structure
                        elements with the desired ADC configuration.
******************************************************************************/


void ADC_Config_Alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr)
{
 ADC_CFG1_REG(adcmap) = ADC_CfgPtr->CONFIG1;
 ADC_CFG2_REG(adcmap) = ADC_CfgPtr->CONFIG2;
 ADC_CV1_REG(adcmap)  = ADC_CfgPtr->COMPARE1; 
 ADC_CV2_REG(adcmap)  = ADC_CfgPtr->COMPARE2;
 ADC_SC2_REG(adcmap)  = ADC_CfgPtr->STATUS2;
 ADC_SC3_REG(adcmap)  = ADC_CfgPtr->STATUS3;
 //ADC_PGA_REG(adcmap)  = ADC_CfgPtr->PGA;  pbd
 ADC_SC1_REG(adcmap,A)= ADC_CfgPtr->STATUS1A;       
 ADC_SC1_REG(adcmap,B)= ADC_CfgPtr->STATUS1B;
}


void ADC_Read_Cal(ADC_MemMapPtr adcmap, tADC_Cal_Blk *blk)
{
  blk->OFS  = ADC_OFS_REG(adcmap);
  blk->PG   = ADC_PG_REG(adcmap); 
  blk->MG   = ADC_MG_REG(adcmap); 
  blk->CLPD = ADC_CLPD_REG(adcmap); 
  blk->CLPS = ADC_CLPS_REG(adcmap); 
  blk->CLP4 = ADC_CLP4_REG(adcmap);
  blk->CLP3 = ADC_CLP3_REG(adcmap); 
  blk->CLP2 = ADC_CLP2_REG(adcmap); 
  blk->CLP1 = ADC_CLP1_REG(adcmap);
  blk->CLP0 = ADC_CLP0_REG(adcmap);
  blk->CLMD = ADC_CLMD_REG(adcmap); 
  blk->CLMS = ADC_CLMS_REG(adcmap); 
  blk->CLM4 = ADC_CLM4_REG(adcmap);
  blk->CLM3 = ADC_CLM3_REG(adcmap); 
  blk->CLM2 = ADC_CLM2_REG(adcmap); 
  blk->CLM1 = ADC_CLM1_REG(adcmap);
  blk->CLM0 = ADC_CLM0_REG(adcmap);
  
}


void init_ADC16(void){
  // ADC configuration variable  keeps configuration parameters, can be used for reconfiguration or use in with different ADCs)
	tADC_Config  Master_Adc_Config;  
	
  // Turn on the ADC0 clock 
	SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );
	
	//data that will be stored at ADCx_CFG1
  Master_Adc_Config.CONFIG1 = (	ADLPC_LOW | 
																ADC_CFG1_ADIV(ADIV_1) | 
																ADLSMP_LONG | 
																ADC_CFG1_MODE(MODE_8) | 
																ADC_CFG1_ADICLK(ADICLK_BUS));
            
  //ADCx_CFG2
	Master_Adc_Config.CONFIG2 =	MUXSEL_ADCA |   //A for FREEDOM
															ADACKEN_DISABLED | 
															ADHSC_HISPEED | 
															ADC_CFG2_ADLSTS(ADLSTS_20) ;
	
	Master_Adc_Config.COMPARE1 = 0x1234u ;                 // can be anything
	Master_Adc_Config.COMPARE2 = 0x5678u ;                 // can be anything
  // since not using
  // compare feature
  Master_Adc_Config.STATUS2 = 	ADTRG_SW | 
																ACFE_DISABLED | 
																ACFGT_GREATER | 
																ACREN_DISABLED | 
																DMAEN_DISABLED | 
																ADC_SC2_REFSEL(REFSEL_EXT);
									
	Master_Adc_Config.STATUS3 =	CAL_OFF | 
															ADCO_SINGLE | 
															!AVGE_ENABLED | 
															ADC_SC3_AVGS(AVGS_4);
	
	Master_Adc_Config.STATUS1A = !AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(31);
  
	// Configure ADC as it will be used, but becuase ADC_SC1_ADCH is 31,
	// the ADC will be inactive.  Channel 31 is just disable function.
	// There really is no channel 31.
	
	ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);  // config ADC
	
	// Calibrate the ADC in the configuration in which it will be used:
	ADC_Cal(ADC0_BASE_PTR);                    // do the calibration
	
	// The structure still has the desired configuration.  So restore it.
	// Why restore it?  The calibration makes some adjustments to the
	// configuration of the ADC.  They are now undone:
	
	// config the ADC again to desired conditions
	ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);
}


/* 
    adc simple read   read a single channel as a configured in 
    Sigle mode, no interruptions
    Note: before run this function make sure init_ADC16 has been executed
*/
uint16 adc16_simple_read(uint8 adc_channel)
{
   ADC0_SC1A = !AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(adc_channel);  // start conversion
   while(!(ADC0_SC1A & ADC_SC1_COCO_MASK)){};                        // wait for conversion complete 
   return ADC0_RA;
   
}

/**/

void adc16_start_conversion(uint8 adc_channel)
{
   ADC0_SC1A = !AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(adc_channel);  // start conversion   
}
                            

void ADC0_isrv(void)
{
  if (ADC0_SC1A & ADC_SC1_COCO_MASK)
  {
    adc_isrv_result = ADC0_RA;
  }

}


