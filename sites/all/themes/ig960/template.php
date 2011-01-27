<?php
//dpm(menu_secondary_links());
/**
* Implementation of hook_menu_alter().
*/
/*
function ig960_menu_alter(&$items) {
  dpm($items);
$items[] = array(
  'data' => l(t('View cart'), 'cart', array('attributes' => array('rel' => 'nofollow'))),
  'class' => 'cart-block-view-cart'
);
    // Only add the checkout link if checkout is enabled.
    if (variable_get('uc_checkout_enabled', TRUE)) {
      $items[] = array( 
        'data' => l(t('Checkout'), 'cart/checkout', array('attributes' => array('rel' => 'nofollow'))),
        'class' => 'cart-block-checkout'
      );
    }
}
*/
/*
function ig960_uc_ajax_cart_cart_links() {
  $items[] = array(
    'data' => l(t('Printable order form'), url('sites/default/files/assets/files/Order form 100601.01.pdf', array('absolute'=>true)), array('attributes' => array('rel' => 'nofollow', 'target'=>'_blank', 'title'=>'If you prefer not to use the web site to place your order, you can print out and mail or fax this order form (Adobe Reader required)'))),
    'class' => 'cart-block-printable-order-form'
  );
$items[] = array(
  'data' => l(t('View cart'), 'cart', array('attributes' => array('rel' => 'nofollow'))),
  'class' => 'cart-block-view-cart'
);
    // Only add the checkout link if checkout is enabled.
    if (variable_get('uc_checkout_enabled', TRUE)) {
      $items[] = array( 
        'data' => l(t('Checkout'), 'cart/checkout', array('attributes' => array('rel' => 'nofollow'))),
        'class' => 'cart-block-checkout'
      );
    }
  return theme('item_list', $items, NULL, 'ul', array('class' => 'links'));
}
*/

function ig960_preprocess_node(&$vars, $hook) {
  if($vars['field_product_reference'][0]['nid']) {
    $vars['node']->product_node = node_load($vars['field_product_reference'][0]['nid']);
     if ($vars['node']->product_node->field_image[0]['filepath'] && file_exists($vars['node']->product_node->field_image[0]['filepath'])) {
       $vars['node']->product_picture['view'] = theme('imagecache', 'product_list', $vars['node']->product_node->field_image[0]['filepath']);
       $vars['node']->product_picture['filepath'] = imagecache_create_url('product_list', $vars['node']->product_node->field_image[0]['filepath']);
     }
  }
  
  $vars['blog_region'] = theme('blocks', 'blog_region');
  $vars['endorsements'] = theme('blocks', 'endorsements');
  $vars['shopping_cart'] = theme('blocks', 'shopping_cart');
}

/*
function ig960_uc_cart_block_summary($item_count, $item_text, $total, $summary_links) {
  $context = array(
    'revision' => 'themed-original',
    'type' => 'amount',
  );
  // Build the basic table with the number of items in the cart and total.
  $output = '<table class="cart-block-summary"><tbody><tr>'
           .'<td class="cart-block-summary-items">'. $item_text .'</td>'
           .'<td class="cart-block-summary-total"><label>'. t('Total:')
           .'</label> '. uc_price($total, $context) .'</td></tr>';

  // If there are products in the cart...
  if ($item_count > 0) {
    // Add a view cart link.
    $output .= '<tr class="cart-block-summary-links"><td colspan="2">'
             . theme('links', $summary_links) .'</td></tr>';
  }

  $output .= '</tbody></table>';

  return $output;
}
*/

function ig960_uc_cart_block_content($help_text, $items, $item_count, $item_text, $total, $summary_links) {
  $output = '';

  // Add the help text if enabled.
  if ($help_text) {
    $output .= '<span class="cart-help-text">'. $help_text .'</span>';
  }

  // Add a wrapper div for use when collapsing the block.
  $output .= '<div id="cart-block-contents">';

  // Add a table of items in the cart or the empty message.
  $output .= theme('uc_cart_block_items', $items);

// THIS IS MY NEW FUNCTION TO GET THE TOTAL DISCOUNTS
  $discount_info = uc_discounts_cart_block_discounts(uc_cart_get_contents());
if ($discount_info['total']) {
  //$output .= $discount_info['body'];
  $output .= "<div class=\"cart-block-discounts-total\"><label>Total discounts:</label><span class=\"uc_price\">" . uc_currency_format($discount_info['total']) . "</span></div>";
}
  $output .= '</div>';


  // Add the summary section beneath the items table.
  $output .= theme('uc_cart_block_summary', $item_count, $item_text, $total - $discount_info['total'], $summary_links);

  return $output;
}


/**
 * Theme the summary table at the bottom of the default shopping cart block.
 *
 * @param $item_count
 *   The number of items in the shopping cart.
 * @param $item_text
 *   A textual representation of the number of items in the shopping cart.
 * @param $total
 *   The unformatted total of all the products in the shopping cart.
 * @param $summary_links
 *   An array of links used in the summary.
 * @ingroup themeable
 */
function ig960_uc_cart_block_summary($item_count, $item_text, $total, $summary_links) {
  $context = array(
    'revision' => 'themed-original',
    'type' => 'amount',
  );
  // Build the basic table with the number of items in the cart and total.
  $output = '<table class="cart-block-summary"><tbody><tr>'
           .'<td class="cart-block-summary-items">'. $item_text .'</td>'
           .'<td class="cart-block-summary-total"><label>'. t('Total:')
           .'</label> '. uc_price($total, $context) .'</td></tr>';

  // If there are products in the cart...
  if ($item_count > 0) {
    // Add a view cart link.
    //    $output .= '<tr class="cart-block-summary-links"><td colspan="2">' . theme('links', $summary_links) .'</td></tr>';
       $output .= '<tr class="cart-block-summary-links"><td colspan="2"><div class="clearfix">';
       if(is_array($summary_links)) {
         foreach($summary_links as $key =>$link) {
           $output .= '<FORM METHOD="LINK" ACTION="/'.$link['href'].'" class="'.$key.'"><INPUT TYPE="submit" class="form-submit" VALUE="'.$link['title'].'"></FORM>';
         }
       }
       $output .= '</div></td></tr>';
  }

  $output .= '</tbody></table>';

  return $output;
}
