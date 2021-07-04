#pragma once
#include "..\Handlers\MouseHandler.h"
#include "StdAfx.h"
#include "..\MainDLL\Measurement\DimPlaneToPlaneDistance.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Handlers_DerivedShapes\PlaneParallel2Plane.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddPointAction.h"
//#include "..\RapidMath3D\Engine\RapidMath3D.h";



//The reason I inherit Mousehandler over here because we required Mouse Handling stuff inorder to create Bisection.
class PlaneAngleBisector:
	public MouseHandler
{
private:
	double CreatedPlane[12] = {0};
	Shape *ParentShape1, * ParentShape2;
	double PlaneLength;
	double Plane1Param[4] = { 0 }, Plane2Param[4] = { 0 }, Plane1EndPoints[12] = { 0 }, Plane2EndPoints[12] = { 0 }, IntersectionLine[6] = { 0 };
	void init();
	double middledistance;

	//PlanePosition Actually
	int planepos;
	bool OffsetDefined;
	bool offset;
protected:
	double TransformationMatrix[9], MeasurementPlane[4], DimSelectionLine[6];
public:
	//double * IntersectionLine;
	//This Might Cause the error cause we are not passing the parameters.
	void ClassOfCalculation();
	PlaneAngleBisector();
	~PlaneAngleBisector();
	void ResetShapeHighlighted();
	double getMiddleValue();

	//Virtual Functions

	virtual void mouseMove();//done
	virtual void LmouseDown();//done
	virtual void LmaxmouseDown();//done
	virtual void draw(int windowno, double WPixelWidth);//done
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);//done
	virtual void PartProgramData();//complete
	virtual void SetAnyData(double* data);//done
	virtual void RmouseDown(double x, double y);
};




//is that okay to use all the virtual functions.