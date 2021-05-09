DimBase* HelperClass::CreateAngularityDim2D(Shape* s1, Shape* s2, Shape* s3, RapidEnums::MEASUREMENTTYPE CurrentMtype, double Angle, double land, double x, double y, double z)
{
	try
	{
		DimBase* Cdim = NULL;
		switch(CurrentMtype)
		{
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
				Cdim = new DimAngularity(_T("Par"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(0, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
				Cdim = new DimAngularity(_T("Per"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
				Cdim = new DimAngularity(_T("Anglty"), CurrentMtype);
				((DimAngularity*)Cdim)->SetLandAndAngle(Angle * M_PI/180, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimAngularity*)Cdim)->CalculateMeasurement(s1, s2);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY:
			case RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY_D:
				Cdim = new DimConcentricity(_T("Concen"));
				Cdim->MeasurementType = CurrentMtype;
				((DimConcentricity*)Cdim)->CalculateMeasurement(s1, s2);
				break;			
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D:
				Cdim = new DimTruePosition(_T("TPG"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONX:
				Cdim = new DimTruePosition(_T("TPX"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONY:
				Cdim = new DimTruePosition(_T("TPY"), CurrentMtype);
				((DimTruePosition*)Cdim)->SetTPposition(Angle, land);
				Cdim->MeasurementType = CurrentMtype;
				((DimTruePosition*)Cdim)->CalculateMeasurement(s1, s2, s3);
				break;
			//case RapidEnums::MEASUREMENTTYPE::DIM_PROFILE:
			//	Cdim = new DimProfile(_T("Pf"));
			//	Cdim->MeasurementType = CurrentMtype;
			//	((DimProfile*)Cdim)->CalculateMeasurement(s1, s2, s3, s4);
			//	break;

		}
		if(Cdim != NULL)
		{
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			Cdim->SetMousePosition(x, y, z);
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			AddGDnTReferenceDatum(Cdim, s2);
			if(CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R || CurrentMtype == RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_D)
				AddGDnTReferenceDatum(Cdim, s3);
			return Cdim;
		}
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMF0003", __FILE__, __FUNCSIG__); return NULL; }
}