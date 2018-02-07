#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

void chatterCallback(const geometry_msgs::Twist::ConstPtr& twist)
{
	ROS_INFO("Linear vel: [%f],[%f],[%f]", twist->linear.x,twist->linear.y,twist->linear.z);
	ROS_INFO("Angular vel: [%f],[%f],[%f]", twist->angular.x,twist->angular.y,twist->angular.z);
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "agv_keyboard_control_test");
	ros::NodeHandle n;
	ros::Subscriber sub_agv_keyboard_control = n.subscribe("/cmd_vel",1000, chatterCallback);
	ros::spin();
	return 0;
}