#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <memory>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include <tf/transform_broadcaster.h>

#include "ManifoldCAN.h"

void subTest(const geometry_msgs::Twist &twist){
    std::cout << "hello" << std::endl;
}

int main(int argc, char **argv) {
    std::cout << "Hello, World!" << std::endl;

    ros::init(argc, argv, "CANRobot");
    ros::NodeHandle n;

    //n.subscribe("cmd_vel", 1000, &ManifoldCAN::sendTargetVelocityROS, &can);

    std::shared_ptr<ros::Publisher> rollPub = std::make_shared<ros::Publisher>(n.advertise<std_msgs::Float32>("imu/roll", 1000));
    ros::Publisher pitchPub = n.advertise<std_msgs::Float32>("imu/pitch", 1000);
    ros::Publisher yawPub = n.advertise<std_msgs::Float32>("imu/yaw", 1000);

    ros::Publisher xPub = n.advertise<std_msgs::Float32>("odomRaw/x", 1000);
    ros::Publisher yPub = n.advertise<std_msgs::Float32>("odomRaw/y", 1000);
    ros::Publisher odomYawPub = n.advertise<std_msgs::Float32>("odomRaw/yaw", 1000);
    ros::Publisher chassis_odom_pub = n.advertise<nav_msgs::Odometry>("odom",1000);

    ros::Publisher rightBallPub = n.advertise<std_msgs::Float32>("rightBall", 1000);

    Twist2D zeroTwist;
    zeroTwist.vX = 0.0;
    zeroTwist.vY = 0.0;
    zeroTwist.w = 0.0;

    tf::TransformBroadcaster tf_broadcaster;

    ManifoldCAN can("can0", 1000.0, tf_broadcaster, std::make_shared<ros::Publisher>(chassis_odom_pub));
    can.sendTargetVelocity(zeroTwist);

    ros::Rate loopRate(100);
    can.initialize(loopRate);

    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_AHRS, CANMESSAGE_SUBID_AHRS_ROLL), rollPub);
    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_AHRS, CANMESSAGE_SUBID_AHRS_PITCH), std::make_shared<ros::Publisher>(pitchPub));
    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_AHRS, CANMESSAGE_SUBID_AHRS_YAW), std::make_shared<ros::Publisher>(yawPub));

    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_ODOMETRY, CANMESSAGE_SUBID_ODOM_Y), std::make_shared<ros::Publisher>(xPub));
    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_ODOMETRY, CANMESSAGE_SUBID_ODOM_X), yPub);
    can.addRosPublisher(ManifoldCAN::newCanId(0x600, CANMESSAGE_ID_ODOMETRY, CANMESSAGE_SUBID_ODOM_YAW), odomYawPub);

    ros::Subscriber sub = n.subscribe("cmd_vel", 30, &ManifoldCAN::sendTargetVelocityROS, &can);
    ros::Subscriber servoSub = n.subscribe("servo_vel", 30, &ManifoldCAN::sendTargetServoVelocity, &can);

    ros::Subscriber buzzerFreqSub = n.subscribe("buzzer/frequency", 30, &ManifoldCAN::sendBuzzerFrequency, &can);
    ros::Subscriber buzzerDutyCycleSub = n.subscribe("buzzer/dutycycle", 30, &ManifoldCAN::sendBuzzerDutyCycle, &can);

    ros::Subscriber rightMecanisimSub = n.subscribe("rightBall", 30, &ManifoldCAN::sendRightBall, &can);
    ros::Subscriber centerMecanisimSub = n.subscribe("centerBall", 30, &ManifoldCAN::sendCenterBall, &can);
    ros::Subscriber leftMecanisimSub = n.subscribe("leftBall", 30, &ManifoldCAN::sendLeftBall, &can);

    ros::spin();

    //can.sendTargetVelocity(test);

    return 0;
}