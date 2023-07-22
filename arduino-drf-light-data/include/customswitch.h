/*
 * Header file for LimitSwitch class
 * Author: Luke Strohbehn
 * Date: 4/12/23
 */

#ifndef CUSTOMSWITCH_H
#define CUSTOMSWITCH_H

#include <stdint.h>
#include <Arduino.h>

class CustomSwitch {

    private:
        // Switch instances for selecting correct interrupt
        static CustomSwitch* instances[2];
        // Interrupt-related vars
        volatile bool interrupt_flag;
        volatile const unsigned long DEBOUNCE_TIME {10};

        bool change_pending = false; // static vars to retain old values between function calls
        unsigned long prev_time;

        uint8_t pin;

        // Private class methods
        void __init__(const uint8_t pin);
        void interrupt_handler();
        static void switch_pressed_0();
        static void switch_pressed_1();
        
    public:
        CustomSwitch();
        CustomSwitch(const uint8_t pin);
        bool read_switch();

};

#endif // CUSTOMSWITCH_H
