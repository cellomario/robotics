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
float x1, x2, ycar, y2, z1, z2;
bool ok;
		
  void SynchroCallback(const nav_msgs::OdometryConstPtr& messaggio1, const nav_msgs::OdometryConstPtr& messaggio2) 
	{ ROS_INFO ("siamo nella callback\n");
        x1=messaggio1->pose.pose.position.x;
        x2=messaggio2->pose.pose.position.x;
        ycar=messaggio1->pose.pose.position.y;
        y2=messaggio2->pose.pose.position.y;
        z1=messaggio1->pose.pose.position.z;
        z2=messaggio2->pose.pose.position.z;
	
        ok = true;
	
	}
	
	bool distance(lla2enu::ComputeDistance::Request &req,lla2enu::ComputeDistance::Response &res)// one for response (otuptu) under form of pointers
	{ float distanza=sqrt(pow((x1-x2),2)+pow((ycar-y2),2)+pow((z1-z2),2));
	ROS_INFO("ho calcolato la distanza");
	res.dist=distanza;
	return true;
	}


int main(int argc, char **argv) 
{

  ok = false;    
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


  
//  ros::init(argc, argv, "compute_distance_server");
	

  
 ros::spin();

 return 0;
}
