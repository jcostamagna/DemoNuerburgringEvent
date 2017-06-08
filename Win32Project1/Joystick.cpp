#include "stdafx.h"
#include "Joystick.h"

DIDEVICEINSTANCE pdidi;
DIDEVICEINSTANCE info;
LPDIRECTINPUT8 di;
HRESULT hr;
LPDIRECTINPUTDEVICE8 joystick;

Joystick::Joystick()
{
}


Joystick::~Joystick()
{
}

HRESULT Joystick::poll(DIJOYSTATE *js) {
	HRESULT    hr;

	if (joystick == NULL)
	{
		return S_OK;
	}

	// Poll the device to read the current state
	hr = joystick->Poll();
	if (FAILED(hr)) {
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = joystick->Acquire();
		while (hr == DIERR_INPUTLOST) {
			hr = joystick->Acquire();
		}

		// If we encounter a fatal error, return failure.
		if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
			return E_FAIL;
		}

		// If another application has control of this device, return successfully.
		// We'll just have to wait our turn to use the joystick.
		if (hr == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
	}
	// Get the input's device state
	if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE), js))) {
		return hr; // The device should have been acquired during the Poll()
	}

	return S_OK;
}

void Joystick::GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;

	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();

	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);

	horizontal = desktop.right;
	vertical = desktop.bottom;
}



void Joystick::close()
{
	if (joystick)
	{
		joystick->Unacquire();
	}
}



BOOL CALLBACK
enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
	HRESULT hr;

	hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);

	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

BOOL CALLBACK
enumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
{
	HWND hDlg = (HWND)context;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -1000;
	propRange.lMax = +1000;

	if (FAILED(joystick->SetProperty(DIPROP_RANGE, &propRange.diph))) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

int Joystick::start()
{
                                       // struktura stanu joysticka

														  // Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL))) {
		return hr;
	}

	if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback,
		NULL, DIEDFL_ATTACHEDONLY))) {
		return hr;
	}

	// sprawdzenie czy jest joystick
	if (joystick == NULL) {

		std::cout << "Joystick not found.\n";
		system("pause");
		return E_FAIL;
	}



	// zdefiniowanie formatu danych urzadzenia
	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick)))
	{
		return hr;
	}

	// Powiazanie urzadzenia z oknem aplikacji
	if (FAILED(hr = joystick->SetCooperativeLevel(GetConsoleWindow(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
		return hr;
	}

	// wczytanie ustawien joysticka
	capabilities.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(hr = joystick->GetCapabilities(&capabilities))) {

		return hr;
	}

	// wyliczanie
	if (FAILED(hr = joystick->EnumObjects(enumAxesCallback, NULL, DIDFT_AXIS))) {
		return hr;
	}

	info.dwSize = sizeof(DIDEVICEINSTANCE);
	if (FAILED(hr = joystick->GetDeviceInfo(&info)))
	{
		return hr;
	}
	/*int i = 0;
	while (i < MAX_PATH && info.tszProductName[i] != 0)
	{
		std::cout << (char)info.tszProductName[i];
		i++;
	}
	std::cout << std::endl;
	//system("pause");


	
	while (i < 100)
	{
		poll(&js);

		for (int i = 0; i < 32; i++)
		{
			if (js.rgbButtons[i] != 0) std::cout << "Pressed " << i + 1 << std::endl;
		}

		std::cout << "X: " << js.lX << " Y: " << js.lY << " Z: " << js.lZ << " " << js.rglSlider[0] << " " << js.rglSlider[1]
			<< " " << js.lRx << " " << js.lRy << " " << js.lRz << std::endl;
	}
	*/
	return 0;
}


DIJOYSTATE * Joystick::getJoy()
{

	poll(&js);
	/*for (int i= 0; i < 100; ++i)
	{
		poll(&js);

		for (int i = 0; i < 32; i++)
		{
			if (js.rgbButtons[i] != 0) std::cout << "Pressed " << i + 1 << std::endl;
		}
	*/
	return &js;
}