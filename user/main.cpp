/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>

#include <MicroNMEA.h>
#include <mcurses.h>

// Check we are using GDB Debugging and include the additional code
#ifdef DEBUG
#include <TeensyDebug.h>
#endif

// Refer to serial devices by use
usb_serial_class& console = SerialUSB;
HardwareSerial& gps = Serial1;

char nmeaBuffer[1024];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
bool ledState = LOW;

extern "C" int main(void)
{

    #ifdef DEBUG
    while (SerialUSB1) {}    // Wait for Debugger connect
    debug.begin(SerialUSB1);  // Start Debug Serial e.g. COM11
    #endif

    console.begin(115200); // console
    gps.begin(9600); // gps

    pinMode(13, OUTPUT);
    digitalWriteFast(13, LOW);
    pinMode(25, OUTPUT);
    digitalWriteFast(25, HIGH);
    pinMode(26, OUTPUT);
    digitalWriteFast(26, HIGH); // Active Low

	while (1) {

        digitalWriteFast(25, LOW);
        while (gps.available()) {
            char c = gps.read();
            //console.print(c);
            nmea.process(c);
        }
        digitalWriteFast(25, HIGH);
        
        // Output GPS information from previous second
        console.print("Valid fix: ");
        console.println(nmea.isValid() ? "yes" : "no");

        console.print("Nav. system: ");
        if (nmea.getNavSystem())
          console.println(nmea.getNavSystem());
        else
          console.println("none");

        console.print("Num. satellites: ");
        console.println(nmea.getNumSatellites());

        console.print("HDOP: ");
        console.println(nmea.getHDOP() / 10., 1);

        console.print("Date/time: ");
        console.print(nmea.getYear());
        console.print('-');
        console.print(int(nmea.getMonth()));
        console.print('-');
        console.print(int(nmea.getDay()));
        console.print('T');
        console.print(int(nmea.getHour()));
        console.print(':');
        console.print(int(nmea.getMinute()));
        console.print(':');
        console.println(int(nmea.getSecond()));

        long latitude_mdeg = nmea.getLatitude();
        long longitude_mdeg = nmea.getLongitude();
        console.print("Latitude (deg): ");
        console.println(latitude_mdeg / 1000000., 6);

        console.print("Longitude (deg): ");
        console.println(longitude_mdeg / 1000000., 6);

        long alt;
        console.print("Altitude (m): ");
        if (nmea.getAltitude(alt))
          console.println(alt / 1000., 3);
        else
          console.println("not available");

        console.print("Speed: ");
        console.println(nmea.getSpeed() / 1000., 3);
        console.print("Course: ");
        console.println(nmea.getCourse() / 1000., 3);

        console.println("-----------------------");
        nmea.clear();

        nmea.isValid() ? digitalWriteFast(26, LOW) : digitalWriteFast(26, HIGH);

	}


}

