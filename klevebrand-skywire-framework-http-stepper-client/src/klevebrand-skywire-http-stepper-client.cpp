#include "klevebrand-skywire-http-stepper-client.h"

void SkywireHttpStepperClient::resetState()
{
	_http_cfg_ok_recieved = false;
	_http_cfg_sent = false;
	_http_qry_ok_recieved = false;
	_http_qry_sent = false;
	_http_ring_recieved = false;
	_http_rcv_sent = false;

	resetRxBuffer();
}

void SkywireHttpStepperClient::resetRxBuffer()
{
	_rx_buffer = "";
}

bool SkywireHttpStepperClient::start()
{
	_skywire.start();
	return true;
}

SkywireResponseResult_t SkywireHttpStepperClient::get(String path)
{
	if (!httpCfg())
	{
		return SkywireResponseResult_t(false, "");
	}

	if (!httpQry(path))
	{
		return SkywireResponseResult_t(false, "");
	}

	if (!httpRing())
	{
		return SkywireResponseResult_t(false, "");
	}

	if (!httpRcv())
	{
		return SkywireResponseResult_t(false, "");
	}

	serialReadToRxBuffer();

	if (_rx_buffer.indexOf("\r\n") == -1 || _skywire.available() || _rx_buffer.indexOf("OK") == -1)
	{
		return SkywireResponseResult_t(false, "");
	}

	String result = _rx_buffer;

	resetState();

	_skywire.print("AT#SH=1\r");

	return SkywireResponseResult_t(true, result);
}

bool SkywireHttpStepperClient::httpCfg()
{
	if (!_http_cfg_sent)
	{
		_skywire.print("AT#HTTPCFG=0,\"" + _base_url + "\"\r");

		_http_cfg_sent = true;

		return false;
	}

	serialReadToRxBuffer();

	if (_http_cfg_sent && !_http_cfg_ok_recieved)
	{
		_http_cfg_ok_recieved = _rx_buffer.indexOf("OK") != -1 && _rx_buffer.indexOf("\r\n") != -1 && !_skywire.available();

		if (_http_cfg_ok_recieved)
		{
			if(_debug_mode) {
				Serial.println("STEPPER CLIENT RECEIVED HTTPCFG OK");
			}

			_rx_buffer = "";

			return true;
		}

		return false;
	}

	return true;
}

bool SkywireHttpStepperClient::httpQry(String path)
{
	if (!_http_qry_sent)
	{
		if(_debug_mode) {
			Serial.println("Sending QRY");
		}

		_http_qry_sent = _skywire.print("AT#HTTPQRY=0,0,\"/" + path + "\"\r");

		return false;
	}

	serialReadToRxBuffer();

	if (!_http_qry_ok_recieved)
	{
		_http_qry_ok_recieved = _rx_buffer.indexOf("OK") != -1 && _rx_buffer.indexOf("\r\n") != -1 && !_skywire.available();

		if (_http_qry_ok_recieved)
		{
			if(_debug_mode) {
				Serial.println("STEPPER CLIENT RECEIVED HTTPQRY OK");
			}

			_rx_buffer = "";

			return true;
		}

		return false;
	}

	return true;
}

bool SkywireHttpStepperClient::httpRing()
{
	serialReadToRxBuffer();

	if (!_http_ring_recieved)
	{
		
		_http_ring_recieved = _rx_buffer.indexOf("HTTPRING") != -1 && _rx_buffer.indexOf("\r\n") != -1 && !_skywire.available();

		if (_http_ring_recieved)
		{
			if(_debug_mode) {
				Serial.println(_rx_buffer);
				Serial.println("STEPPER CLIENT RECEIVED HTTPRING OK");
			}

			_rx_buffer = "";

			return true;
		}

		return false;
	}

	return true;
}

bool SkywireHttpStepperClient::httpRcv()
{
	if (!_http_rcv_sent)
	{
		_http_rcv_sent = _skywire.print("AT#HTTPRCV=0\r");

		return false;
	}

	return true;
}

void SkywireHttpStepperClient::serialReadToRxBuffer()
{
	if (_skywire.available())
	{
		char c = _skywire.read();
		_rx_buffer += c;
	}
}
