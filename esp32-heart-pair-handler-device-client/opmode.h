#ifndef MODE_H
#define MODE_H

#define NORMAL_MODE 0
#define LAMP_MODE 1
#define LAMP_MODE_WITH_BUTTONS 2
#define CONFIGURE_MODE 3
#define PROM_MODE 4
#define DEMO_MODE 5
#define OFF_MODE 6

uint8_t opmode = NORMAL_MODE;
uint8_t previous_opmode = opmode;

#endif
