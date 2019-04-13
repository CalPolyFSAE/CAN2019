#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <System/System.h>
#include <System/Scheduler.h>

#include "canlight.h"
#include "adc.h"
#include "pin_mux.h"
#include "clock_config.h"

using namespace BSP;

/*number of clock ticks per trigger of tick handlet*/
#define SYSTICK 60000

/*scheduler setup*/
using scheduler = System::PeriodicScheduler<System::TaskPeriodic, 5>;

static scheduler a;


/*collects data at specfied pin and sends it out with specfied id*/
void collectAndTx(ADC_Type *base, uint32_t ch, int id){

   /*input tests*/
   assert(base>=0);

   /*get adc*/
   BSP::adc::ADC& st_adc = adc::ADC::StaticClass();

   /*put data in frame*/
   uint32_t pin_data = st_adc.read(base,ch);
   BSP::can::CANlight::frame f;

   f.id = id;
   memcpy(((f.data)), (&pin_data), 1);
   memcpy(((f.data)+1), (&pin_data)+1, 1);
   memcpy(((f.data)+2), (&pin_data)+2, 1);

   /*write tx out*/
   BSP::can::CANlight& can = can::CANlight::StaticClass();

   can.tx(0, f);

   BSP::can::CANlight::frame f2 = can.readrx(1);
   uint8_t pin_data3 = (f2.data)[0];
   uint8_t pin_data4 = (f2.data)[1];
   uint8_t pin_data5 = (f2.data)[2];
   uint8_t pin_data6 = (f2.data)[3];
   uint8_t pin_data7 = (f2.data)[4];
   uint8_t pin_data8 = f2.data[5];
   uint8_t pin_data9 = f2.data[6];
   uint8_t pin_data10 = f2.data[7];

}

/*ISR function for Systick*/
extern "C" {
void SysTick_Handler(void) {
	a.tick();
}
}

/*Tasks for each sesnor*/
/*Pin 0*/
static void Task0() {
   collectAndTx(ADC2,13, 666);
}

/*Pin 1
static void Task1() {
   collectAndTx(AD..,channel, 666);
}

in 2
static void Task2() {
   collectAndTx(AD.., channel, 666);
}*/

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

   /*adc setup*/   
   BSP::adc::ADC::ConstructStatic(NULL);

   /*initalize interupt that adds to counter*/
   SysTick_Config(SYSTICK);

   /*Adding all sensor tasks --- adress of func -- id of test -- number of ticks betwween executions
   --- deadline */
   a.addTask(System::TaskPeriodic(Task0, 1, 4, 100));
   //a.addTask(System::TaskPeriodic(Task1, 2, 4, 20));
   //a.addTask(System::TaskPeriodic(Task2, 3, 4, 20));

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