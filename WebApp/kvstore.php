<?php

class KeyValueStore
{
	const DATA = './data/';

	public static function Get($key)
	{
		if(file_exists(self::DATA.$key))
		{			
			return file_get_contents(self::DATA.$key);
		}

		return NULL;
	}

	public static function Set($key, $value)
	{
		file_put_contents(self::DATA.$key,  $value);
	}

	public static function Clear($key)
	{
		unlink(self::DATA.$key);
	}
}

?>