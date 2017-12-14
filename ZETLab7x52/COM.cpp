#include <string>
#include "cstdint"
#include "COM.h"

//��������, �� ������� ����� ��������� ���������
static const DWORD baudRates[] = {
	/*CBR_2400,
	CBR_4800,
	CBR_9600,
	CBR_19200,*/
	CBR_38400,
	/*CBR_57600,
	CBR_115200*/
};
const unsigned short baudRateCount = sizeof(baudRates) / sizeof(DWORD);

//�������� ���� ��������, �� ������� ����� ��������� ��������� 
static const BYTE parities[] = {
	NOPARITY,
	ODDPARITY
};
const unsigned short parityCount = sizeof(parities) / sizeof(BYTE);

COM::COM(){
	COMPort = NULL;
	ComList = new vector<string>;
	ComList = CheckCOMPort();
	DeviceList = new vector <Device>;
	ConnectedDevice = new vector <Device>;
}

vector<string>* COM::CheckCOMPort(){
	char* PortName = new char[10];
	vector<string>* AvaliablePort = new vector<string>;
	for (int i = 1; i <= 256; ++i)
	{
		if (i < 10)
			sprintf(PortName, "COM%d", i);
		else
			sprintf(PortName, "\\\\.\\COM%d", i);
		COMPort = CreateFile(PortName,
			GENERIC_READ | GENERIC_WRITE, // desired access should be read&write  
			0,                          // COM port must be opened in non-sharing mode  
			NULL,                       // don't care about the security  
			OPEN_EXISTING,              // IMPORTANT: must use OPEN_EXISTING for a COM port  
			0,                          // usually overlapped but non-overlapped for existance test  
			NULL);                      // always NULL for a general purpose COM port  
		if (COMPort != INVALID_HANDLE_VALUE && GetLastError() != ERROR_ACCESS_DENIED)
		{
			// COM port exist
			AvaliablePort->push_back(PortName);
			CloseHandle(COMPort);
		}
	}
	return AvaliablePort;
}

bool COM::COMPortReady(){
	//�������� ����� 
	COMPort = CreateFile(COMName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (COMPort == INVALID_HANDLE_VALUE) {
		COMPort = NULL;
		return false;
	}
	//��������� ��������� �������� ������ 
	COMMTIMEOUTS commtimouts;
	commtimouts.ReadIntervalTimeout = 0;
	commtimouts.ReadTotalTimeoutMultiplier = 0;
	commtimouts.ReadTotalTimeoutConstant = 0;
	commtimouts.WriteTotalTimeoutMultiplier = 0;
	commtimouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(COMPort, &commtimouts)) {
		//������
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	//��������� �������� �������� ����� � ��������
	if (!SetupComm(COMPort, 512, 512)) {
		//������
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	//������� ������������ ������ ����� 
	if (!PurgeComm(COMPort, PURGE_RXCLEAR)) {
		//������
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	return true;
}

bool COM::DCBReady(DWORD baudRate, BYTE parity){
	//���������� ��������� DCB 
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(COMPort, &dcb)) {
		//wprintf(L"DCB read error!!!\n\r");
		return false;
	}
	//��������� ���������� ������ ������� 
	dcb.DCBlength = sizeof(DCB);
	dcb.fBinary = TRUE;
	dcb.BaudRate = baudRate;
	dcb.ByteSize = 8;
	dcb.Parity = parity;
	dcb.StopBits = ONESTOPBIT;
	//��������� ��������� DCB 
	if (!SetCommState(COMPort, &dcb)) {
		//printf("DCB write error!!!\n\r");
		return false;
	}
	return true;
}

bool COM::COMPortClose(){
	//�������� �����
	if (CloseHandle(COMPort)){
		COMPort = NULL;
		return true;
	}
	else return false;
}

uint16_t CRC16(uint16_t oldcrc, uint8_t *data, int length)
{
	uint16_t crc = oldcrc;
	for (int i = 0; i < length; i++){
		crc ^= *data++;
		for (int j = 0; j < 8; ++j)
		{
			if (crc & 1) crc = (crc >> 1) ^ 0xA001;
			else crc = (crc >> 1);
		}
	}
	unsigned char hiByte = LOBYTE(crc);
	unsigned char loByte = HIBYTE(crc);
	return MAKEWORD(hiByte, loByte);
}

vector<unsigned char> CreateRequest(unsigned char node, unsigned char command, unsigned short address, unsigned short size) {
	vector<unsigned char> data;
	//������ ���� - ����� ���������� 
	data.push_back(node);
	//������ ���� - ������� 
	data.push_back(command);
	//������ � ��������� ����� - ����� ��������� 
	data.push_back(HIBYTE(address));
	data.push_back(LOBYTE(address));
	//����� � ������ ����� - ������ ������� � ������ 
	data.push_back(HIBYTE(size));
	data.push_back(LOBYTE(size));
	//������� ����������� ����� CRC16 
	unsigned short crc16 = CRC16(0xFFFF, data.data(), data.size());
	//������� � ������� ����� - ����������� ����� CRC16 
	data.push_back(LOBYTE(crc16));
	data.push_back(HIBYTE(crc16));
	return data;
}

bool CheckResponse(unsigned char node, std::vector<unsigned char> &data) {
	unsigned int size(data.size());
	unsigned int index(0);
	unsigned char symbol;
	unsigned short crc16(0xFFFF);
	if (index >= size) return false;
	//�������� ���������� ������� ���������� ������� � ������ 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (symbol != node) return false;
	if (index >= size) return false;
	//�������� ���������� ������� ������� � ������ 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (symbol != 3) return false;
	if (index >= size) return false;
	//����������� ������� ��������� ������ 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (index >= size) return false;
	unsigned short dataSize = symbol;
	for (unsigned short i = 0; i < dataSize; ++i) {
		symbol = data[index++];
		crc16 = CRC16(crc16, &symbol, 1);
		if (index >= size) return false;
	}
	//�������� ����������� ����� 
	if (++index >= size) return false;
	unsigned short crc16FromResponse = MAKEWORD(data[index - 1], data[index]);
	if (crc16FromResponse != crc16) return false;
	return true;

}

unsigned int GetSleepTimeout(unsigned int length, DWORD baudRate) {
	long sleepTime(1);
	//����� �������� ��� ��������� ���� 19200 ���/� (� �������������) 
	if (baudRate > CBR_19200) sleepTime = long(1.75 + 1000. * double(length) * 11. / double(baudRate) + 0.5);
	//����� �������� ��� ��������� ���� 19200 ���/� (� �������������) 
	else sleepTime = long(1000. * (3.5 + double(length)) * 11. / double(baudRate) + 0.5);
	if (sleepTime < 1) sleepTime = 1; return sleepTime;
}

double COM::GetTime() {
	LARGE_INTEGER frequency, counter;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);
	return double(counter.QuadPart) / double(frequency.QuadPart);
}

vector<unsigned char> COM::Stream(vector<unsigned char>* requestData, DWORD baudRate){
	vector<unsigned char> responseData;
	vector<unsigned char> clearData;
	//������� ������� � ���� 
	DWORD bytesWritten(0);
	BOOL bStatus = WriteFile(COMPort, requestData->data(), requestData->size(), &bytesWritten, NULL);
	if (bStatus == TRUE && requestData->size() == bytesWritten) {
		//�������� ����������� �������
		long sleepTime4Request = GetSleepTimeout(requestData->size(), baudRate);
		double tTime = GetTime();
		while (sleepTime4Request > (GetTime() - tTime) * 1000) Sleep(1);
		long sleepTimeAfterResponse = GetSleepTimeout(4, baudRate) + 50;
		tTime = GetTime();
		//�������� ������ �� ����������
		while (1) {
			//������ ��������� �����
			COMSTAT comstat;
			bStatus = ClearCommError(COMPort, NULL, &comstat);
			if (bStatus != TRUE) break;
			//�������� ������� ���� � ������ ������ �����
			if (comstat.cbInQue > 0) {
				//������ ���� �� ������ ������ �����
				tTime = GetTime();
				std::vector<unsigned char> data(comstat.cbInQue);
				DWORD bytesRead(0);
				bStatus = ReadFile(COMPort, data.data(), comstat.cbInQue, &bytesRead, NULL);
				if (bStatus != TRUE || comstat.cbInQue != bytesRead) break;
				responseData.insert(responseData.end(), data.begin(), data.end());
				//�������� ������������ ������ �� ����������
				if (CheckResponse(requestData->at(0), responseData)) {
					clearData.assign(responseData.begin()+3, responseData.end()-2);
					//�������� ������� �� ������������ ���������� �������
					//Sleep(sleepTimeAfterResponse);
					break;
				}
			}
			else {
				//�������� ���������� ������� �������� ������ �� ����������
				if ((GetTime() - tTime) * 1000 > sleepTimeAfterResponse) {
					break;
				}
			}
		}
	}
	return clearData;
}

bool CheckZET7052(vector<unsigned char> data){
	if (data[0] == data[2] == data[3] == 0 && data[1] == 0x46)
		return true;
	else return false;
}
vector<Device>* COM::DeviceScan(){
	vector<Device>* ConnectedDevices = new vector<Device>;
	if (!COMPortReady()){
		return NULL;
	}
	//������ �� ���� ��������� 
	unsigned short baudRateIndex(0);
	while (baudRateIndex < baudRateCount) {
		//������ �� ���� ��������� ���� �������� 
		unsigned short parityIndex(0);
		while (parityIndex < parityCount) {
			if (!DCBReady(baudRates[baudRateIndex], parities[parityIndex])){
				return NULL;
			}
			//������ �� ���� ��������� ������� ��������� 
			///////////////////////////////////////////////////////////////////
			for (unsigned char node = 2; node <= 64; ++node){
			//������������ �������
			vector<unsigned char> requestData = CreateRequest(node, 3, 4, 2);
			vector<unsigned char> getData = Stream(&requestData, baudRates[baudRateIndex]);
			if (getData.size() && CheckZET7052(getData))
			ConnectedDevices->push_back(Device(node, baudRates[baudRateIndex], parities[parityIndex]));
			}
			//////////////////////////////////////////////////////////////////////////////////////////
			//ConnectedDevices->push_back(Device(6, 19200, 0));
			///////////////////////////////////////////////////////////////////////////////////////////
			++parityIndex;
		}
		++baudRateIndex;
	}
	if (COMPortClose())
		return ConnectedDevices;
}

float COM::GetAxis(vector<unsigned char> request, DWORD baudRate){
	vector<unsigned char> axis;
	axis = Stream(&request, baudRate);
	if (axis.size() == 4){
		uint8_t data[4] = { axis[1], axis[0], axis[3], axis[2] };
		float* z = (float*)data;
		return *z;
	}
	else return NULL;
}

Measure COM::GetMeasure(Device Dev){
	Measure data;
	vector<unsigned char> request;
	if (DCBReady(Dev.BaudRate, Dev.Parity)){
		///////////////////////////////////////////////
		//double t1 = GetTime();
		//data.X = t1;
		//data.Y = t1;
		//data.Z = t1;
		//////////////////////////////////////////////
		request = CreateRequest(Dev.Channel, 3, 20, 2);
		data.X = GetAxis(request, Dev.BaudRate);
		request = CreateRequest(Dev.Channel, 3, 58, 2);
		data.Y = GetAxis(request, Dev.BaudRate);
		request = CreateRequest(Dev.Channel, 3, 96, 2);
		data.Z = GetAxis(request, Dev.BaudRate);
		//////////////////////////////////////////////
		return data;
		///////////////////////////////////////////////
	}
}

