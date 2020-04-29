#include "ros/ros.h"
#include "lla2enu/ComputeDistance.h" //include [package]/[servicename].h
#include "nav_msgs/Odometry.h"
#include <math.h>  
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/sync_policies/exact_time.h>

using namespace message_filters;
float x1, x2, ycar, y2, z1, z2, distanza;
bool IsError;
		
void SynchroCallback(const nav_msgs::OdometryConstPtr& messaggio1, const nav_msgs::OdometryConstPtr& messaggio2) 
{ 
	ROS_INFO ("siamo nella callback\n");
	x1=messaggio1->pose.pose.position.x;
	x2=messaggio2->pose.pose.position.x;
	ycar=messaggio1->pose.pose.position.y;
	y2=messaggio2->pose.pose.position.y;
	z1=messaggio1->pose.pose.position.z;
	z2=messaggio2->pose.pose.position.z;
	
	if(messaggio1->twist.twist.linear.x==-1 || messaggio2->twist.twist.linear.x==-1)
		{
			IsError = true;
		}
		else
		{
			 IsError = false;
		}
}

bool distance(lla2enu::ComputeDistance::Request &req,lla2enu::ComputeDistance::Response &res)// one for response (otuptu) under form of pointers
{   
   if(IsError)
    {
        distanza = -1;
    }
    else if(x1==0 && x2==0 && ycar==0 && y2==0 && z1==0 && z2==0)
    {
        distanza = -2;
    }
    else
    {
        distanza=sqrt(pow((x1-x2),2)+pow((ycar-y2),2)+pow((z1-z2),2));
    }
   
 if(distanza == -1)
    {
        ROS_INFO("GPS error detection");   
    }
    else if(distanza == -2)
    {
        ROS_INFO("errore 2");
    }
    else
    {
        ROS_INFO("distanza calcolata correttamente");
    }
 
    res.dist=distanza;

	return true;
}

int main(int argc, char **argv) 
{

	ros::init(argc, argv, "subscriber_sync");
	ros::init(argc, argv, "compute_distance_client");

	ros::NodeHandle n;

	message_filters::Subscriber<nav_msgs::Odometry> sub1(n, "car/Odom", 1);  
	message_filters::Subscriber<nav_msgs::Odometry> sub2(n, "obs/Odom", 1);

	typedef message_filters::sync_policies::ApproximateTime<nav_msgs::Odometry, nav_msgs::Odometry> MySyncPolicy;
	ros::ServiceServer service= n.advertiseService("compute_distance", distance);

	message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), sub1, sub2);
	sync.registerCallback(boost::bind(&SynchroCallback, _1, _2));

	ROS_INFO("cazzo sono %d %d",x1,x2);
  
	// ros::init(argc, argv, "compute_distance_server");
  
	ros::spin();

	return 0;
}
