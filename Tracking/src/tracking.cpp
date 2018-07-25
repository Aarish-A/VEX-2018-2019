#include <cmath>
#include "tracking.hpp"

tracking::tracking(ADIEncoder& left, ADIEncoder& right, ADIEncoder& back) :
  left(left),
  right(right),
  back(back),
  lst_left(0),
  lst_right(0),
  lst_back(0),
  gX(0),
  gY(0),
  gA(0)
{
  if (!task_started)
    Task tracking_task ( track );
  task_started = true;
}

void tracking::track()
{
  while (true)
  {

    int32_t left = ( left.get_value() - lst_left ) * SPIN_TO_IN_LR; //arc length travelled by left wheel inches
    int32_t right = ( right.get_value() - lst_right ) * SPIN_TO_IN_LR; //arc length travelled by right wheel in inches
    int32_t back = (back.get_value() - lst_back) * SPIN_TO_IN_B; //arc length travelled by back wheel in inches

    double a = (double)(right-left) / (L_DISTANCE_IN + R_DISTANCE_IN); //robot's turning angle
    double l = a/2.0; //half of the robot's turning angle; equal to the angle that opposotie to local x displaement

    double dist; //shortest distance between the centre of the robot's starting and ending points
    double distB; //shortest distance between the back wheel's starting and ending points


    if (a)
    {

      double r = ((double)right / a) + R_DISTANCE_IN; //radius of circle robot is travelling about
      dist = 2.0 * sin(l) * r;

      double rB = ((double)back / a) + B_DISTANCE_IN; //radius of circle back wheel travelling about
      distB = 2.0 * sin(l) * rB;

    }
    else
    {
      dist = right;
  		l = 0;

  		distB = back;
    }
    double g = l + gA; //angle used to calculate global displacement

    double x = sin(g) * dist; //global x displacement
    double y = cos(g) * dist; //global y displacement

  	x += cos(g) * hB; // account for back wheel movement
    y += -sin(g)* hB; // account for back wheel movement

  //Set global position variables
    gA += a;
    gX += x;
    gY += y;

  //Set last values
    lst_left = left.get_value();
    lst_right = right.get_value();
    lst_back = back.get_value();

    //Do this each cycle
    sleep (1);
    std::cout <<"xPos: " <<gX <<", yPos: " <<gY <<", a: " <<gA;
  }

}

void tracking::resetPosition()
{
  lst_left = lst_right = lst_back = 0;
	gA = gX = gY = 0;
}
