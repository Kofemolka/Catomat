<?php
	function nameDelay($when, $from)
	{
		$doPlural = function($nb,$str)
		{
			$plural = $nb>1?$str.'s':$str;
			return $nb.$plural;
		};

		$interval = $from->diff($when);		

		if($interval->days > 0)
		{
			return $doPlural($interval->days, " day")." ago";
		}

		if($interval->h > 0)
		{
			return $doPlural( $interval->h, " hour")." ago";
		}

		if($interval->i > 0)
		{
			return $doPlural($interval->i, " minute")." ago";
		}

		return "Less than minute ago";
	}

	function event2img($event)
	{
		switch($event)
		{
			case constant("FEED_ACTION"):
				return "./img/bowl_s.png";

			case constant("PUMP_ACTION"):
				return "./img/drop_s.png";	
        
      case constant("VISIT_ACTION"):
				return "./img/cat_s.png";	
		}

		return "";
	}
?>

<table class="stats">
	<tr>
		<td style="width:100px">What</td>
		<td style="width:auto">When</td>
	</tr>
	<?php
		$handle = @fopen(constant("STATS_FILE"), "r");
		if ($handle) {

			$records = Array();

			while (($buffer = fgets($handle, 4096)) !== false)
			{
				$record = explode(constant("STATS_DEL"), $buffer);	

				array_push($records, 
					$record
				);
			}

			if (!feof($handle)) {
				echo "Error: unexpected fgets() fail\n";
			}
			fclose($handle);

			$ordered_records = array_reverse($records);

			$now = new DateTime();

			foreach ($ordered_records as $record)
			{
				echo "<tr>";
				echo "<td align=\"center\"><img src=\"".event2img($record[0])."\"/></td>";
				echo "<td>".nameDelay(new DateTime($record[1]),$now)."</td>";				
				echo "</tr>";
			}
		}
	?>
	</table>
	
	<br/>

	<table class="btns">
		<tr>
			 <td>
		 		<form action="
					<?php echo "http://".$_SERVER['HTTP_HOST']; ?>" method="get">
					<input class="btn" type="submit" value="Clear">
					<input type="hidden" name="<?php echo constant("ACTION"); ?>" value="<? echo constant("CLEAR_ACTION"); ?>" />						
				</form>	
			</td>
		</tr>
	</table>