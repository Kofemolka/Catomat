#pragma once

class SerialCommand
{
private:
	String _empty;
	String _cmd;
	Stream& _stream;
	bool _complete;

public:
	SerialCommand(Stream& stream) : _stream(stream)
	{
		_empty = "";
		_cmd.reserve(100);		
		_complete = false;
	}

	String GetCommand()
	{
		if (_complete)
		{
			_cmd = "";
			_complete = false;
		}

		while (_stream.available() > 0)
		{
			char c = (char)_stream.read();

			_cmd += c;
			delay(3);

			if (c == '\n')
			{				
				_complete = true;
				return _cmd;
			}
		}

		return _empty;
	}
};