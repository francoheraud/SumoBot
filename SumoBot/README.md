# ELEC3020 Project 2025: Sumobot


## Notes to team members (by Franco)
- Location of pre-processor directives (i.e. defines) must be kept to a consistent standard
- Do not redefine arduino + tft headers unnecessarily
- Not good to add multiple definitions of 'loop()' & 'setup', these functions should only be in main.cpp (called once!)
- Stop defining seperate instances of important structs/objects (i.e. TFT_eSPI), define it in just one spot or pass a pointer to the instance in src file functions (parameters) and instantiate it once in main.cpp!

## Other:
- LineDetector needs lots of work, plz work on it so that it integrates well with the entire project!
- Someone plz integrate Allan's resistive divider idea in IR line detector code
- Will add .stl files to chassis directory soon

## MCU PIN OUT (FINAL)
- 1->IN1
- 2->IN2
- 3->IN3
- 10->IN4
- 11->PWM A
- 12->PWM B
- 13->Encoder A
- 43->Ultrasonic 1 Trigger
- 44->Ultrasonic 1 Echo
- 18->Ultrasonic 2 Trigger
- 17->Ultrasonic 2 Echo
- 21->IR Sensors
- 16->Encoder B
