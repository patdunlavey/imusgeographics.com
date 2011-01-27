<?php
// $Id$

/**
 * @file
 * Conditional Actions hooks for uc_discount.module.
 */

/**
 * Implementation of hook_ca_trigger().
 */
function uc_discount_ca_trigger() {
  $triggers = array();

  $triggers['calculate_order_discounts'] = array(
    '#title' => t('Calculate order discounts'),
    '#category' => t('Discount'),
    '#arguments' => array(
      'order' => array('#entity' => 'uc_order', '#title' => t('Order')),
      'account' => array('#entity' => 'user', '#title' => t('Customer account')),
    ),
  );
  $triggers['calculate_product_discounts'] = array(
    '#title' => t('Calculate product discounts'),
    '#category' => t('Discount'),
    '#arguments' => array(
      'product' => array('#entity' => 'node', '#title' => t('Product')),
      'account' => array('#entity' => 'user', '#title' => t('Current user')),
    ),
  );
  $triggers['calculate_line_item_discounts'] = array(
    '#title' => t('Calculate line item discounts'),
    '#category' => t('Discount'),
    '#arguments' => array(
      'line_item' => array('#entity' => 'uc_line_item', '#title' => t('Line item')),
      'account' => array('#entity' => 'user', '#title' => t('Customer account')),
    ),
  );
  $triggers['calculate_order_product_discounts'] = array(
    '#title' => t('Calculate product discounts on the order'),
    '#category' => t('Discount'),
    '#arguments' => array(
      'product' => array('#entity' => 'node', '#title' => t('Product')),
      'order' => array('#entity' => 'uc_order', '#title' => t('Order')),
    ),
  );
  return $triggers;
}

/**
 * Implementation of hook_ca_condition().
 */
function uc_discount_ca_condition() {
  $conditions = array();

  if (module_exists('taxonomy')) {
    $conditions['uc_discount_condition_node_term'] = array(
      '#title' => t('Node category'),
      '#description' => t('The node is in a taxonomy term.'),
      '#category' => t('Taxonomy'),
      '#callback' => 'uc_discount_condition_node_term',
      '#arguments' => array(
        'node' => array('#entity' => 'node', '#title' => t('Node')),
      ),
    );
  }

  $conditions['uc_discount_condition_node_type'] = array(
    '#title' => t('Node type'),
    '#description' => t('The node has a particular type.'),
    '#category' => t('Node'),
    '#callback' => 'uc_discount_condition_node_type',
    '#arguments' => array(
      'node' => array('#entity' => 'node', '#title' => t('Node')),
    ),
  );

  $conditions['uc_discount_condition_coupon_code'] = array(
    '#title' => t('Coupon entered'),
    '#description' => t('Customer has entered a valid coupon code at checkout.'),
    '#category' => t('Order'),
    '#callback' => 'uc_discount_condition_coupon_code',
    '#arguments' => array(
      'order' => array('#entity' => 'uc_order', '#title' => t('Order')),
    ),
  );

  $conditions['uc_discount_condition_shippable_product'] = array(
    '#title' => t('Shippable Product'),
    '#description' => t('The base product is shippable'),
    '#category' => t('Node'),
    '#callback' => 'uc_discount_condition_shippable_product',
    '#arguments' => array(
      'node' => array('#entity' => 'node', '#title' => t('Node')),
    ),
  );
  return $conditions;
}

function uc_discount_condition_node_term($node, $settings) {
  if (!isset($node->vid)) {
    // Assume current revision
    $node->vid = db_result(db_query("SELECT vid FROM {node} WHERE nid = %d", $node->nid));
  }
  $node_terms = array_keys(taxonomy_node_get_terms($node));

  // Convert tags into term ids.
  if (isset($settings['taxonomy']['tags'])) {
    $typed_input = $settings['taxonomy']['tags'];
    unset($settings['taxonomy']['tags']);

    foreach ($typed_input as $vid => $vid_value) {
      $typed_terms = drupal_explode_tags($vid_value);

      foreach ($typed_terms as $typed_term) {
        // See if the term exists in the chosen vocabulary
        // and return the tid.
        $possibilities = taxonomy_get_term_by_name($typed_term);
        $typed_term_tid = NULL; // tid match, if any.
        foreach ($possibilities as $possibility) {
          if ($possibility->vid == $vid) {
            $typed_term_tid = $possibility->tid;
            // Add found term id back to chosen terms.
            $settings['taxonomy'][$vid][] = $typed_term_tid;
            break;
          }
        }
      }
    }
  }

  foreach ($settings['taxonomy'] as $vid => $terms) {
    // Skip vocabularies with no chosen terms.
    if (empty($terms)) {
      continue;
    }
    if (!is_array($terms)) {
      $terms = array($terms);
    }

    // Get the chosen terms that the node actually has.
    $has_terms = array_intersect($terms, $node_terms);

    if ($settings['operator'] == 'AND' && $has_terms != $terms) {
      // Node must have all chosen terms to return TRUE.
      return FALSE;
    }
    elseif ($settings['operator'] == 'OR' && count($has_terms)) {
      // Node needs just one of the chosen terms to return TRUE.
      return TRUE;
    }
  }

  // If we get this far without returning, the final result depends on the
  // logic operator we have been using.
  if ($settings['operator'] == 'AND') {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

function uc_discount_condition_node_term_form($form_state, $settings = array()) {
  $form = array();

  $form['operator'] = array(
    '#type' => 'select',
    '#title' => t('Logic'),
    '#options' => array(
      'AND' => t('Node must have all chosen terms, even if it is not included in that vocabulary.'),
      'OR' => t('Node must have one of the chosen terms.'),
    ),
    '#default_value' => $settings['operator'],
  );

  $c = db_query("SELECT * FROM {vocabulary} ORDER BY weight, name");
  while ($vocabulary = db_fetch_object($c)) {
    if ($vocabulary->tags) {
      if ($vocabulary->help) {
        $help = $vocabulary->help;
      }
      else {
        $help = t('A comma-separated list of terms describing this content. Example: funny, bungee jumping, "Company, Inc.".');
      }
      $form['taxonomy']['tags'][$vocabulary->vid] = array(
        '#type' => 'textfield',
        '#title' => $vocabulary->name,
        '#description' => $help,
        '#required' => FALSE,
        '#default_value' => $settings['taxonomy']['tags'][$vocabulary->vid],
        '#autocomplete_path' => 'taxonomy/autocomplete/'. $vocabulary->vid,
        '#weight' => $vocabulary->weight,
        '#maxlength' => 255,
      );
    }
    else {
      $form['taxonomy'][$vocabulary->vid] = taxonomy_form($vocabulary->vid, $settings['taxonomy'][$vocabulary->vid], $vocabulary->help);
      $form['taxonomy'][$vocabulary->vid]['#weight'] = $vocabulary->weight;
      $form['taxonomy'][$vocabulary->vid]['#required'] = FALSE;
    }
  }

  return $form;
}

/**
 * Condition callback for node type.
 */
function uc_discount_condition_node_type($node, $settings) {
  if (in_array($node->type, $settings['node_type'])) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Condition settings form for node type.
 */
function uc_discount_condition_node_type_form($form_state, $settings) {
  $form = array();

  // Get all node types from uc_product's hook_node_info().
  $types =  uc_product_node_info();
  foreach ($types as $type => $info) {
    $options[$type] = t($info['name']);
  }

  $form['node_type'] = array(
    '#type' => 'select',
    '#title' => t('Node type'),
    '#default_value' => $settings['node_type'],
    '#multiple' => TRUE,
    '#options' => $options,
    '#description' => t('The product node is one of the selected types.'),
  );

  return $form;
}


function uc_discount_condition_coupon_code($order, $settings) {
  if (isset($order->data['coupon_code'])) {
    $order->coupon_code = drupal_strtolower($order->data['coupon_code']);
  }

  $settings['coupon_value'] = drupal_strtolower($settings['coupon_value']);

  switch ($settings['operator']) {
    case 'begins':
      return strpos($order->coupon_code, $settings['coupon_value']) === 0;

    case 'ends':
      return drupal_substr($order->coupon_code, -1 * strlen($settings['coupon_value'])) === $settings['coupon_value'];

    case 'equals':
      return $order->coupon_code == $settings['coupon_value'];

    case 'contains':
      return strpos($order->coupon_code, $settings['coupon_value']) !== FALSE;
  }

}

function uc_discount_condition_coupon_code_form($form_state, $settings = array()) {
  $form = array();

  $form['operator'] = array(
    '#type' => 'select',
    '#title' => t('Operator'),
    '#options' => array(
      'begins' => t('begins with'),
      'ends' => t('ends with'),
      'equals' => t('equals'),
      'contains' => t('contains'),
    ),
    '#default_value' => $settings['operator'],
  );

  $form['coupon_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Coupon code'),
    '#default_value' => $settings['coupon_value'],
  );

  return $form;
}

function uc_discount_condition_shippable_product($node, $settings) {
    return $node->data['shippable'];
}

/**
 * Implementation of hook_ca_action().
 */
function uc_discount_ca_action() {
  $actions = array();

  $actions['uc_discount_action_get_order_discount'] = array(
    '#title' => t('Apply a discount'),
    '#category' => t('Discounts'),
    '#callback' => 'uc_discount_get_order_discount',
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
    ),
  );
  $actions['uc_discount_action_get_product_discount'] = array(
    '#title' => t('Apply a discount to a product'),
    '#category' => t('Discounts'),
    '#callback' => 'uc_discount_get_product_discount',
    '#arguments' => array(
      'product' => array(
        '#entity' => 'node',
        '#title' => t('Product'),
      ),
    ),
  );
  $actions['uc_discount_action_get_line_item_discount'] = array(
    '#title' => t('Apply a discount to a line item'),
    '#category' => t('Discounts'),
    '#callback' => 'uc_discount_get_line_item_discount',
    '#arguments' => array(
      'line_item' => array(
        '#entity' => 'line_item',
        '#title' => t('Line item'),
      ),
    ),
  );

  return $actions;
}

/**
 * Wrapper function for order discounts.
 */
function uc_discount_get_order_discount($order, $settings) {
  return uc_discount_get_discount($order, $settings);
}

/**
 * Wrapper function for order discount forms.
 */
function uc_discount_get_order_discount_form($form_state, $settings = array()) {
  $settings['context'] = 'order';

  $form = uc_discount_get_discount_form($form_state, $settings);
  $form['line_item_weight'] = array(
    '#type' => 'weight',
    '#title' => t('List position'),
    '#delta' => 10,
    '#default_value' => isset($settings['line_item_weight']) ? $settings['line_item_weight'] : variable_get('uc_li_discount_weight', 5),
  );

  return $form;
}

/**
 * Wrapper function for product discounts.
 */
function uc_discount_get_product_discount($product, $settings) {
  $product->sell_price += uc_discount_get_discount($product, $settings);
}

/**
 * Wrapper function for product discount forms.
 */
function uc_discount_get_product_discount_form($form_state, $settings = array()) {
  $settings['context'] = 'product';
  return uc_discount_get_discount_form($form_state, $settings);
}

/**
 * Wrapper function for line item discounts.
 */
function uc_discount_get_line_item_discount($line_item, $settings) {
  $line_item['amount'] += uc_discount_get_discount($line_item, $settings);
}

/**
 * Wrapper function for line item discount forms.
 */
function uc_discount_get_line_item_discount_form($form_state, $settings = array()) {
  $settings['context'] = 'line_item';
  return uc_discount_get_discount_form($form_state, $settings);
}

/**
 * Action callback to return the amount of a discount line item.
 *
 * Unfortunately, only the forms of the discount actions is any different based
 * on the discount context. The form function names are based on the callback
 * function of the action, so they have to be split apart to establish context.
 *
 * @see uc_discount_ca_action()
 */
function uc_discount_get_discount($entity, $settings) {
  // If we are adding a base number as a discount, return it.
  if ($settings['operation'] == 'add') {
    return floatval($settings['amount']);
  }

  // When multiplying, we need to figure out how much to multiply the amount
  // by.
  $type = $settings['discount']['type'];
  $discount = uc_discount_get_discounts('discounts', $type);

  // Each discount type uses its settings and the order object to calculate
  // what the discount applies to.
  if (isset($discount['callback']) && function_exists($discount['callback'])) {
    $base = call_user_func($discount['callback'], $entity, $settings['discount'][$type]);
  }
  else {
    $base = 0;
  }

  return $base * $settings['amount'];
}

/**
 * Settings form for uc_discount_action_get_discount().
 *
 * @see uc_discount_action_get_discount()
 */
function uc_discount_get_discount_form($form_state, $settings = array()) {
  $form = array();

  $form['operation'] = array(
    '#type' => 'radios',
    '#title' => t('Operation'),
    '#options' => array(
      'add' => t('Add'),
      'multiply' => t('Multiply'),
    ),
    '#default_value' => isset($settings['operation']) ? $settings['operation'] : 'add',
    '#description' => t('Choose how the discount amount will be calculated against the order.'),
  );

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount'),
    '#default_value' => $settings['amount'],
    '#description' => t('The value to be added to or multiplied. Remember to use negative numbers to lower the order total.'),
  );

  $form['discount'] = array(
    '#type' => 'fieldset',
    '#title' => t('Discount settings'),
    '#tree' => TRUE,
  );

  // Get all of the discount types for this context, and their settings forms.
  $discounts = uc_discount_get_discount_options($settings['context']);
  $form['discount']['type'] = array(
    '#type' => 'radios',
    '#title' => t('Discount type'),
    '#options' => $discounts,
    '#default_value' => isset($settings['discount']['type']) ? $settings['discount']['type'] : key($discounts),
    '#description' => t('The part of the order needed to calculate the discount amount. Added discounts are always applied to the order total.'),
  );

  $discount_forms = uc_discount_get_discount_forms($form_state, $settings);
  foreach ($discount_forms as $id => $fieldset) {
    $form['discount'][$id] = $fieldset;
    if (!empty($fieldset)) {
      $form['discount'][$id] = $form['discount'][$id] + array(
        '#type' => 'fieldset',
        '#title' => $discounts[$id],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }
  }

  return $form;
}
