#include <iostream>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <cmath>
#include <stdint.h>
#include "lib/serialib.h"

#define SAMPLE_BUFFER_SIZE 1024
#define SERIAL_BUFFER 4
#define SENSITIVITY 15
#define BAUDRATE 115200

// double const MAX_SIGNAL = 1 << 16;
// double const MAX_DB = log10( 1/MAX_SIGNAL) * 20;
using namespace std;

char playback[] = "alsa_output.pci-0000_00_1f.3.analog-stereo";
char device[] = "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
char arduino_serial[] = "/dev/ttyACM0";
pa_simple *s = NULL;
pa_simple *p = NULL;
serialib serial;

int ret = 1;
int error;

void print_level(double value);
void to_levels(double db, uint8_t * buffer);

int16_t abs16(int16_t i){
    if (i < 0) i = -i;
    return i;
}

double average_buffer(int16_t * buffer, int size);

int main() {
    bool const passthrough = false;

    if (serial.openDevice(arduino_serial, BAUDRATE)!=1) {
        printf("Error opening serial port. %s\n", arduino_serial);
        return -1;
    }
    uint8_t serbuf[SERIAL_BUFFER];

    printf("Starting to read samples.\n");

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 2;
    ss.rate = 44100;

    if (!(s = pa_simple_new(NULL, "Read Sample tests", PA_STREAM_RECORD, device, "Read sample monitor", &ss, NULL, NULL, &error))){
        printf("Opening record Device faild.\n");
        return -1;
    }

    if(error != 0) {
        printf("Opening record device falid with code : %d\n", error);
        return -1;
    }

    if(passthrough){
        if (!(p = pa_simple_new(NULL, "Read Sample tests playback", PA_STREAM_PLAYBACK, playback, "Playback", &ss, NULL, NULL, &error))){
            printf("Opening playback Device faild.\n");
            goto finish;
        }

        if(error != 0) {
            printf("Opening playback device falid with code : %d\n", error);
            goto finish;
        }
    }
    
    int16_t buf[SAMPLE_BUFFER_SIZE];

    for(;;){
        pa_simple_read(s, &buf, sizeof(buf), &error);

        if (error != 0) {
            printf("Reading sample falid with code : %d\n", error);
            break;
        }
        for(int i = 0; i < 100; i++){
            serbuf[i] = 0;
        }
        double l = (average_buffer(buf, SAMPLE_BUFFER_SIZE)/INT16_MAX)*100*SENSITIVITY;
        //print_level(l);
        to_levels(l, serbuf);

        //printf("%d, %d, %d, %d\n", serbuf[0], serbuf[1], serbuf[2], serbuf[3]);
        serial.writeBytes(serbuf, SERIAL_BUFFER);
        

        if(passthrough){
            serial.flushReceiver();
            pa_simple_write(p, buf, sizeof(buf), &error);
            if (error != 0) {
                printf("Playing samples falid with code : %d\n", error);
                break;
            }
        }
        
    }
    
    printf("%d\n", sizeof(buf));

finish:
    if(s != NULL) pa_simple_free(s);
    if(p != NULL) pa_simple_free(p);
    serial.closeDevice();
}

double average_buffer(int16_t * buffer, int size) {
    double accume = abs16(*buffer);
    //printf("%lf\n", accume);
    for(int i = 1; i < size; i++){
        accume += abs16(*(buffer + i));
    }
    
    return accume/size;
}

void print_level(double value) {
    int level = (int)value;

    for(int i = 0; i < level; i++){
        printf("#");
    }
    printf("%lf\n", value);
}

void to_levels(double db, uint8_t * buffer){
    double step = 100 / 4.0;
    // printf("%lf\n",step);
    if(db < step) {
        *(buffer) = (db/step) * 255;
    }else if(db > step && db <= step * 2.0){
        *(buffer) = 255;
        *(buffer + 1) = ((db - step)/step) * 255;
    }else if(db > step * 2.0 && db <=  step * 3.0){
        *(buffer) = 255;
        *(buffer + 1) = 255;
        *(buffer + 2) = ((db - (step * 2))/step) * 255;
    }else if(db > step * 3.0){
        *(buffer) = 255;
        *(buffer + 1) = 255;
        *(buffer + 2) = 255;
        *(buffer + 3) = ((db - (step * 2))/step) * 255;
    }
    //*(buffer + 3) = 0;
}