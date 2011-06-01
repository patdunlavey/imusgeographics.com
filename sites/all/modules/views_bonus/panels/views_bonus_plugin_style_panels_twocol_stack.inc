<?php
/**
 * @file
* Plugin include file for a 2 column stacked style plugin.
 */

/**
 * ...
 *
 * @todo document
 * @ingroup views_style_plugins
 */
class views_bonus_plugin_style_panels_twocol_stack extends views_plugin_style {
  /**
   * Set default options
   */
  function options(&$options) {
    parent::options($options);
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }
}
