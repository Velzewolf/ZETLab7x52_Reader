#include <string>
#include "cstdint"
#include "COM.h"

//скорости, по которым будет проходить программа
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

//проверки бита четности, по которым будет проходить программа 
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
	//открытие порта 
	COMPort = CreateFile(COMName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (COMPort == INVALID_HANDLE_VALUE) {
		COMPort = NULL;
		return false;
	}
	//установка таймаутов передачи данных 
	COMMTIMEOUTS commtimouts;
	commtimouts.ReadIntervalTimeout = 0;
	commtimouts.ReadTotalTimeoutMultiplier = 0;
	commtimouts.ReadTotalTimeoutConstant = 0;
	commtimouts.WriteTotalTimeoutMultiplier = 0;
	commtimouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(COMPort, &commtimouts)) {
		//Ошибка
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	//установка размеров очередей приёма и передачи
	if (!SetupComm(COMPort, 512, 512)) {
		//Ошибка
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	//очистка принимающего буфера порта 
	if (!PurgeComm(COMPort, PURGE_RXCLEAR)) {
		//Ошибка
		CloseHandle(COMPort);
		COMPort = NULL;
		return false;
	}
	return true;
}

bool COM::DCBReady(DWORD baudRate, BYTE parity){
	//считывание структуры DCB 
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(COMPort, &dcb)) {
		//wprintf(L"DCB read error!!!\n\r");
		return false;
	}
	//установка параметров обмена данными 
	dcb.DCBlength = sizeof(DCB);
	dcb.fBinary = TRUE;
	dcb.BaudRate = baudRate;
	dcb.ByteSize = 8;
	dcb.Parity = parity;
	dcb.StopBits = ONESTOPBIT;
	//установка структуры DCB 
	if (!SetCommState(COMPort, &dcb)) {
		//printf("DCB write error!!!\n\r");
		return false;
	}
	return true;
}

bool COM::COMPortClose(){
	//закрытие порта
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
	//первый байт - адрес устройства 
	data.push_back(node);
	//второй байт - команда 
	data.push_back(command);
	//третий и четвертый байты - адрес обращения 
	data.push_back(HIBYTE(address));
	data.push_back(LOBYTE(address));
	//пятый и шестой байты - размер запроса в словах 
	data.push_back(HIBYTE(size));
	data.push_back(LOBYTE(size));
	//рассчет контрольной суммы CRC16 
	unsigned short crc16 = CRC16(0xFFFF, data.data(), data.size());
	//седьмой и восьмой байты - контрольная сумма CRC16 
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
	//проверка совпадения адресов устройства запроса и ответа 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (symbol != node) return false;
	if (index >= size) return false;
	//проверка совпадения команды запроса и ответа 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (symbol != 3) return false;
	if (index >= size) return false;
	//определения размера пришедших данных 
	symbol = data[index++];
	crc16 = CRC16(crc16, &symbol, 1);
	if (index >= size) return false;
	unsigned short dataSize = symbol;
	for (unsigned short i = 0; i < dataSize; ++i) {
		symbol = data[index++];
		crc16 = CRC16(crc16, &symbol, 1);
		if (index >= size) return false;
	}
	//проверка контрольной суммы 
	if (++index >= size) return false;
	unsigned short crc16FromResponse = MAKEWORD(data[index - 1], data[index]);
	if (crc16FromResponse != crc16) return false;
	return true;

}

unsigned int GetSleepTimeout(unsigned int length, DWORD baudRate) {
	long sleepTime(1);
	//время ожидания при скоростях выше 19200 бит/с (в миллисекундах) 
	if (baudRate > CBR_19200) sleepTime = long(1.75 + 1000. * double(length) * 11. / double(baudRate) + 0.5);
	//время ожидания при скоростях ниже 19200 бит/с (в миллисекундах) 
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
	//посылка запроса в порт 
	DWORD bytesWritten(0);
	BOOL bStatus = WriteFile(COMPort, requestData->data(), requestData->size(), &bytesWritten, NULL);
	if (bStatus == TRUE && requestData->size() == bytesWritten) {
		//ожидание прохождения запроса
		long sleepTime4Request = GetSleepTimeout(requestData->size(), baudRate);
		double tTime = GetTime();
		while (sleepTime4Request > (GetTime() - tTime) * 1000) Sleep(1);
		long sleepTimeAfterResponse = GetSleepTimeout(4, baudRate) + 50;
		tTime = GetTime();
		//ожидание ответа от устройства
		while (1) {
			//запрос состояния порта
			COMSTAT comstat;
			bStatus = ClearCommError(COMPort, NULL, &comstat);
			if (bStatus != TRUE) break;
			//проверка наличия байт в буфере приема порта
			if (comstat.cbInQue > 0) {
				//чтение байт из буфера приема порта
				tTime = GetTime();
				std::vector<unsigned char> data(comstat.cbInQue);
				DWORD bytesRead(0);
				bStatus = ReadFile(COMPort, data.data(), comstat.cbInQue, &bytesRead, NULL);
				if (bStatus != TRUE || comstat.cbInQue != bytesRead) break;
				responseData.insert(responseData.end(), data.begin(), data.end());
				//проверка правильности ответа от устройства
				if (CheckResponse(requestData->at(0), responseData)) {
					clearData.assign(responseData.begin()+3, responseData.end()-2);
					//ожидание времени до формирования следующего запроса
					//Sleep(sleepTimeAfterResponse);
					break;
				}
			}
			else {
				//проверка превышения времени ожидания ответа от устройства
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
	//проход по всем скоростям 
	unsigned short baudRateIndex(0);
	while (baudRateIndex < baudRateCount) {
		//проход по всем проверкам бита четности 
		unsigned short parityIndex(0);
		while (parityIndex < parityCount) {
			if (!DCBReady(baudRates[baudRateIndex], parities[parityIndex])){
				return NULL;
			}
			//проход по всем возможным адресам устройств 
			///////////////////////////////////////////////////////////////////
			for (unsigned char node = 2; node <= 64; ++node){
			//формирование запроса
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

