#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
// BufferedSerial pc(USBTX, USBRX);

BBCar car(pin5, pin6, servo_ticker);

int main() {

   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};

   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);


   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
}