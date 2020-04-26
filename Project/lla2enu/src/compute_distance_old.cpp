#include "ros/ros.h"
#include "lla2enu/ComputeDistance.h" //include [package]/[servicename].h
#include "nav_msgs/Odometry.h"
#include <math.h>  
#include "lla2enu/ComputeDistance.h" //include [package]/[servicename].h
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/sync_policies/exact_time.h>


using namespace message_filters;

class computer{
	nav_msgs::Odometry messaggio1;
	nav_msgs::Odometry messaggio2;

private:
	static float x1,x2,y1,y2,z1,z2;

	float dist;
	ros::NodeHandle n;
	ros::ServiceServer service= n.advertiseService("compute_distance", distance);

	public:
	computer(){
	static message_filters::Subscriber<nav_msgs::Odometry> sub1(n, "car/Odom", 1);
	static message_filters::Subscriber<nav_msgs::Odometry> sub2(n, "obs/Odom", 1);
	typedef message_filters::sync_policies::ApproximateTime<nav_msgs::Odometry, nav_msgs::Odometry> MySyncPolicy;
	static message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), sub1, sub2);
	sync.registerCallback(boost::bind(&computer::SynchroCallback, this, _1, _2)); //register callback


		}
		
	void SynchroCallback(const nav_msgs::OdometryConstPtr& msg1, const nav_msgs::OdometryConstPtr& msg2) // alle good practiveci penseremo dopo
	{ //callback just debug
	messaggio1=*msg1;
	messaggio2=*msg2;
	static float x3;
	x1=x3;
	}


	static bool distance(lla2enu::ComputeDistance::Request &req,lla2enu::ComputeDistance::Response &res)// one for response (otuptu) under form of pointers
	{
	nav_msgs::Odometry msg1;
	nav_msgs::Odometry msg2;
	float x4=x1;
	
	/*
	x1=&messaggio1.pose.pose.position.x;
	x2=messaggio2.pose.pose.position.x;
	y1=messaggio1.pose.pose.position.y;
	y2=messaggio2.pose.pose.position.y;
	z1=messaggio1.pose.pose.position.z;
	z2=messaggio2.pose.pose.position.z;
	
	dist=sqrt(pow((x1-x2),2)+pow((y1-y2),2)+pow((z1-z2),2));*/
		return true; //return true because when we call it from other code, need a way if service has performed correctly or some bad stuff happened.
	} //return false=discard response as not meaningful


};

int main(int argc, char **argv) //main for this node similar to pub-sub
{ //standard node initializaiton
  ros::init(argc, argv, "compute_distance_server");
	
  computer MyComputer;
  
  ros::spin();

  return 0;
}
