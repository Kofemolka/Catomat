<?php
	define("ACTION", 'action');

	define("STATS_FILE", './data/stats.txt');
	define("STATS_DEL", ';');
	define("FEED_ACTION", 'feed');
	define("PUMP_ACTION", 'pump');
	define("VISIT_ACTION", 'visit');
	define("MANUAL_ACTION", 'manual');
	define("AUTO_ACTION", 'auto');
	define("MODE_ACTION", 'mode');
	define("CLEAR_ACTION", 'clear');
	define("MODE", 'mode');
	define("MODE_MANUAL", 'M');
	define("MODE_AUTO", 'A');
	define("DEVICE_MODE", 'devmode');
	define("DATA_FOLDER", './data/');
	//////////////////////////////////////
	error_reporting(E_ERROR | E_PARSE);	

	validateUser();

	include('kvstore.php');

	$action = $_GET[constant("ACTION")];
		
	switch ($action) {
		case constant("FEED_ACTION"):
		case constant("PUMP_ACTION"):
			saveRequest($action);				
			break;		
			
		case constant("MODE_ACTION"):
			setMode($_GET[constant("MODE")]);
			break;

		case constant("CLEAR_ACTION"):
			clearLogs();
			break;

		case "query":
			query();
			break;

		case "update":
			update($_GET['event'], $_GET[constant("MODE")]);
			echo "OK";
			die;
			break;
	}			
		
	/////////////////////////////////////
		
	function validateUser()
	{
		$valid_passwords = array ("!!!"=>"!!!");
		$valid_users = array_keys($valid_passwords);

		list($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']) = explode(':' , base64_decode(substr($_SERVER['HTTP_AUTHORIZATION'], 6)));

		$user = $_SERVER['PHP_AUTH_USER'];
		$pass = $_SERVER['PHP_AUTH_PW'];		

		$validated = (in_array($user,$valid_users)) && ($pass == $valid_passwords[$user]);

		if (!$validated) {
		  header('WWW-Authenticate: Basic realm="Catville"');
		  header('HTTP/1.0 401 Unauthorized');		  

		  die ("Not authorized");
		}
	}

	function clearLogs()
	{
		try
		{
			unlink(constant("STATS_FILE"));
		}
		catch (Exception $e) {}
	}

	function setMode($mode)
	{
		KeyValueStore::Set(constant("MODE_ACTION"), $mode);		
	}

	function query()
	{
		if(KeyValueStore::Get(constant("FEED_ACTION")))
		{			
			echo "[action]={".constant("FEED_ACTION")."}";
			KeyValueStore::Clear(constant("FEED_ACTION"));	
			die;
		}

		if(KeyValueStore::Get(constant("PUMP_ACTION")))
		{			
			echo "[action]={".constant("PUMP_ACTION")."}";
			KeyValueStore::Clear(constant("PUMP_ACTION"));	
			die;
		}

		if(KeyValueStore::Get(constant("MODE_ACTION")))
		{			
			$mode = KeyValueStore::Get(constant("MODE_ACTION"));
			echo "[action]={";

			if($mode == constant("MODE_MANUAL"))
			{
				echo constant("MANUAL_ACTION");
			}
			else if($mode == constant("MODE_AUTO"))
			{
				echo constant("AUTO_ACTION");
			}

			echo "}";

			KeyValueStore::Clear(constant("MODE_ACTION"));	

			die;
		}

		die;
	}

	function saveRequest($type)
	{	
		KeyValueStore::Set($type, "1");		
	}

	function update($event, $data = null)
	{		
		if(strcmp($event, constant("FEED_ACTION")) == 0)
		{
					
		}
		else if(strcmp($event, constant("PUMP_ACTION")) == 0)
		{
					  
		}
		else if(strcmp($event, constant("VISIT_ACTION")) == 0)
		{
					  
		}
		else if(strcmp($event, constant("MODE_ACTION")) == 0)
		{					
			KeyValueStore::Set(constant("DEVICE_MODE"), $data);
			
			return;		   
		}
		else
		{
			return;
		}

		$txt = $event.constant("STATS_DEL").date("r");
		$stats = file_put_contents(constant("STATS_FILE"), $txt.PHP_EOL, FILE_APPEND);		
	}	

	function isAutoMode()
	{
		$mode = KeyValueStore::Get(constant("DEVICE_MODE"));

		if(!is_null($mode) && strcmp($mode, constant("MODE_AUTO")) == 0)
		{
			return true;		
		}
		
		return false;
	}
?>

<!DOCTYPE html>


<html>
<head>
    <meta content="text/html;charset=utf-8" http-equiv="Content-Type">
    <meta content="utf-8" http-equiv="encoding">    
    <title>CatoMat</title>
	<link rel="stylesheet" type="text/css" href="style.css">	
	<meta http-equiv="refresh" content="15;URL='<?php echo $_SERVER['PHP_SELF']?>'">
</head>
<body>
    <div>
	<h1><?php echo date("r"); ?></h1>

	<table class="btns">
		<tr>
			 <td>
		 		<form action="
					<?php echo "http://".$_SERVER['HTTP_HOST']; ?>" method="get">
					<input class="btn_food" type="submit" value="">					
					<input type="hidden" name="<?php echo constant("ACTION"); ?>" value="<? echo constant("FEED_ACTION"); ?>" />		
				</form>	
			</td>

			<td>
				<form action="
					<?php echo "http://".$_SERVER['HTTP_HOST']; ?>" method="get">
					<input class="btn_water" type="submit" value="">
					<input type="hidden" name="<?php echo constant("ACTION"); ?>" value="<? echo constant("PUMP_ACTION"); ?>" />		
				</form>	
			</td>

			<td>
		 		<form action="
					<?php echo "http://".$_SERVER['HTTP_HOST']; ?>" method="get">
					<input type="hidden" name="<?php echo constant("ACTION"); ?>" value="<? echo constant("MODE_ACTION"); ?>" />	

					<?php
					if(isAutoMode())
					{
						echo '<input class="btn_manual" type="submit" value=""/>';
						echo '<input type="hidden" name="mode"; value="M"/>';
					}
					else
					{
						echo '<input class="btn_auto" type="submit" value=""/>';
						echo '<input type="hidden" name="mode"; value="A"/>';
					}
					?>
				</form>	
			</td>
		</tr>
	</table>	

	<?php include('stats.php'); ?>
	 
	</div>	
</body>
</html>

