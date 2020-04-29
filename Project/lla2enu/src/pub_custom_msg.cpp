#include "ros/ros.h"
#include "std_msgs/String.h"
#include "lla2enu/MyMessage.h" 
#include "lla2enu/ComputeDistance.h" 
#include <dynamic_reconfigure/server.h>
#include <lla2enu/parametriConfig.h>

//This node subscribes to the service "compute_distance" and reads the provided distance, then publishes a custom message
//containing the aforementioned distance and a string containing the current status (Safe/Unsafe/Crash) or, if an error occurs,
//the error itself.


double soglia_safe; //soglia tra safe e unsafe
double soglia_unsafe; //soglia tra unsafe e crash

void callback(lla2enu::parametriConfig &config, uint32_t level) { 
	//leggo i parametri e li salvo nelle due variabili globali
	soglia_safe=config.soglia_safe;
	soglia_unsafe=config.soglia_unsafe;
	ROS_INFO("Ho alterato i parametri");
}






int main(int argc, char **argv)
{
	ros::init(argc, argv, "custom_publisher");
	ros::NodeHandle n;
	
	//create publisher for our custom message
	ros::Publisher chatter_pub = n.advertise<lla2enu::MyMessage>("custom_msg", 1000); 
	ros::Rate loop_rate(10);
	
	//create new custom message object
	lla2enu::MyMessage msg; 
	
	//client to subscribe to service that computes distance
	ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance"); 

	//create and configure reconfiguration server and relative callback
	dynamic_reconfigure::Server<lla2enu::parametriConfig> server; 
	dynamic_reconfigure::Server<lla2enu::parametriConfig>::CallbackType f;
	f = boost::bind(&callback, _1, _2);
	server.setCallback(f); 

	while(ros::ok())
	{	
		//get distance from service
		lla2enu::ComputeDistance srv; 
		
		//call the service and read exit core
		if (client.call(srv)) 
		{	
			//put distance into message object, ready for pubblication
			msg.dist = srv.response.dist; 
			
			//"if" block to define the state of the system in the string of the custom message
			if (srv.response.dist > soglia_safe) 
			{
				msg.flag.data = "Safe";
			}
			else if (srv.response.dist > soglia_unsafe)
			{
				msg.flag.data = "Unsafe";
			}
			else if (srv.response.dist > 0)
			{
				msg.flag.data = "Crash";
			} 
			else if (srv.response.dist == -1.0) //our custom error code to define 
			{
				msg.flag.data = "Gps Lost Satellite";
			}
			else
			{
				msg.flag.data = "Insufficient Data";
			}
		
		//publish the message
		chatter_pub.publish (msg); 
		
		//publish in console current distance and status
		ROS_INFO("La distanza e': %f, lo stato e' %s", (float)msg.dist, msg.flag.data.c_str()); 
		}
		else
		{ 
			ROS_INFO("Failed to call service compute_distance");
		}  
		ros::spinOnce();
	}

	ros::spin();
	return 0;
}