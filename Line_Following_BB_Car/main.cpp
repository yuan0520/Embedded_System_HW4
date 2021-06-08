#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
// BufferedSerial uart(D1, D0); //tx,rx
// BufferedSerial pc(USBTX, USBRX);

BBCar car(pin5, pin6, servo_ticker);

int main() {

    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};

    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    // uart.set_baud(9600);
    // xbee.set_baud(9600);

    // int count = 0;

    // while(1){
    //     if(uart.readable()){
    //         char recv_uart[1];
    //         uart.read(recv_uart, sizeof(recv_uart));
    //         xbee.write(recv_uart, sizeof(recv_uart));
    //         printf("%c", recv_uart[0]);  // print the data of detected line to screen  
    //         count++;
    //         if(recv_uart[0] == '\n' && count > 50){
    //             printf("Finish collecting data");
    //             xbee.write("Finish collecting data\n", 24);
    //             break; // if we receive a whole line, break and go calling RPC
    //         }    
    //     }
    // }

    // make RPC call
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
        // printf("%s\r\n", outbuf);
    }

}