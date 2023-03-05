#pragma once
#ifndef SPHERE_H
#define SPHERE_H
#include<hittable.h>

class sphere : public hittable
{
public:
	sphere(point3 cen, double r) :center(cen), radius(r) {};
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;


public:
	point3 center;
	double radius;
};


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	






	return true;
}




#endif SPHERE_H