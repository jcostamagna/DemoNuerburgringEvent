#pragma once
#ifndef _LIGHTHOUSETRACKING_H_
#define _LIGHTHOUSETRACKING_H_

#include "stdafx.h"
// OpenVR
#include <openvr.h>
#include "samples\shared\Matrices.h"
#include "Device.h"
#include <Windows.h>
#include <vector>

class LighthouseTracking : public Device {
private:

	// Basic stuff
	vr::IVRSystem *m_pHMD = NULL;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	Matrix4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

	// Position and rotation of pose
	vr::HmdVector3_t LighthouseTracking::GetPosition(vr::HmdMatrix34_t matrix);
	vr::HmdQuaternion_t LighthouseTracking::GetRotation(vr::HmdMatrix34_t matrix);

	// If false the program will parse tracking data continously and not wait for openvr events
	bool bWaitForEventsBeforeParsing = false;

	std::list<int> dataCollected;
	//HWND dummyHWND;

public:
	~LighthouseTracking();
	LighthouseTracking();

	// Main loop that listens for openvr events and calls process and parse routines, if false the service has quit
	bool RunProcedure(bool bWaitForEvents);

	// Process a VR event and print some general info of what happens
	bool ProcessVREvent(const vr::VREvent_t & event);

	// Parse a tracking frame and print its position / rotation
	std::vector<double> ParseTrackingFrame();

	std::list<int> getData() {
		return dataCollected;
	}

};

#endif _LIGHTHOUSETRACKING_H_
