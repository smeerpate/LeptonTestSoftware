#include "Lepton_I2C.h"

#include <LEPTON_SDK.h>
#include <LEPTON_SYS.h>
#include <LEPTON_Types.h>

bool _connected;

LEP_CAMERA_PORT_DESC_T _port;

int lepton_connect()
{
	LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	_connected = true;
	return 0;
}

void lepton_perform_ffc()
{
	if(!_connected) {
		lepton_connect();
	}
	LEP_RunSysFFCNormalization(&_port);
}

float lepton_get_fpaTemp() 
{
    LEP_SYS_FPA_TEMPERATURE_CELCIUS_T sFpaTemp;
    
	if(!_connected) {
		lepton_connect();
	}
	LEP_GetSysFpaTemperatureCelcius(&_port, &sFpaTemp);
    return (float)sFpaTemp;
}