#include "StdAfx.h"
#include "DimConcentricity.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\RPoint.h"

DimConcentricity::DimConcentricity(TCHAR* myname):DimBase(genName(myname))
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0001", __FILE__, __FUNCSIG__); }
}

DimConcentricity::DimConcentricity(bool simply):DimBase(false)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0002", __FILE__, __FUNCSIG__); }
}

DimConcentricity::~DimConcentricity()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0003", __FILE__, __FUNCSIG__); }
}

TCHAR* DimConcentricity::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 20, prefix);
		ConcenCnt++;
		count++;
		TCHAR shapenumstr[20];
		_itot_s(ConcenCnt, shapenumstr, 20, 10);
		_tcscat_s(name, 20, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0004", __FILE__, __FUNCSIG__); return _T("NA"); }
} 

void DimConcentricity::drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm)
{
	try
	{
		double cen[3] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(), ((Circle*)ParentShape1)->getCenter()->getZ()};
		GRAFIX->drawConcentricity(mposition.getX(), mposition.getY(), ((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY(),  ((Circle*)ParentShape1)->Radius() * 2, getCDimension(), &ParentShape2->DatumName, getModifiedName(), windowno, WPixelWidth, TextScaleFactor, Left_Top, Right_Btm, cen[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0005", __FILE__, __FUNCSIG__); }
}


void DimConcentricity::CalculateMeasurement(Shape* s1, Shape* s2)
{
	try
	{
		this->ParentShape1 = s1;
		this->ParentShape2 = s2;
		//this->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY;
		double cen1[2] = {((Circle*)ParentShape1)->getCenter()->getX(), ((Circle*)ParentShape1)->getCenter()->getY()};
		double cen2[2] = { 0 };
		double multiplier = 1.0;
		if (MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D) multiplier = 2.0;

		if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE || ParentShape2->ShapeType == RapidEnums::SHAPETYPE::XLINE || ParentShape2->ShapeType == RapidEnums::SHAPETYPE::XRAY)
		{
			Line* RefLine = (Line*)ParentShape2;
			cen2[0] = RefLine->Angle();
			cen2[1] = RefLine->Intercept();
			setDimension(multiplier * RMATH2DOBJECT->Pt2Line_Dist(cen1[0], cen1[1], cen2[0], cen2[1]));
		}
		else if (ParentShape2->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			Vector* pp = ((RPoint*)ParentShape2)->getPosition();
			cen2[0] = pp->getX();
			cen2[1] = pp->getY();
			//cen3[2] = pp->getZ();
			setDimension(multiplier * RMATH2DOBJECT->Pt2Pt_distance(cen1[0], cen1[1], cen2[0], cen2[1]));
		}
		else
		{
			cen2[0] = ((Circle*)ParentShape2)->getCenter()->getX(); 
			cen2[1] = ((Circle*)ParentShape2)->getCenter()->getY();
			setDimension(multiplier * RMATH2DOBJECT->Pt2Pt_distance(cen1[0], cen1[1], cen2[0], cen2[1]));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0006", __FILE__, __FUNCSIG__); }
}

void DimConcentricity::UpdateMeasurement()
{
	CalculateMeasurement(ParentShape1, ParentShape2);
}

void DimConcentricity::UpdateForParentChange(BaseItem* sender)
{
	try
	{
		if(this->UpperTolerance() != 0.0 || this->LowerTolerance() != 0.0 || this->NominalValue() != 0.0)
			this->UpdateMeasurementColor();
		if(sender->getId() == ParentShape1->getId())
			this->CalculateMeasurement((Shape*)sender, ParentShape2);
		else if(sender->getId() == ParentShape2->getId())
			this->CalculateMeasurement(ParentShape1, (Shape*)sender);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0007", __FILE__, __FUNCSIG__); }
}

DimBase* DimConcentricity::Clone(int n)
{
	try
	{
		DimConcentricity* Cdim = new DimConcentricity(_T("Concen"));
		Cdim->CopyOriginalProperties(this);
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0008", __FILE__, __FUNCSIG__); return NULL; }
}

DimBase* DimConcentricity::CreateDummyCopy()
{
	try
	{
		DimConcentricity* Cdim = new DimConcentricity(false);
		Cdim->CopyOriginalProperties(this);
		Cdim->setId(this->getId());
		return Cdim;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0009", __FILE__, __FUNCSIG__); return NULL;}
}

void DimConcentricity::CopyMeasureParameters(DimBase* Cdim)
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0010", __FILE__, __FUNCSIG__); }
}

int DimConcentricity::ConcenCnt = 0;
void DimConcentricity::reset()
{
	ConcenCnt = 0;
}

wostream& operator<<( wostream& os, DimConcentricity& x)
{
	try
	{
		os << (*static_cast<DimBase*>(&x));
		os<<"DimConcentricity"<<endl;
		os <<"ParentShape1:"<< x.ParentShape1->getId() << endl;
		os <<"ParentShape2:"<< x.ParentShape2->getId() << endl;
		os<<"EndDimConcentricity"<<endl;		
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0011", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, DimConcentricity& x)
{
	try
	{
		is >> (*(DimBase*)&x);	
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"DimConcentricity")
			{
				while(Tagname!=L"EndDimConcentricity")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{						
						MAINDllOBJECT->PPLoadSuccessful(false);
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end() );
						Tagname=Tag;												
						if(Tagname==L"ParentShape1")
						{
							x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}		
						else if(Tagname==L"ParentShape2")
						{
							x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atoi((const char*)(Val).c_str())];
						}	
					}
				}
				HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
				x.UpdateMeasurement();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DimConcentricity& x)
{
	try
	{
		int n;
		is >> n;
		x.ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		is >> n;
		x.ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n];
		HELPEROBJECT->AddGDnTReference(&x, x.ParentShape2);
		x.UpdateMeasurement();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DIMCONC0012", __FILE__, __FUNCSIG__);}
}