#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <System/System.h>
#include <System/Scheduler.h>
#include <iostream>

#include "canlight.h"
#include "adc.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "eCarCanSettings.h"

#include "MKE18F16.h"

 
using namespace BSP;

/*number of clock ticks per trigger of tick handlet*/
#define SYSTICK 60000

/*scheduler setup*/
using scheduler = System::PeriodicScheduler<System::TaskPeriodic, 5>;

static scheduler a;


/*collects data at specfied pin and sends it out with specfied id*/
void collectAndTx(ADC_Type *base, uint32_t ch, int id){

   /*input tests*/
   //assert(base>=0);

   /*get adc*/
   BSP::adc::ADC& st_adc = adc::ADC::StaticClass();

   /*put data in frame*/
   uint32_t pin_data = st_adc.read(base, ch);
   BSP::can::CANlight::frame f;

   uint32_t pin_data6 = st_adc.read(ADC0, 6);
   uint32_t pin_data7 = st_adc.read(ADC0, 7);
   uint32_t pin_data8 = st_adc.read(ADC0, 8);
   uint32_t pin_data9 = st_adc.read(ADC0, 9);
   uint32_t pin_data10 = st_adc.read(ADC0, 10);
   uint32_t pin_data11 = st_adc.read(ADC0, 11);
   uint32_t pin_data12 = st_adc.read(ADC0, 12);
   uint32_t pin_data13 = st_adc.read(ADC0, 13);
   uint32_t pin_data14 = st_adc.read(ADC0, 14);
   uint32_t pin_data15 = st_adc.read(ADC0, 15);

   //moving data to the frame
   f.id = id;
   printf("%p", f.data);
   memcpy(((f.data)), (&pin_data), 1);
   uint32_t pin_data_shifted = pin_data>>8;
   memcpy(((f.data)+1), (&pin_data_shifted), 1);

   /*write tx out*/
   BSP::can::CANlight& can = can::CANlight::StaticClass();

   can.tx(0, f);

   //reading in test
   BSP::can::CANlight::frame f2 = can.readrx(1);
   uint8_t pin_data3a = (f2.data)[0];
   uint8_t pin_data4a = (f2.data)[1];
   uint8_t pin_data5a = (f2.data)[2];
   uint8_t pin_data6a = (f2.data)[3];
   uint8_t pin_data7a = (f2.data)[4];
   

}

/*ISR function for Systick*/
extern "C" {
void SysTick_Handler(void) {
	a.tick();
}
}

/*Tasks for each sesnor*/
//Pin 0
static void Task0() {
   collectAndTx(ADC_BASE0, ADC_CH0, CAN_ID0);
}

//Pin 1
static void Task1() {
   collectAndTx(ADC_BASE1, ADC_CH1, CAN_ID1);
}

//Pin 2
static void Task2() {
   collectAndTx(ADC_BASE2, ADC_CH2, CAN_ID2);
}

//Pin 3
static void Task3() {
   collectAndTx(ADC_BASE3, ADC_CH3, CAN_ID3);
}

//Pin 4
static void Task4() {
   collectAndTx(ADC_BASE4, ADC_CH4, CAN_ID4);
}

//Pin 5
static void Task5() {
   collectAndTx(ADC_BASE5, ADC_CH5, CAN_ID5);
}

//Pin 6
static void Task6() {
   collectAndTx(ADC_BASE6, ADC_CH6, CAN_ID6);
}

//Pin 7
static void Task7() {
   collectAndTx(ADC_BASE7, ADC_CH7, CAN_ID7);
}

/*main loop*/
int main(){
   BOARD_InitBootPins();
   BOARD_InitBootClocks();

   /*create canlight_config*/
   BSP::can::canlight_config cl_config;
   
   /*CANlight::CANlight(cl_config)*/

   /*create canxconfig*/
   BSP::can::CANlight::canx_config cx_config;
   BSP::can::CANlight::ConstructStatic(&cl_config);
   BSP::can::CANlight& can = can::CANlight::StaticClass();
   cx_config.baudRate = 500000;

   /*init for bus 0*/
   can.init(0, &cx_config);
   can.init(1, &cx_config);

   // if(BSP::adc::ADC::calibrate(ADC0) != kStatus_Success){
   //    perror(NULL);
   // }

   /*adc setup*/   
   BSP::adc::ADC::ConstructStatic(NULL);


   /*initalize interupt that adds to counter*/
   SysTick_Config(SYSTICK);

   /*Adding all sensor tasks --- adress of func -- id of test -- number of ticks betwween executions
   --- deadline */
   if(ADC_ON0 == 1){
      a.addTask(System::TaskPeriodic(Task0, 1, ADC_TBE0, ADC_DEAD0));
   }
   if(ADC_ON1 == 1){
      a.addTask(System::TaskPeriodic(Task1, 2, ADC_TBE1, ADC_DEAD1));
   }
   if(ADC_ON2 == 1){
      a.addTask(System::TaskPeriodic(Task2, 3, ADC_TBE2, ADC_DEAD2));
   }
   if(ADC_ON3 == 1){
      a.addTask(System::TaskPeriodic(Task3, 4, ADC_TBE3, ADC_DEAD3));
   }
   if(ADC_ON4 == 1){
      a.addTask(System::TaskPeriodic(Task4, 5, ADC_TBE4, ADC_DEAD4));
   }
   if(ADC_ON5 == 1){
      a.addTask(System::TaskPeriodic(Task5, 6, ADC_TBE5, ADC_DEAD5));
   }
   if(ADC_ON6 == 1){
      a.addTask(System::TaskPeriodic(Task6, 7, ADC_TBE6, ADC_DEAD6));
   }
   if(ADC_ON7 == 1){
      a.addTask(System::TaskPeriodic(Task7, 8, ADC_TBE7, ADC_DEAD7));
   }

   a.start();

   /*main loop*/
   while(1){
      a.onFrame();
   }
}


/*
fix freq/tasks in genral
get data from pins/pakege them
fix baud rate
fix sys tick
fix bus 0 name


how to test
pins/ad
taskperiod io

10 mili hearts
*/