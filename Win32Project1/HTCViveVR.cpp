#include "stdafx.h"
#include "HTCViveVR.h"


// Destructor
LighthouseTracking::~LighthouseTracking() {
	if (m_pHMD != NULL)
	{
		vr::VR_Shutdown();
		m_pHMD = NULL;
	}
}

// Constructor
LighthouseTracking::LighthouseTracking() {

	//this->dummyHWND = ;
	//::SetWindowTextA(this->dummyHWND, "Dummy Window!");
	//MoveToEx(GetDC(this->dummyHWND), 0, 0, NULL);

	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Background);



	while (!m_pHMD) {
		//std::cout << "Waiting for SteamVR..." << std::endl;
		MessageBox(NULL, L"Waiting for SteamVR...", L"Error", NULL);
		Sleep(5000);
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Background);
	}

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		printf_s(buf);
		exit(EXIT_FAILURE);
	}
}

/*
* Loop-listen for events then parses them (e.g. prints the to user)
* Returns true if success or false if openvr has quit
*/
bool LighthouseTracking::RunProcedure(bool bWaitForEvents) {



	// Either A) wait for events, such as hand controller button press, before parsing...
	if (bWaitForEvents) {
		// Process VREvent
		vr::VREvent_t event;
		while (m_pHMD->PollNextEvent(&event, sizeof(event)))
		{
			ParseTrackingFrame();
		}
	}
	else {
		// ... or B) continous parsint of tracking data irrespective of events
		std::cout << std::endl << "Parsing next frame...";

		ParseTrackingFrame();
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Processes a single VR event
//-----------------------------------------------------------------------------

bool LighthouseTracking::ProcessVREvent(const vr::VREvent_t & event) {

	return true;
}


// Get the quaternion representing the rotation
vr::HmdQuaternion_t LighthouseTracking::GetRotation(vr::HmdMatrix34_t matrix) {
	vr::HmdQuaternion_t q;

	q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
	return q;
}

// Get the vector representing the position
vr::HmdVector3_t LighthouseTracking::GetPosition(vr::HmdMatrix34_t matrix) {
	vr::HmdVector3_t vector;

	vector.v[0] = matrix.m[0][3];
	vector.v[1] = matrix.m[1][3];
	vector.v[2] = matrix.m[2][3];

	return vector;
}

/*
* Parse a Frame with data from the tracking system
*
* Handy reference:
* https://github.com/TomorrowTodayLabs/NewtonVR/blob/master/Assets/SteamVR/Scripts/SteamVR_Utils.cs
*
* Also:
* Open VR Convention (same as OpenGL)
* right-handed system
* +y is up
* +x is to the right
* -z is going away from you
* http://www.3dgep.com/understanding-the-view-matrix/
*
*/
std::vector<double> LighthouseTracking::ParseTrackingFrame() {

	// Process SteamVR device states
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		// if not connected just skip the rest of the routine
		if (!m_pHMD->IsTrackedDeviceConnected(unDevice))
			continue;

		vr::TrackedDevicePose_t trackedDevicePose;
		vr::TrackedDevicePose_t *devicePose = &trackedDevicePose;

		vr::VRControllerState_t controllerState;
		vr::VRControllerState_t *ontrollerState_ptr = &controllerState;

		vr::HmdVector3_t position;
		vr::HmdQuaternion_t quaternion;

		if (vr::VRSystem()->IsInputFocusCapturedByAnotherProcess()) {
			char buf[1024];

			sprintf_s(buf, sizeof(buf), "\nInput Focus by Another Process\n");
			printf_s(buf);
		}

		bool bPoseValid = trackedDevicePose.bPoseIsValid;
		vr::HmdVector3_t vVel;
		vr::HmdVector3_t vAngVel;
		vr::ETrackingResult eTrackingResult;

		// Get what type of device it is and work with its data
		vr::ETrackedDeviceClass trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(unDevice);
		switch (trackedDeviceClass) {
		case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
		{
			// print stuff for the HMD here, see controller stuff in next case block

			// get pose relative to the safe bounds defined by the user
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1);

			// get the position and rotation
			position = GetPosition(devicePose->mDeviceToAbsoluteTracking);
			quaternion = GetRotation(devicePose->mDeviceToAbsoluteTracking);

			// get some data
			vVel = trackedDevicePose.vVelocity;
			vAngVel = trackedDevicePose.vAngularVelocity;
			eTrackingResult = trackedDevicePose.eTrackingResult;
			bPoseValid = trackedDevicePose.bPoseIsValid;

			// print the tracking data
			//char buf[1024];
			//sprintf_s(buf, sizeof(buf), "\nHMD\nx: %.2f y: %.2f z: %.2f\n", position.v[0], position.v[1], position.v[2]);
			//printf_s(buf);
			//sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
			//printf_s(buf);
		
			double vrData[6] = { position.v[0], position.v[1], position.v[2], quaternion.x, quaternion.y, quaternion.z };
			std::vector<double> dataVr(vrData, vrData + sizeof(vrData) / sizeof(vrData[0]));
			return dataVr;



			//LineTo(GetDC(this->dummyHWND), quaternion.x, quaternion.y);

			// and print some more info to the user about the state of the device/pose
			/*switch (eTrackingResult) {
			case vr::ETrackingResult::TrackingResult_Uninitialized:
				sprintf_s(buf, sizeof(buf), "Invalid tracking result\n");
				printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Calibrating_InProgress:
				sprintf_s(buf, sizeof(buf), "Calibrating in progress\n");
				printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Calibrating_OutOfRange:
				sprintf_s(buf, sizeof(buf), "Calibrating Out of range\n");
				printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Running_OK:
				sprintf_s(buf, sizeof(buf), "Running OK\n");
				printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Running_OutOfRange:
				sprintf_s(buf, sizeof(buf), "WARNING: Running Out of Range\n");
				printf_s(buf);

				break;
			default:
				sprintf_s(buf, sizeof(buf), "Default\n");
				printf_s(buf);
				break;
			}

			// print if the pose is valid or not
			if (bPoseValid)
				sprintf_s(buf, sizeof(buf), "Valid pose\n");
			else
				sprintf_s(buf, sizeof(buf), "Invalid pose\n");
			printf_s(buf);

			break;*/
		}
		}
	}
}

