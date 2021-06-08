#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
Timer t;
PwmOut pin5(D5), pin6(D6);
DigitalInOut ping(D11);
BufferedSerial xbee(D1, D0);
// BufferedSerial uart(D1, D0); //tx,rx
BufferedSerial pc(USBTX, USBRX);

BBCar car(pin5, pin6, servo_ticker);

void openmv_detect(int state){
    float val;
    // while(1){
    ping.output();
    ping = 0; wait_us(200);
    ping = 1; wait_us(5);
    ping = 0; wait_us(5);

    ping.input();
    while(ping.read() == 0);
    t.start();
    while(ping.read() == 1);
    val = t.read();
    if(!state)
        printf("Distance before carlibration = %lf\r\n", val*17700.4f);
    else
        printf("Distance after carlibration = %lf\r\n", val*17700.4f);
    t.stop();
    t.reset();

    ThisThread::sleep_for(1s);
    // }
    
}

int main() {
    // uart.set_baud(9600);
    // xbee.set_baud(9600);
    // pc.set_baud(9600);

    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-16.582, -16.263, -15.227, -11.879, -5.820, 0.000, 6.059, 12.038, 15.147, 16.263, 16.742};

    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    // openmv_t.start(callback(&openmv_queue, &EventQueue::dispatch_forever));
    // openmv_queue.call(&openmv_detect);

    // make RPC call
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&xbee, "r");
    FILE *devout = fdopen(&xbee, "w");
    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                // printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        openmv_detect(0);
        RPC::call(buf, outbuf);
        openmv_detect(1);
        // printf("%s\r\n", outbuf);
    }

}