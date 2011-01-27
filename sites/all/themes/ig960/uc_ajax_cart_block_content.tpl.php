<?php
// $Id: uc_ajax_cart_block_content.tpl.php,v 1.1.2.8 2010/05/01 12:42:30 erikseifert Exp $
?>
<?php // dpm($items); ?>
<?php
  if(isset($items[0])) {
    $uc_qty = 0;
    foreach($items as $item) {
      $uc_qty += $item->item['qty'];
    }
  }
?>
<div class="cart-block-summary-links">
  <?php // print "(".$uc_qty.")";
  ?>
			<?php print $cart_links; ?>
</div>