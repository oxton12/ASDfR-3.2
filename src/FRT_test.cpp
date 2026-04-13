#include "FRT_test.hpp"

FRTTest::FRTTest(uint write_decimator_freq, uint monitor_freq)
    : XenoFrt20Sim(write_decimator_freq, monitor_freq, file,
                   &data_to_be_logged),
      file(1, "./xrf2_logging/TEMPLATE",
           "bin"),  // change template to your project name
      controller() {
  printf("%s: Constructing rampio\n", __FUNCTION__);
  // Add variables to logger to be logged, has to be done before you can log
  // data
  logger.addVariable("this_is_a_int", integer);
  logger.addVariable("this_is_a_double", double_);
  logger.addVariable("this_is_a_float", float_);
  logger.addVariable("this_is_a_char", character);
  logger.addVariable("this_is_a_bool", boolean);

  // To infinite run the controller, uncomment line below
  controller.SetFinishTime(0.0);
}

FRTTest::~FRTTest() {}

int FRTTest::initialising() {
  // Set physical and cyber system up for use in a
  // Return 1 to go to initialised state

  evl_printf("Hello from initialising\n");  // Do something

  std::fill(u, u + 4, 0.0);  // Making sure that u contains only zeros

  // The logger has to be initialised at only once
  logger.initialise();
  // The FPGA has to be initialised at least once
  ico_io.init();

  return 1;
}

int FRTTest::initialised() {
  // Keep the physical syste in a state to be used in the run state
  // Call start() or return 1 to go to run state

  evl_printf("Hello from initialised\n");  // Do something

  return 0;
}

int FRTTest::run() {
  // Do what you need to do
  // Return 1 to go to stopping state

  int encLeft = sample_data.channel2;   // current reading of the left encoder
  int encRight = sample_data.channel1;  // current reading of the right encoder

  int leftDiff = encLeft - prevEncLeft;        // difference in left encoder
                                               // reading since last iteration
  int rightDiff = -(encRight - prevEncRight);  // same of the right

  // account for the discontinuity in the reading for the left
  if (leftDiff > encMax / 2) {
    leftDiff = leftDiff - encMax;
  } else if (leftDiff < -1 * encMax / 2) {
    leftDiff = encMax + leftDiff;
  }

  // account for the discontinuity in the reading for the right
  if (rightDiff > encMax / 2) {
    rightDiff = rightDiff - encMax;
  } else if (rightDiff < -1 * encMax / 2) {
    rightDiff = encMax + rightDiff;
  }

  monitor.printf("Left enc count diff : %d, Right enc count diff : %d \n",
                 leftDiff, rightDiff);

  prevEncLeft = encLeft;
  prevEncRight = encRight;

  u[2] = ros_msg.left_wheel_vel;
  u[3] = ros_msg.right_wheel_vel;
  controller.Calculate(u, y);
  if (controller.IsFinished()) {
    actuate_data.pwm1 = 0;
    actuate_data.pwm2 = 0;
    return 1;
  }
  actuate_data.pwm2 = PWMMax * y[0];   // set left motor velocity
  actuate_data.pwm1 = -PWMMax * y[1];  // set right motor velocity

  return 0;
}

int FRTTest::stopping() {
  // Bring the physical system to a stop and set it in a state that the system
  // can be deactivated Return 1 to go to stopped state
  logger.stop();                        // Stop logger
  evl_printf("Hello from stopping\n");  // Do something

  return 1;
}

int FRTTest::stopped() {
  // A steady state in which the system can be deactivated whitout harming the
  // physical system

  monitor.printf("Hello from stopping\n");  // Do something

  return 0;
}

int FRTTest::pausing() {
  // Bring the physical system to a stop as fast as possible without causing
  // harm to the physical system

  evl_printf("Hello from pausing\n");  // Do something
  return 1;
}

int FRTTest::paused() {
  // Keep the physical system in the current physical state

  monitor.printf("Hello from paused\n");  // Do something
  return 0;
}

int FRTTest::error() {
  // Error detected in the system
  // Can go to error if the previous state returns 1 from every other state
  // function but initialising

  monitor.printf("Hello from error\n");  // Do something

  return 0;
}
