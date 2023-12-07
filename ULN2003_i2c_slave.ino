// stm8s103f3

#include <ULN2003.h>
#include <Wire.h>

int const step = 8 * 30; // it should be multiply of 8
int const zeroStep = 8 * 10; // step should be multiply of zeroStep

volatile int8_t stepperConfig_zero = 0; // zero position movement
volatile int8_t stepperConfig_wheel = 0;

int8_t wheelPosition = 0;

void onReceive(int bytes) {
  if (bytes < 2) {
      return;
  }
  int cmd = Wire_read();
  switch (cmd) {
      case 0x01: {
        stepperConfig_zero += (int8_t)Wire_read();
        break;
      }
      case 0x02: {
        stepperConfig_wheel = (int8_t)Wire_read();
        break;
      }
  }
}

INTERRUPT_HANDLER(SPI_IRQHandler, 10) {
}

void setup() {
    Wire_beginSlave(0x12);
    Wire_setTimeout(100);
    Wire_setClock(400000);
    Wire_onReceive(onReceive);

    stepperMotor_init(PC7, PC6, PC5, PC4);
    stepperMotor_setStepDuration(1);
}

void loop() {
    if (wheelPosition != stepperConfig_wheel) {
        if (wheelPosition < stepperConfig_wheel) {
            wheelPosition++;
            stepperMotor_step(-step);
        } else if (wheelPosition > stepperConfig_wheel) {
            wheelPosition--;
            stepperMotor_step(step);
        }
        stepperConfig_zero = 0; // it can save some lives
        return;
    }
    if (wheelPosition == 0) {
        if (stepperConfig_zero < 0) {
            stepperConfig_zero++;
            stepperMotor_step(zeroStep);
        } else if (stepperConfig_zero > 0) {
            stepperConfig_zero--;
            stepperMotor_step(-zeroStep);
        }
    }
}
