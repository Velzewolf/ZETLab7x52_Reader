#ifndef COM_H
#define COM_H

#include <vector>
#include <windows.h>

using namespace std;

struct Device{
	BYTE Channel;
	DWORD BaudRate;
	BYTE Parity;
	Device(BYTE Ch, DWORD BR, BYTE P){
		Channel = Ch; 
		BaudRate = BR; 
		Parity = P;
	}
};

struct Measure{
	float X;
	float Y;
	float Z;
};

class COM{
public:
	HANDLE COMPort;
	string COMName;
	vector<string>* ComList;
	vector<Device>* DeviceList;
	vector<Device>* ConnectedDevice;

	COM();
	vector<string>* CheckCOMPort();
	bool COMPortReady();
	bool COMPortClose();
	bool DCBReady(DWORD, BYTE);
	Measure GetMeasure(Device);
	float GetAxis(vector<unsigned char>, DWORD);
	vector<unsigned char> Stream(vector<unsigned char>*, DWORD);
	vector<Device>* DeviceScan();
	double GetTime();
};



#endif