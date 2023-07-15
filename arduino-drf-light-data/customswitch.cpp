/*
 * Source file for CustomSwitch class
 * Author: Luke Strohbehn
 * Date: 4/12/23
 * 
 * ISRs have to be static, taking no arguments. However, this means the method is no longer tied to any specific 
 * instance and can only access static variables, defeating the purpose of the class. Therefore, we need to write
 * some "glue" routines. These functions interface between an ISR and a specific class instance. This particular
 * class implements this in the `__init__()` function.
 * For more details, see: http://www.gammon.com.au/forum/?id=12983
 * 
 */

#include "customswitch.h"


CustomSwitch* CustomSwitch::instances[2] = {NULL, NULL};

// CustomSwitch constructor
CustomSwitch::CustomSwitch(const uint8_t pin) {
    __init__(pin);
}


void CustomSwitch::__init__(const uint8_t pin) {
    interrupt_flag = false;
    this->pin = pin;
    pinMode(pin, INPUT_PULLUP);

    switch (pin) {
        case 2:
            attachInterrupt(0, switch_pressed_0, RISING);
            instances[0] = this;
            break;
        case 3:
            attachInterrupt(1, switch_pressed_1, RISING);
            instances[1] = this;
            break;
    } // end CustomSwitch
    
}

void CustomSwitch::switch_pressed_0() {
    if (CustomSwitch::instances[0] != NULL) {
        CustomSwitch::instances[0] -> interrupt_handler();
    }
}

void CustomSwitch::switch_pressed_1() {
    if (CustomSwitch::instances[1] != NULL) {
        CustomSwitch::instances[1] -> interrupt_handler();
    }
}

void CustomSwitch::interrupt_handler() {
    // if the flag is not already set, set to true. Debounce handled in `read_button()`
    if (!interrupt_flag) {
        interrupt_flag = true;
    }
}

bool CustomSwitch::read_switch() {
    // If the interrupt was triggered, complete the reading/debounce
    if (interrupt_flag) {
        bool reading = digitalRead(pin); // move to class?
        // If the reading is 1, button is pressed.
        if (!reading) {
            change_pending = true;
            prev_time = millis();
        }
        if (reading && change_pending) {
            if (millis() - prev_time > DEBOUNCE_TIME) {
                interrupt_flag = false;
                change_pending = false;
                return true;         
            }
        }
    }
    return false;
}
