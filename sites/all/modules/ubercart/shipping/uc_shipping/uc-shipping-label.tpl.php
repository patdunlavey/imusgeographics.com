<?php
// $Id: uc-shipping-label.tpl.php,v 1.1.2.1 2010/01/27 22:24:45 islandusurper Exp $

/**
 * @file
 * The shipment mailing label template.
 */
?>
<style>
.shipment-label img {display:block;}
</style>
<div class="shipment-label" style="float:left; margin:30px;">

  <table id="Table_01" width="432" height="298" border="0" cellpadding="0" cellspacing="0">
  	<tr>
  		<td colspan="3">
  			<img src="/<?php print $directory; ?>/images/shiplabel_01.jpg" width="432" height="97" alt=""></td>
  	</tr>
  	<tr>
  		<td>
  			<img src="/<?php print $directory; ?>/images/shiplabel_02.jpg" width="110" height="148" alt=""></td>
  		<td width="281"><?php print $shipping_address; ?>
  			</td>
  		<td>
  			<img src="/<?php print $directory; ?>/images/shiplabel_04.jpg" width="41" height="148" alt=""></td>
  	</tr>
  	<tr>
  		<td colspan="3">
  			<img src="/<?php print $directory; ?>/images/shiplabel_05.jpg" width="432" height="53" alt=""></td>
  	</tr>
  </table>
</div>
