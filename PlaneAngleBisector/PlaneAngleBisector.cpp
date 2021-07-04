//This Would be better if we take Relative Path.
#include "E:\CTPL\Sources\Build3600\Rapid-i\MainDLL\Handlers_DerivedShapes\PlaneAngleBisector.h"


//In the class PlaneAngleBisector we inherited the class of MouseHandler. That would actually help the
//newly formed plane to adjust accordingly.

//Constructor
PlaneAngleBisector::PlaneAngleBisector()
{
	try
	{
		
		init();
		printf("From The Constructor");
		MAINDllOBJECT->SetStatusCode("PlaneAngleBisector"); // What this thing is actually Doing ??

	}
	catch (...) {
		MAINDllOBJECT->SetAndRaiseErrorMessage("VIKASPLANEBISECTOR", __FILE__, __FUNCSIG__);
	}
}
//Destructor
PlaneAngleBisector::~PlaneAngleBisector()
{

	try
	{
		printf("From The Destructor");
		ResetShapeHighlighted();
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0002", __FILE__, __FUNCSIG__); }
}
//This would reset the Selection of planes on the screen.
//This would help hugly when we actually need to create multiple planes while recursively.
void PlaneAngleBisector::ResetShapeHighlighted()
{
	try
	{
		if (ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if (ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("PLANEANGLEBISECTOR", __FILE__, __FUNCSIG__); }
}
//the Method defines the maxclicks and certain other things.
//This is part of Constructor
void PlaneAngleBisector::init() 
{
	resetClicks();
	OffsetDefined = false;
	printf("From the init method");
	offset = 0;
	PlaneLength = 1;
	setMaxClicks(3);
	ParentShape1 = NULL;
	ParentShape2 = NULL;
	//created plane would help us draw the plane.
	memset(CreatedPlane, 0, sizeof(double) * 12);
	

}

//This class do actually all the calculations by which we can obtain the Angle Bisector of the Plane.
void PlaneAngleBisector::ClassOfCalculation()
{
	//printf("TheMainFunctionisHavingControl");
	((Plane*)ParentShape1)->getParameters(&Plane1Param[0]);
	((Plane*)ParentShape2)->getParameters(&Plane2Param[0]);
	((Plane*)ParentShape1)->DefinedOffset(OffsetDefined);
	((Plane*)ParentShape2)->DefinedOffset(OffsetDefined);
	((Plane*)ParentShape1)->getEndPoints(&Plane1EndPoints[0]);
	((Plane*)ParentShape2)->getEndPoints(&Plane2EndPoints[0]);
	double pt1[3], pt2[3];
	((Plane*)ParentShape1)->getPoint1()->FillDoublePointer(&pt1[0], 3);
	((Plane*)ParentShape1)->getPoint3()->FillDoublePointer(&pt2[0], 3);
	double mid1[3] = { (pt1[0] + pt2[0]) / 2, (pt1[1] + pt2[1]) / 2, (pt1[2] + pt2[2]) / 2 };

	((Plane*)ParentShape2)->getPoint1()->FillDoublePointer(&pt1[0], 3);
	((Plane*)ParentShape2)->getPoint3()->FillDoublePointer(&pt2[0], 3);

	double mid2[3] = { (pt1[0] + pt2[0]) / 2, (pt1[1] + pt2[1]) / 2, (pt1[2] + pt2[2]) / 2 };
	int functionreturnvalue = RMATH3DOBJECT->Intersection_Plane_Plane(Plane1Param, Plane2Param, IntersectionLine);
	//The Method to find intersection is actually returning the integer which will Return
	//0 if parallel
	//1 if Coincide
	//2 if non Parallel
	//and thus the cases
	if (functionreturnvalue == 0)
	{
		//If we are in this module then it means the planes are parallel.
		double Dist = RMATH3DOBJECT->Distance_Plane_Plane(&Plane1Param[0], &Plane2Param[0]);
		//Distance Between 2 planes.
		double midpoint = Dist / 2;
		double MSelectionLine[6], tmpLine[6] = { 0 }, tmpPoints[6] = { 0 };
		MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);//Is this line actually Required ? 
		//this array is actually doing (x1 + x3)/2 , (y1+y3)/2, (z1+z3)/2; 
		//clearly the array midpoint would store the coordinates of the midpoint of the plane p1.
		//p1 is the plane which USER would select the first Time.
		double midPnt[3] = { (Plane1EndPoints[0] + Plane1EndPoints[6]) / 2, (Plane1EndPoints[1] + Plane1EndPoints[7]) / 2, (Plane1EndPoints[2] + Plane1EndPoints[8]) / 2 };
		// Copying the midpoints & Plane1Param to the templin
		for (int i = 0; i < 3; i++)
		{

			tmpLine[i] = midPnt[i];
			tmpLine[3 + i] = Plane1Param[i];
		}
		//The Inspiration of making thing is like from the Class Parallel2plane 
		//where we are creating the plane parallel to the selected plane.

		//-> here we are creating a plane parallel to the plane selected first.
		//but the plane we have to create would be on the distance = midpoint because that is the 
		//middle distance between 2 planes.
		RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, tmpLine, tmpPoints);
		double curD = Plane1Param[0] * tmpPoints[0] + Plane1Param[1] * tmpPoints[1] + Plane1Param[2] * tmpPoints[2];
		Plane1Param[3] += midpoint;
		for (int i = 0; i < 4; i++)
			RMATH3DOBJECT->Projection_Point_on_Plane(&Plane1EndPoints[3 * i], Plane1Param, &CreatedPlane[3 * i]);
	}


	if (functionreturnvalue == 1)
	{//if the planes are coincide. So we don't have to think about this condition.
		//Because if two planes coincide, technically the bisector is something passing through the plane 
		//itself.
		//So better we don't have to think about this condition.


		//Should be put break or something in this Condition ?

	}
	if (functionreturnvalue == 2)
	{
		//This part deals when the planes are actually not parallel or non coincide
		//Means this is the non Trivial Part of the problem
		//Or do I say to this solution.

		RMATH3DOBJECT->TransformationMatrix_Angle_Pln2Pln(&Plane1Param[0], &Plane2Param[0], &mid1[0], &TransformationMatrix[0], &MeasurementPlane[0]);
		//Angle Between 2 Planes.
		double Angle = RMATH3DOBJECT->Angle_Plane_Plane(&Plane1Param[0], &Plane2Param[0], MeasurementPlane, DimSelectionLine);
		//The moment we find out the angle now we can take help from the Module 
		//--------------------------------
		//		LineAngleBisector.cpp
		//---------------------------------

		/*
		the way they are doing with line angle Bisector we can also do that.
		Since Because the plane is in 3d Then we have to consider the factor that we should produce the
		plane on the line-angle Bisector on the 90 degree.
		*/


		//Finding the Line Passing through the excat middle of the plane1.
		double x1 = Plane1EndPoints[0];
		double y1 = Plane1EndPoints[1];
		double z1 = Plane1EndPoints[2];
		double x2 = Plane1EndPoints[3];
		double y2 = Plane1EndPoints[4];
		double z2 = Plane1EndPoints[5];
		double x3 = Plane1EndPoints[6];
		double y3 = Plane1EndPoints[7];
		double z3 = Plane1EndPoints[8];
		double x4 = Plane1EndPoints[9];
		double y4 = Plane1EndPoints[10];
		double z4 = Plane1EndPoints[11];
		double line1passingplane1[6];


		line1passingplane1[0] = (x1 + x2) / 2;
		line1passingplane1[1] = (y1 + y2) / 2;
		line1passingplane1[2] = (z1 + z2) / 2;
		line1passingplane1[3] = (x4 + x3) / 2;
		line1passingplane1[4] = (y4 + y3) / 2;
		line1passingplane1[5] = (z4 + z3) / 2;
		//Line passing Through the plane


		//Finding the Line Passing through the excat middle of the plane2.
		double x21 = Plane2EndPoints[0];
		double y21 = Plane2EndPoints[1];
		double z21 = Plane2EndPoints[2];
		double x22 = Plane2EndPoints[3];
		double y22 = Plane2EndPoints[4];
		double z22 = Plane2EndPoints[5];
		double x23 = Plane2EndPoints[6];
		double y23 = Plane2EndPoints[7];
		double z23 = Plane2EndPoints[8];
		double x24 = Plane2EndPoints[9];
		double y24 = Plane2EndPoints[10];
		double z24 = Plane2EndPoints[11];

		double line1passingplane2[6];

		line1passingplane2[0] = (x21 + x22) / 2;
		line1passingplane2[1] = (y21 + y22) / 2;
		line1passingplane2[2] = (z21 + z22) / 2;
		line1passingplane2[3] = (x24 + x23) / 2;
		line1passingplane2[4] = (y24 + y23) / 2;
		line1passingplane2[5] = (z24 + z23) / 2;
		//Line passing Through the plane


		//now we have actually 2 arrays which can be created and they are having the line coordinates passing through the plane.
		// now lets create the angle bisector of these 2 lines.


		//double slope = (y2 -y1) / (x2 - x1)// if if the line is in 3d then we have to chnage this somwhow.
		double slope1 = (line1passingplane1[4] - line1passingplane1[1]) / line1passingplane1[3] - line1passingplane1[0];
		double slope2 = (line1passingplane2[4] - line1passingplane2[1]) / line1passingplane2[3] - line1passingplane2[0];
		//Intersept would be 
		//lets say we find out the intercept of 2 lines
		double intersecpt1;
		double intersept2;
		//Now we have to find out the Bisection of 2 lines 
		//Just like we are doing in other perMentioned Class.
		
		//mp[2] would take the value of mouse pointer.
		double mp[3] = { getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(),getClicksValue(getClicksDone()).getZ() };
		//Create the Bisection of the two lines which are Created over from here.


	}

	//Now lets find out the Interection Line of plane plane Intersection.

}
double PlaneAngleBisector::getMiddleValue()
{
	return middledistance;
}

//Drawing call to opengl
void PlaneAngleBisector::draw(int windowno, double WPixelWidth)
{

	if (getClicksDone() == getMaxClicks() - 1)
		GRAFIX->draw_PlanePolygon(CreatedPlane, 4, true, false);
}

//Handle the mouse move..//
void PlaneAngleBisector::mouseMove()
{
	if (getClicksDone() == 2)
		ClassOfCalculation();
	//---------------completed !
}

//Handle the Maximum Click
void PlaneAngleBisector::LmaxmouseDown()
{
	double clickPoint[6] = { 0 }, parentParam[4] = { 0 };
	((Plane*)ParentShape1)->getParameters(&Plane1Param[0]);
	((Plane*)ParentShape2)->getParameters(&Plane2Param[0]);
	if (MAINDllOBJECT->highlightedPoint() != NULL)
	{
		Vector* pt = MAINDllOBJECT->highlightedPoint();
		clickPoint[0] = pt->getX();
		clickPoint[1] = pt->getY();
		clickPoint[2] = pt->getZ();
	}
	else
	{
		double MSelectionLine[6] = { 0 }, tmpLine[6] = { 0 };
		MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
		double parentEndPnts[12] = { 0 };
		((Plane*)ParentShape1)->getEndPoints(&Plane1EndPoints[0]);
		((Plane*)ParentShape2)->getEndPoints(&Plane2EndPoints[0]);
		double midPnt[3] = { (parentEndPnts[0] + parentEndPnts[6]) / 2, (parentEndPnts[1] + parentEndPnts[7]) / 2, (parentEndPnts[2] + parentEndPnts[8]) / 2 };
		for (int i = 0; i < 3; i++)
		{
			tmpLine[i] = midPnt[i];
			tmpLine[3 + i] = parentParam[i];
		}
		RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, parentParam, clickPoint);
		if (RMATH3DOBJECT->Checking_Point_on_Plane(clickPoint, parentParam))
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}
		CurrentShape = new Plane();
		setBaseRProperty(clickPoint[0], clickPoint[1], clickPoint[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		((Plane*)CurrentShape)->PlaneType = RapidEnums::PLANETYPE::BISECTOROFPLANE;
		ParentShape1->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape1);
		((Plane*)CurrentShape)->Offset(offset);
		((Plane*)CurrentShape)->DefinedOffset(OffsetDefined);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
		//baseaction->AllPointsList.deleteAll();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
		//Completed----------------------------!
		
	}
}
void PlaneAngleBisector::RmouseDown(double x, double y)
{
}
void PlaneAngleBisector::SetAnyData(double* data)
{
//Setting the data.
	try
	{
		offset = data[0];
		if (data[0] != 0 && getClicksDone() == 1)
			OffsetDefined = true;
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0005", __FILE__, __FUNCSIG__); }
}
void PlaneAngleBisector::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("VikasAngleBisector", __FILE__, __FUNCSIG__); }
}
void PlaneAngleBisector::PartProgramData()
{

}


//I think This is for actually The Left Click of the mouse.
//From here we actually select the Planes by using the Left Mouse Click.
void PlaneAngleBisector::LmouseDown()
{
	try 
	{
		if (MAINDllOBJECT->highlightedShape() == NULL) 
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			ParentShape1 = (Plane*)MAINDllOBJECT->highlightedShape();
			ParentShape2 = (Plane*)MAINDllOBJECT->highlightedShape();
			MAINDllOBJECT->DerivedShapeCallback();

			/*	MAINDllOBJECT->SetStatusCode("DL002");*/
		}
		else 
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("VIKASPLANEANGLEBISECTOR", __FILE__, __FUNCSIG__); }
}
void PlaneAngleBisector::MouseScroll(bool flag)
{
// Copied from the LineBisector thing.
	//its not doing anything actually
}


//Mental Points:
/*
First the plane have to move with the mouse, Like in the mouseHandling Thing.
Second, How big should you create the Plane Like, We can be same as the parallel thing.
In which Dimension you should make the plane ?
That path which actually 
*/