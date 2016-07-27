#include <ros/ros.h>/*{{{*/
#include <unistd.h>
#include <sound_play/SoundRequest.h>
#include <std_msgs/Int32.h>
#include "kbhit.h"

#define LOOPRATE 10/*}}}*/

bool flag_ring = false;/*{{{*/
char start[] = "../ros_catkin_ws/src/typing/sounds/start.wav";
char maru[]  = "../ros_catkin_ws/src/typing/sounds/maru.wav";
char batu[]  = "../ros_catkin_ws/src/typing/sounds/batu.wav";
char ringfile[128];/*}}}*/

void ringCallback(const std_msgs::Int32 &msg)/*{{{*/
{
	flag_ring = true;
	if      (msg.data == 0) strcpy(ringfile,start);
	else if (msg.data == 1) strcpy(ringfile,maru);
	else if (msg.data == 2) strcpy(ringfile,batu);
}/*}}}*/

int main(int argc, char** argv)/*{{{*/
{
	ros::init(argc, argv, "say");
	ros::NodeHandle nh;

	ros::Publisher say_pub = nh.advertise<sound_play::SoundRequest>("robotsound", 1);
	ros::Subscriber ring_sub = nh.subscribe("ring", 1, ringCallback);

	sound_play::SoundRequest say_msg;
	say_msg.sound = -2;	
	say_msg.command = 1;
	say_msg.volume = 1.0;
	say_msg.arg = "";
	say_msg.arg2 = "";

	ros::Rate loop_rate(LOOPRATE);
	while (ros::ok()) {
		if (flag_ring) {
			say_msg.arg = ringfile;
			say_pub.publish(say_msg);
			flag_ring = false;
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}/*}}}*/
