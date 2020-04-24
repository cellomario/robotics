#include "ros/ros.h"
#include "lla2enu/ComputeDistance.h" //include [package]/[servicename].h
#include "nav_msgs/Odometry.h"
#include <math.h>  
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/sync_policies/exact_time.h>




class computer{
	private:
	ros::NodeHandle n;
	ros::ServiceServer service= n.advertiseService("compute_distance", distance);

	public:
	computer(){
	message_filters::Subscriber<nav_msgs::Odometry> sub1(n, "/car/Odom", 1);
	message_filters::Subscriber<nav_msgs::Odometry> sub2(n, "/obs/Odom", 1);
	typedef message_filters::sync_policies::ApproximateTime<nav_msgs::Odometry, nav_msgs::Odometry> MySyncPolicy;
	message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), sub1, sub2);
	sync.registerCallback(boost::bind(&computer::SynchroCallback, _1, _2)); //register callback


		}
		
	void SynchroCallback(const nav_msgs::Odometry msg1, const nav_msgs::Odometry msg2) // alle good practiveci penseremo dopo
	{ //callback just debug
	}


	static bool distance(lla2enu::ComputeDistance::Request &req,lla2enu::ComputeDistance::Response &res)// one for response (otuptu) under form of pointers
	{
	  
	  return true; //return true because when we call it from other code, need a way if service has performed correctly or some bad stuff happened.
	} //return false=discard response as not meaningful


};

int main(int argc, char **argv) //main for this node similar to pub-sub
{ //standard node initializaiton
  ros::init(argc, argv, "compute_distance_server");
	
  ROS_INFO("Ready to add two ints."); //usual stuff
  computer MyComputer;
  ros::spin();

  return 0;
}
