/*
 * Path.h
 *
 *  Created on: Feb 27, 2012
 *      Author: rellik
 */

#ifndef PATH_H_
#define PATH_H_

#include "Vector2D.h"
#include <vector>
#include "Point.h"


/**
 * Clase Path
 * Implementa un camino a seguir
 */
class Path{
public:
	Path();
	~Path();
	Path(const Path &path);
	Path & operator=(const Path &path);

	Path(vector<Point> camino, bool loop_=false);
	Path(vector<Vector2D> camino, bool loop_=false);

	void setWaypoints(vector<Point> camino, bool loop_=false);
	void setWaypoints(vector<Vector2D> camino, bool loop_=false);
	void addWaypoint(Vector2D vec);
	void addWaypoint(Point p);

	vector<Point> getWaypointsP();
	vector<Vector2D> getWaypointsV();
	Point getCurrentWaypointP();
	Vector2D getCurrentWaypointV();

	bool setCurrentWaypoint(Point p);
	bool setCurrentWaypoint(int index);

	bool setNextWaypoint();

	void setLoop(bool isALoop=true);
	bool lastWaypoint();
	bool isFinished();

private:
	/**
	 * Puntos a seguir
	 */
	vector<Vector2D> waypoints;

	/**
	 * Punto actual
	 */
	vector<Vector2D>::iterator wpIter;

	/**
	 * Si es un bucle o no
	 */
	bool loop;
};


#endif /* PATH_H_ */
