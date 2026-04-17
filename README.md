### Description
The purpose of this program is to verify that communication with the RELbot hardware is realized correctly and that the loop controller was generated correctly.

### Inputs
Messages passed through RosXenoBridge over topic '/Ros2Xeno'
    Type: Ros2Xeno
    Combined float message. Consists of 'left_wheel_vel' and 'left_wheel_vel'

Encoder readings sent from FPGA

### Outputs
Encoder readings into the console

Wheel velocity commands sent to FPGA

### Run
In a terminal run the following commands
'sudo ./build/FRT_test/FRT_test'
'ros2 run ros_xeno_bridge RosXenoBridge'

### Core components
'run()': reads change in encoders reading in each iteration and prints it. Sends motor commands to the FPGA