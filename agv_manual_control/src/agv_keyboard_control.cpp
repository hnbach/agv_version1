#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// this function is to check if some key has been pressed
// source: https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}
 

int main(int argc, char **argv)
{
	ros::init(argc, argv, "agv_keyboard_control");
	ros::NodeHandle n;
	ros::Publisher pub_keyboard_control = n.advertise<geometry_msgs::Twist>("/cmd_vel",1000);
	ros::Rate loop_rate(5);
	int inputKey;
	ROS_INFO("\n"
			 "Control your AGV\n"
			 "----------------\n"
			 "      w        \n"
			 "   a  s  d     \n"
			 "      x        \n"
			 "w,x : increase/descrease linear velocity\n"
			 "a,d : increase/descrease angular velocity\n"
			 "s   : force stop\n"
			 "CTRL-C to quit\n");
  // Those limitations depend on the AGV specification
  float max_linear_vel = 3;
  float max_angular_vel = 2;
  // AGV target velocity
  float target_linear_vel = 0;
  float target_angular_vel = 0;
  
	while (ros::ok())
	{
		if (kbhit())
		{
			// a: 97; w: 119; d: 100; s: 115; x: 120
			inputKey = getchar();
      // check the limitation and increase/descrease angular/linear velocity
			if (inputKey == 97)
      {
        if (target_angular_vel <= max_angular_vel)
          // increase angular velocity
          target_angular_vel += 0.1;
        else
          ROS_WARN("Target command out of limit");
      }
			else if (inputKey == 119)
      {
        if (target_linear_vel <= max_linear_vel)
          // increase linear velocity
          target_linear_vel += 0.01;
        else
          ROS_WARN("Target command out of limit");
      }
			else if (inputKey == 100)
      {
        if (target_angular_vel >= -max_angular_vel)
          // descrease angular velocity
          target_angular_vel += -0.1;
        else
          ROS_WARN("Target command out of limit");
      }
			else if (inputKey == 120)
      {
        if (target_linear_vel >= -max_linear_vel)
          // descrease linear velocity
          target_linear_vel += -0.01;
        else
          ROS_WARN("Target command out of limit");
      }
			else if (inputKey == 115)
      {
        // stop, set all to zeros
        target_linear_vel = 0;
        target_angular_vel = 0;
        ROS_WARN("Agv have been stopped");
      }
		}
		ROS_INFO("Linear velocity: [%f] , Angular velocity: [%f]", target_linear_vel, target_angular_vel);
		geometry_msgs::Twist twist;
		twist.linear.x = target_linear_vel;
		twist.linear.y = 0;
		twist.linear.z = 0;
		twist.angular.x = 0;
		twist.angular.y = 0;
		twist.angular.z = target_angular_vel;

		pub_keyboard_control.publish(twist);
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}