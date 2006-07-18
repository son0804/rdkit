//
// Copyright (C) 2003-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//


#ifndef __RD_POINT_H__
#define __RD_POINT_H__
#include <iostream>
#include <cmath>
#include <vector>
#ifdef WIN32
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

#include <RDGeneral/Invariant.h>

namespace RDGeom {
class Point3D;
class Point2D;
}
std::ostream & operator<<(std::ostream& target, const RDGeom::Point3D &pt);
std::ostream & operator<<(std::ostream& target, const RDGeom::Point2D &pt);

RDGeom::Point3D operator+ (const RDGeom::Point3D& p1, const RDGeom::Point3D& p2);
RDGeom::Point3D operator- (const RDGeom::Point3D& p1, const RDGeom::Point3D& p2);
RDGeom::Point3D operator* (const RDGeom::Point3D& p1, const double v);
RDGeom::Point3D operator/ (const RDGeom::Point3D& p1, const double v);

RDGeom::Point2D operator+ (const RDGeom::Point2D& p1, const RDGeom::Point2D& p2);
RDGeom::Point2D operator- (const RDGeom::Point2D& p1, const RDGeom::Point2D& p2);
RDGeom::Point2D operator* (const RDGeom::Point2D& p1, const double v);
RDGeom::Point2D operator/ (const RDGeom::Point2D& p1, const double v);


namespace RDGeom {
  typedef class Point3D Point;
  class Point3D {

  public:
    double x,
      y,
      z;

    Point3D() : x(0.0), y(0.0), z(0.0) {};
    Point3D(double xv,double yv,double zv): x(xv),y(yv),z(zv) {};

    Point3D(const Point3D &other) :
      x(other.x), y(other.y), z(other.z) {
    }

    Point3D&
      operator=(const Point3D &other)
    {
      x = other.x;y=other.y;z=other.z;
      return *this;
    };

    Point3D& operator+=(const Point3D &other) {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    };
 
    Point3D& operator-=(const Point3D &other) {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    };
    
    Point3D& operator*=(double scale) {
      x *= scale;
      y *= scale;
      z *= scale;
      return *this;
    };  

    Point3D& operator/=(double scale) {
      x /= scale;
      y /= scale;
      z /= scale;
      return *this;
    };  

    Point3D operator-() const {
      Point3D res(x, y, z);
      res.x *= -1.0;
      res.y *= -1.0;
      res.z *= -1.0;
      return res;
    }

    void normalize() {
      double l = this->length();
      x /= l;
      y /= l;
      z /= l;
    };

    double length() const {
      double res = x*x+y*y+z*z;
      return sqrt(res);
    };

    double lengthSq() const {
      double res = pow(x,2) + pow(y,2) + pow(z,2);
      return res;
    };

    double dotProduct(const Point3D &other) const {
      double res = x*(other.x) + y*(other.y) + z*(other.z);
      return res;
    };

    /*! \brief determines the angle between a vector to this point
     *   from the origin and a vector to the other point.
     * 
     *  The angle is unsigned: the results of this call will always
     *   be between 0 and M_PI
     */
    double angleTo(const Point3D &other) const {
      Point3D t1,t2;
      t1 = *this;
      t2 = other;
      t1.normalize();
      t2.normalize();
      double dotProd = t1.dotProduct(t2);
      // watch for roundoff error:
      if(dotProd<-1.0) dotProd = -1.0;
      else if(dotProd>1.0) dotProd = 1.0;
      return acos(dotProd);
    }

    /*! \brief determines the signed angle between a vector to this point
     *   from the origin and a vector to the other point.
     * 
     *  The results of this call will be between 0 and M_2_PI
     */
    double signedAngleTo(const Point3D &other) const {
      double res=this->angleTo(other);
      // check the sign of the z component of the cross product:
      if((this->x*other.y-this->y*other.x)<-1e-6) res = 2.0*M_PI-res;
      return res;
    }

    /*! \brief Returns a normalized direction vector from this
     *   point to another.
     * 
     */
    Point3D directionVector(const Point3D &other) const {
      Point3D res;
      res.x = other.x - x;
      res.y = other.y - y;
      res.z = other.z - z;
      res.normalize();
      return res;
	
    }

    
    /*! \brief Cross product of this point with the another point
     * 
     * The order is important here
     *  The result is "this" cross with "other" not (other x this)
     */
    Point3D crossProduct(const Point3D &other) const {
      Point3D res;
      res.x = y*(other.z) - z*(other.y);
      res.y = -x*(other.z) + z*(other.x);
      res.z = x*(other.y) - y*(other.x);
      return res;
    };

    /*! \brief Get a unit perpendicular from this point (treating it as a vector):
     *
     */
    Point3D getPerpendicular() const {
      Point3D res(0.0,0.0,0.0);
      if(x){
	if(y){
	  res.y = -1*x;
	  res.x = y;
	} else if(z) {
	  res.z = -1*x;
	  res.x = z;
	} else {
	  res.y = 1;
	}
      } else if(y){
	if(z){
	  res.z = -1*y;
	  res.y = z;
	} else {
	  res.x = 1;
	}
      } else if(z){
	res.x = 1;
      }
      double l=res.length();
      POSTCONDITION(l>0.0,"zero perpendicular");
      res = res/l;
      return res;
    }
  };
  
  // given a  set of four pts in 3D compute the dihedral angle between the
  // plane of the first three points (pt1, pt2, pt3) and the plane of the 
  // last three points (pt2, pt3, pt4)
  // the computed angle is between 0 and PI
  double computeDihedralAngle(Point3D pt1, Point3D pt2, Point3D pt3, Point3D pt4);

  class Point2D {
  public:
    double x,
      y;

    Point2D() : x(0.0), y(0.0) {};
    Point2D(double xv,double yv): x(xv),y(yv) {};
    
    Point2D(const Point2D &other) : x(other.x), y(other.y) {
    }

    Point2D&
      operator=(const Point2D &other)
    {
      x = other.x;y=other.y;
      return *this;
    };

    Point2D& operator+=(const Point2D &other) {
      x += other.x;
      y += other.y;
      return *this;
    };

    Point2D& operator-=(const Point2D &other) {
      x -= other.x;
      y -= other.y;
      return *this;
    };
    
    Point2D& operator*=(double scale){
      x *= scale;
      y *= scale;
      return *this;
    };

    Point2D& operator/=(double scale){
      x /= scale;
      y /= scale;
      return *this;
    };

    Point2D operator-() const {
      Point2D res(x, y);
      res.x *= -1.0;
      res.y *= -1.0;
      return res;
    }
      
    void normalize() {
      double ln = this->length();
      x /= ln;
      y /= ln;
    };

    void rotate90() {
      double temp = x;
      x = -y;
      y = temp;
    }

    double length() const {
      double res = pow(x,2) + pow(y,2);
      return sqrt(res);
    };

    double lengthSq() const {
      double res = x*x+y*y;
      return res;
    };

    double dotProduct(const Point2D &other) const {
      double res = x*(other.x) + y*(other.y);
      return res;
    };

    double angleTo(const Point2D &other) const {
      Point2D t1,t2;
      t1 = *this;
      t2 = other;
      t1.normalize();
      t2.normalize();
      double dotProd = t1.dotProduct(t2);
      // watch for roundoff error:
      if(dotProd<-1.0) dotProd = -1.0;
      else if(dotProd>1.0) dotProd = 1.0;
      return acos(dotProd);
    }

    double signedAngleTo(const Point2D &other) const {
      double res=this->angleTo(other);
      if((this->x*other.y-this->y*other.x)<-1e-6) res = 2.0*M_PI-res;
      return res;
    }

    Point2D directionVector(const Point2D &other) const {
      Point2D res;
      res.x = other.x - x;
      res.y = other.y - y;
      res.normalize();
      return res;
	
    }
    
  };

  typedef std::vector<RDGeom::Point3D *> Point3DPtrVect;
  typedef std::vector<RDGeom::Point2D *> Point2DPtrVect;
  typedef Point3DPtrVect::iterator Point3DPtrVect_I;
  typedef Point3DPtrVect::const_iterator Point3DPtrVect_CI;
  typedef Point2DPtrVect::iterator Point2DPtrVect_I;
  typedef Point2DPtrVect::const_iterator Point2DPtrVect_CI;

  typedef std::vector<const RDGeom::Point3D *> Point3DConstPtrVect;
  typedef Point3DConstPtrVect::iterator Point3DConstPtrVect_I;
  typedef Point3DConstPtrVect::const_iterator Point3DConstPtrVect_CI;
}




#endif
