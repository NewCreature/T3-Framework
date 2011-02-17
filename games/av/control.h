#ifndef CONTROL_H
#define CONTROL_H

#include "includes.h"

#define REPEAT_DELAY 20
#define REPEAT_RATE   8
#define DROP_SPEED    4

#define CONTROL_SOURCE_KEY  0 /* uses keyboard */
#define CONTROL_SOURCE_JOY1 1 /* uses joystick 1 */
#define CONTROL_SOURCE_JOY2 2 /* uses joystick 2 */
#define CONTROL_SOURCE_JOY3 3 /* uses joystick 3 */
#define CONTROL_SOURCE_JOY4 4 /* uses joystick 4 */

void read_controller(CONTROLLER * cp, int keys[8]);

#endif
