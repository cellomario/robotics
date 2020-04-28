#include "ros/ros.h"
#include "lla2enu/ComputeDistance.h" // include service
// this is the client that will call the service.

int main(int argc, char **argv) // we sum two integers, passed with arguments
{
	ros::init(argc, argv, "qua voglio vedere se il serviozion funziona");

	ros::NodeHandle n; // usual nodehandle
	// here we create our service client in the usual way important is to put the name of
	// the service we are going to call and then a name to the client as we can have
	// multiple clients of the same service
	ros::ServiceClient client = n.serviceClient<lla2enu::ComputeDistance>("compute_distance");
	// create a new object of type service to call it
	lla2enu::ComputeDistance srv;
	// set the different fields of the numbers we want to broadcast

	while(ros::ok())
	{
		ROS_INFO("Sto per chiamare il servizio");
		if (client.call(srv)) //call the service
		{
			// service accmplished successfully
			ROS_INFO("Dist: %f", (float)srv.response.dist);
		}
		else
		{ // service not successful
			ROS_INFO("Failed to call service add_two_ints");
		}
		// ros::spin();
	}

	return 0;
}
