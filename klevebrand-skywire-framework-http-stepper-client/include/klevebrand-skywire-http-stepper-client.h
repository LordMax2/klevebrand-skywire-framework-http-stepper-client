#ifndef KLEVEBRAND_SKYWIRE_HTTP_STEPPER_CLIENT_H
#define KLEVEBRAND_SKYWIRE_HTTP_STEPPER_CLIENT_H

#include "Arduino.h"
#include "klevebrand-skywire-framework.h"

class SkywireHttpStepperClient
{
public:
	SkywireHttpStepperClient(HardwareSerial& skywire_serial, String base_url, int port, bool debug_mode = false) : _skywire(skywire_serial, debug_mode)
	{
		this->_base_url = base_url;
		this->_port = port;
		this->_debug_mode = debug_mode;

		resetState();
	}
	bool start();
	SkywireResponseResult_t get(String path);

private:
	Skywire _skywire;
	String _base_url;
	int _port;

	bool _debug_mode = false;

	bool httpCfg();
	bool _http_cfg_sent = false;
	bool _http_cfg_ok_recieved = false;

	bool httpQry(String path);
	bool _http_qry_sent = false;
	bool _http_qry_ok_recieved = false;

	bool httpRcv();
	bool _http_rcv_sent = false;
	bool _http_rcv_ok_recieved = false;

	bool httpRing();
	bool _http_ring_recieved = false;

	void resetState();
	void serialReadToRxBuffer();

	void resetRxBuffer();
	String _rx_buffer;
};

#endif // KLEVEBRAND_SKYWIRE_HTTP_STEPPER_CLIENT_H
