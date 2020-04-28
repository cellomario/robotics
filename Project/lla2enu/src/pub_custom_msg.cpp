#include "ros/ros.h"
#include "std_msgs/String.h"
#include "lla2enu/MyMessage.h" 
#include "lla2enu/ComputeDistance.h" // include service

#include <sstream>


int main(int argc, char **argv)
{
	float dist;
	ros::init(argc, argv, "talker");
	ros::NodeHandle n;

	ros::Publisher chatter_pub = n.advertise<lla2enu::MyMessage>("chatter", 1000);

	ros::Rate loop_rate(100);
	lla2enu::MyMessage msg;
	ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance");

	while(ros::ok())
	{
		//ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance");
		lla2enu::ComputeDistance srv;

		ROS_INFO("Sto per chiamare il servizio");
		if (client.call(srv)) // call the service
		{
			msg.dist = srv.response.dist;

			if (srv.response.dist>5)
			{
				msg.flag.data = "Safe";
			}
			else if (srv.response.dist < 1)
			{
				msg.flag.data = "Crash";
			}
			else 
			{
				msg.flag.data = "Unsafe";
			} 
		
		chatter_pub.publish (msg);
		ROS_INFO("La distanza è: %f, lo stato è %s", (float)msg.dist, msg.flag);
              
		}
		else
		{ 
			ROS_INFO("Failed to call service compute_distance");
		}
	}
	
	return 0;
}