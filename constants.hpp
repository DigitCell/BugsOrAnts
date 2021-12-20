#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include "math.h"
#define c_window_width 1900
#define c_window_height 900

#define c_world_width 1400
#define c_world_height 800

#define c_ground_level 50

const int numberOfBugs =11;

//BugFactory
const float WING_CHANCE = 0.6;
const float WING_LENGTH_MIN = 2;
const float WING_LENGTH_MAX = 4;
const float WING_SIZE_MIN = 70;
const float WING_ROOT_THRESHOLD = 2;
const float WING_SCALE = 1;
const int LENGTH_MIN = 2;
const int LENGTH_MAX = 12;
const float INTERLEAVE_LEGS_THRESHOLD = 3;
const float INTERLEAVE_LEGS_CHANCE = 0.7;


//Bug
const float VISIBILITY_RADIUS = 500;
const float SPAWN_RADIUS = 400;
const float NOISE_SCALE_MIN = 0.003;
const float NOISE_SCALE_MAX = 0.01;
const float SPEED_SCALE_MIN = 0.1;
const float SPEED_SCALE_MAX = 0.6;
const float SEGMENT_OVERLAP = 0.4;
const float SPEED_MAX_MIN = 100;
const float SPEED_MAX_MAX = 180;
const float EYE_SCALE_MIN = 0.2;
const float EYE_SCALE_MAX = 1.8;
const float EYE_DEVIANCE_MIN = M_PI * 0.8;
const float EYE_DEVIANCE_MAX = M_PI * 1.5;
const float EYE_BLINK_DELAY_MIN = 1;
const float EYE_BLINK_DELAY_MAX = 4;
const float EYE_BLINK_DURATION = 0.1;
const float EYE_PUPIL_RATIO = 2 / 5;
const float EYE_RADIUS_MIN = 5;
const float EYE_RADIUS_FACTOR_MAX = 0.8;


//BodyShape

const int WIDTH_MIN = 7;
const int WIDTH_MAX = 12;
const float THICKNESS_MULTIPLIER_MIN = 0.3;
const float BodyShapeLENGTH_MIN = 18;
const float BodyShapeLENGTH_MAX = 30;
const float LENGTH_SEGMENT = 6;
const float LEG_ANGLE_MIN = 0.3;
const float LEG_ANGLE_MAX = 0.9;
const float SINE_WAVE_PORTION = 0.9;
const float LEG_SCALE = 1.5;
const float LEG_LENGTH_MIN = 18;
const float LEG_FACTOR_MIN = 0.5;
const float LEG_FACTOR_MAX = 1;



#endif // CONSTANTS_HPP
