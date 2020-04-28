#include "ros/ros.h"
#include "std_msgs/String.h"
#include "lla2enu/MyMessage.h" 
#include "lla2enu/ComputeDistance.h" // include service

#include <dynamic_reconfigure/server.h> //standard libraries for dynamic reconfiguration
#include <lla2enu/parametriConfig.h>

#include <sstream>

float soglia_safe;
float soglia_unsafe;

void callback(lla2enu::parametriConfig &config, uint32_t level) { //how do we define our callback?
	soglia_safe=config.soglia_safe;
	soglia_unsafe=config.soglia_unsafe;
}






int main(int argc, char **argv)
{
	float dist;
	ros::init(argc, argv, "talker");
	ros::NodeHandle n;

	ros::Publisher chatter_pub = n.advertise<lla2enu::MyMessage>("chatter", 1000);

	ros::Rate loop_rate(100);
	lla2enu::MyMessage msg;
	ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance");

	dynamic_reconfigure::Server<lla2enu::parametriConfig> server; //create reconfiguration server
  //then, create recoinfiguration callback
	dynamic_reconfigure::Server<lla2enu::parametriConfig>::CallbackType f;

	f = boost::bind(&callback, _1, _2); //we bind it
	server.setCallback(f); //we assign to server the callback we created ???

	while(ros::ok())
	{
		//ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance");
		lla2enu::ComputeDistance srv;

		ROS_INFO("Sto per chiamare il servizio");
		if (client.call(srv)) // call the service
		{
			msg.dist = srv.response.dist;

			if (srv.response.dist > soglia_safe)
			{
				msg.flag.data = "Safe";
			}
			else if (srv.response.dist < soglia_unsafe)
			{
				msg.flag.data = "Crash";
			}
			else if (srv.response.dist > 0)
			{
				msg.flag.data = "Unsafe";
			} 
			else
			{
				msg.flag.data = "Error";
			}
		
		chatter_pub.publish (msg);
		ROS_INFO("La distanza e': %f, lo stato e' %s", (float)msg.dist, msg.flag.data.c_str());
              
		}
		else
		{ 
			ROS_INFO("Failed to call service compute_distance");
		}
	}
	
	return 0;
}