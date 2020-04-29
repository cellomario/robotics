#include "ros/ros.h"
#include "lla2enu/ComputeDistance.h" 
#include "nav_msgs/Odometry.h"
#include <math.h>  
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

//this node subscribes to the two sets of transformed coordinates, synchronizes the messages through a message filter and
//then publishes the service "compute_distance" that returns the distance between the those two messages, handling
//the situation whenever one GPS loses its connection.

//declare global variables
float x1, x2, ycar, y2, z1, z2, distanza; 
bool IsError; //flag to see if we are in an error condition

//callback that gets called after we manage to synchronize two messages
void SynchroCallback(const nav_msgs::OdometryConstPtr& messaggio1, const nav_msgs::OdometryConstPtr& messaggio2) 
{ 
	//assign to global variables the content of the two messages
	x1=messaggio1->pose.pose.position.x;
	x2=messaggio2->pose.pose.position.x;
	ycar=messaggio1->pose.pose.position.y;
	y2=messaggio2->pose.pose.position.y;
	z1=messaggio1->pose.pose.position.z;
	z2=messaggio2->pose.pose.position.z;

	//we read this field of the message that, since it has no use in our case, we use as a flag to signal that there is some
	//wrong behavior in our system, specifically, that one of the GPS has lost its link
	if(messaggio1->twist.twist.linear.x==-1 || messaggio2->twist.twist.linear.x==-1)
		{
			IsError = true;
		}
		else
		{
			 IsError = false;
		}
}

//callback to be called whenever the service is called
bool distance(lla2enu::ComputeDistance::Request &req,lla2enu::ComputeDistance::Response &res)
{   
   if(IsError) //error code -1 if I have no signal
    {
        distanza = -1;
    }
	//if I still have received no data (as c++ initializes all variables equal to 0), I send a different error code.
    else if(x1==0 && x2==0 && ycar==0 && y2==0 && z1==0 && z2==0) 
    {
        distanza = -2;
    }
    else //compute distance
    {
        distanza=sqrt(pow((x1-x2),2)+pow((ycar-y2),2)+pow((z1-z2),2));
    }
   
    res.dist=distanza; //fill response field with distance.

	return true;
}

int main(int argc, char **argv) 
{
	//usual ROS initialization
	ros::init(argc, argv, "compute_distance_server"); 
	ros::NodeHandle n;
	
	//subscribe the synchronizer to the two topics containig ENU data. 
	message_filters::Subscriber<nav_msgs::Odometry> sub1(n, "car/Odom", 1);  
	message_filters::Subscriber<nav_msgs::Odometry> sub2(n, "obs/Odom", 1);

	//define message filter synchronization policy
	typedef message_filters::sync_policies::ApproximateTime<nav_msgs::Odometry, nav_msgs::Odometry> MySyncPolicy;
	
	//advertise service
	ros::ServiceServer service= n.advertiseService("compute_distance", distance);
	
	//create synchronizer object and assign the callback to it.
	message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), sub1, sub2);
	sync.registerCallback(boost::bind(&SynchroCallback, _1, _2));

	ros::spin();

	return 0;
}