//------------------------------------------------------------------------------
// File: AptinaDS.cpp
//
// Desc: DirectShow sample code - In-memory push mode source filter
//       accesses images from Aptina Cam and pushes it downstream.
//
// Copyright (c) Customised Technologies (P) Ltd. All rights reserved.
//------------------------------------------------------------------------------

#include <streams.h>
#include <InitGuid.h>
#include "AptinaDS.h"
#include "AptinaDSGuids.h"
//#include "ictpl_VideoControl.h"

AptinaSensor4_Api *ApS;				// An instance of the class that will call the Aptina API

//LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
//LARGE_INTEGER Frequency;

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

// Utility method to query location of installed SDK's media path
//const TCHAR* DXUtil_GetDXSDKMediaPath();
//void ReadImagefromCam(void* anything);
//void fnOpenCameraThread(void* anything);


/**********************************************
 *
 *  CPushPinBitmap Class
 *  
 *
 **********************************************/

CApCamOutPin::CApCamOutPin(HRESULT *phr, CSource *pFilter)
      : CSourceStream(NAME("ApCam FilterOutPin"), phr, pFilter, L"Out"),
        vWidth (800), 
		vHeight (600), 
		FrameRate (20),
        m_iFrameNumber(0)
{
	//m_pSource = (cApCam *)this->m_pFilter;// pFilter;
	FullFrameFlag = 0;
	*phr = S_OK;
}


CApCamOutPin::~CApCamOutPin()
{   
    //DbgLog((LOG_TRACE, 3, TEXT("Frames written %d"),m_iFrameNumber));
	CoTaskMemFree(&ppm);
	//free(m_pBmi);
	//delete(m_pMediaType);
}


// GetMediaType: This method tells the downstream pin what types we support.

// Here is how CSourceStream deals with media types:
//
// If you support exactly one type, override GetMediaType(CMediaType*). It will then be
// called when (a) our filter proposes a media type, (b) the other filter proposes a
// type and we have to check that type.
//
// If you support > 1 type, override GetMediaType(int,CMediaType*) AND CheckMediaType.
//
// In this case we support only one type, which we obtain from the bitmap file.

HRESULT CApCamOutPin::GetMediaType(CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pMediaType, E_POINTER);

	// Allocate enough room for the VIDEOINFOHEADER and the color tables
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));//SIZE_PREHEADER + m_pBmi->bmiHeader.biSize);
    if (pvi == 0) 
        return(E_OUTOFMEMORY);

    ZeroMemory(pvi, pMediaType->cbFormat);   
    pvi->AvgTimePerFrame = m_rtFrameLength;

	m_pBmi = (BITMAPINFO *)_alloca(sizeof(BITMAPINFOHEADER) +  256*sizeof(RGBQUAD));
	memset(&m_pBmi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));

	m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmi->bmiHeader.biWidth = vWidth;
	m_pBmi->bmiHeader.biHeight = -vHeight;
	//if (this->IsPreviewPin)
	//{	m_pBmi->bmiHeader.biWidth = vWidth / ReduceFactor;
	//	m_pBmi->bmiHeader.biHeight = -vHeight / ReduceFactor;
	//}
	m_pBmi->bmiHeader.biPlanes = 1;
	m_pBmi->bmiHeader.biBitCount = 24;
	m_pBmi->bmiHeader.biCompression = BI_RGB;
	m_pBmi->bmiHeader.biSizeImage = 0;
	m_pBmi->bmiHeader.biClrUsed = 0;
	m_pBmi->bmiHeader.biClrImportant = 0;

    // Copy the header info
	memcpy(&(pvi->bmiHeader), &(m_pBmi->bmiHeader), m_pBmi->bmiHeader.biSize);

    // Set image size for use in FillBuffer
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);

    // Clear source and target rectangles
    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pMediaType->SetType(&MEDIATYPE_Video);
	//pMediaType->SetSubtype(&MEDIASUBTYPE_RGB24);
    pMediaType->SetFormatType(&FORMAT_VideoInfo);
    pMediaType->SetTemporalCompression(FALSE);
	pMediaType->IsFixedSize();// = true;

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pMediaType->SetSubtype(&SubTypeGUID);
    pMediaType->SetSampleSize(pvi->bmiHeader.biSizeImage);
	if (FirstTimeGetType)
	{	//m_pMediaType = pMediaType;
		FirstTimeGetType = false;
	}
	m_pMediaType = pMediaType;
    return S_OK;
}


HRESULT CApCamOutPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest)
{
    HRESULT hr;
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pRequest, E_POINTER);

    // If the bitmap file was not loaded, just fail here.
    //if (!m_pImage)
    //    return E_FAIL;

    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*) m_mt.Format();
    
    // Ensure a minimum number of buffers
    if (pRequest->cBuffers == 0)
    {
        pRequest->cBuffers = 8;
    }
    pRequest->cbBuffer = pvi->bmiHeader.biSizeImage;

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pRequest, &Actual);
    if (FAILED(hr)) 
    {
        return hr;
    }

    // Is this allocator unsuitable?
    if (Actual.cbBuffer < pRequest->cbBuffer) 
    {
        return E_FAIL;
    }

    return S_OK;
}


// This is where we insert the DIB bits into the video stream.
// FillBuffer is called once for every sample in the stream.
HRESULT CApCamOutPin::FillBuffer(IMediaSample *pSample)
{
    BYTE *pData;
    long cbData;
		//QueryPerformanceFrequency(&Frequency); 

    CheckPointer(pSample, E_POINTER);

    //// If the bitmap file was not loaded, just fail here.
    //if (!ApS)
    //    return E_FAIL;

    CAutoLock cAutoLockShared(&m_cSharedState);

	//Make the API retrieve the latest image from Cam Sensor
	//ApS->fnOpenCameraThread();
	
    // Access the sample's data buffer
    pSample->GetPointer(&pData);
    cbData = pSample->GetSize();

    // Check that we're still using video
    ASSERT(m_mt.formattype == FORMAT_VideoInfo);

    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)m_mt.pbFormat;

	//We now have two output pins - one with uncompressed video, and one with compressed. 
	//We first find out which instance we are, and call the appropriate function - 
	//if (this->IsPreviewPin)
	//{	memcpy(pData, ApS->rgbImgReduced, cbData);
	//}
	//else
	//{	
		if (!ApS->retrieveRGBImage(pData, !IsPreviewPin))
			return E_INVALIDARG;
	//}
    // Copy the DIB bits over into our filter's output buffer.
    // Since sample size may be larger than the image size, bound the copy size.

   
	////Try writing one image to disk
	//BITMAPFILEHEADER bf;
	//bf.bfType = 0x4d42;
	//bf.bfSize = sizeof(BITMAPINFOHEADER);
	//bf.bfOffBits = 54;
	//m_pBmi = (BITMAPINFO *)_alloca(sizeof(BITMAPINFOHEADER) +  256*sizeof(RGBQUAD));
	//memset(&m_pBmi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));

	////output to bmp....?
	//std::ofstream outFile;
	//outFile.open("outtestbmp.bmp");
	//outFile.write((char *)&bf,sizeof(bf));
	//outFile.write((char *)&m_pBmi->bmiHeader,sizeof(m_pBmi->bmiHeader));
	//outFile.write((char *)pData, 800 * 600 * 3);
	//outFile.close();
	
	// Set the timestamps that will govern playback frame rate.
    // If this file is getting written out as an AVI,
    // then you'll also need to configure the AVI Mux filter to 
    // set the Average Time Per Frame for the AVI Header.
    // The current time is the sample's start
    REFERENCE_TIME rtStart = m_iFrameNumber * m_rtFrameLength;
    REFERENCE_TIME rtStop  = rtStart + m_rtFrameLength;

    pSample->SetTime(&rtStart, &rtStop);
    m_iFrameNumber++;

    // Set TRUE on every sample for uncompressed frames
	if (FullFrameFlag == 0)
		pSample->SetSyncPoint(TRUE);
	else
	{	if (FullFrameFlag == 3) FullFrameFlag = -1;
		pSample->SetSyncPoint(FALSE);
	}
	FullFrameFlag ++;

		//	QueryPerformanceCounter(&EndingTime);
		//ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
		//ElapsedMicroseconds.QuadPart *= 1000000;
		//ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
		//QueryPerformanceCounter(&StartingTime);

    return S_OK;
}

HRESULT WINAPI CApCamOutPin::GetFormat(AM_MEDIA_TYPE **pmt)
{	//First allocate memory for the structure.
	//**pmt = (AM_MEDIA_TYPE *) CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
	//if (!FirstTimeGetType)
	//{	ppm.formattype = m_pMediaType->formattype;
	//	ppm.subtype = m_pMediaType->subtype;
	//	ppm.bFixedSizeSamples = true;
	//	ppm.bTemporalCompression = 0;
	//	ppm.cbFormat = sizeof(VIDEOINFOHEADER);
	//	memcpy(ppm.pbFormat, m_pMediaType->pbFormat, ppm.cbFormat);
	//}
	//else
	{	ppm.formattype = FORMAT_VideoInfo;
		ppm.majortype = MEDIATYPE_Video;
		ppm.subtype = MEDIASUBTYPE_RGB24;
		ppm.cbFormat = sizeof(VIDEOINFOHEADER);
		ppm.pbFormat = (BYTE*) CoTaskMemAlloc(ppm.cbFormat);
		ppm.bFixedSizeSamples = 1;
		ppm.bTemporalCompression = 0;
		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*)malloc(sizeof(VIDEOINFOHEADER));
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
		pvi->AvgTimePerFrame = m_rtFrameLength;
		
		m_pBmi = (BITMAPINFO *)_alloca(sizeof(BITMAPINFOHEADER) +  256*sizeof(RGBQUAD));
		memset(&m_pBmi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));

		m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBmi->bmiHeader.biWidth = this->vWidth;
		m_pBmi->bmiHeader.biHeight = -this->vHeight;
		m_pBmi->bmiHeader.biPlanes = 1;
		m_pBmi->bmiHeader.biBitCount = 24;
		m_pBmi->bmiHeader.biCompression = BI_RGB;
		m_pBmi->bmiHeader.biSizeImage = 0;
		m_pBmi->bmiHeader.biClrUsed = 0;
		m_pBmi->bmiHeader.biClrImportant = 0;

		// Copy the header info
		memcpy(&(pvi->bmiHeader), &(m_pBmi->bmiHeader), m_pBmi->bmiHeader.biSize); 
		memcpy(ppm.pbFormat, pvi, sizeof(VIDEOINFOHEADER));
	}
	//*pmt = CreateMediaType(&ppm);
	*pmt = &ppm;
	//memcpy((void*)pmt, (void*)&ppm, sizeof(AM_MEDIA_TYPE));
	//
	//*pmt = &ppm;
	//free(pvi);
	return S_OK;
}

HRESULT WINAPI CApCamOutPin::GetNumberOfCapabilities(int *piCount, int *piSize)
{	
	return E_NOTIMPL;
}
//Have to Relocate more.
//Vikas on 16 AUG 2021

