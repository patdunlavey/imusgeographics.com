<?php
// $Id: template.php,v 1.1.2.6 2010/04/25 05:16:47 dvessel Exp $


/**
 * Preprocessor for page.tpl.php template file.
 */
function ninesixty_preprocess_page(&$vars, $hook) {
  // For easy printing of variables.
  $vars['logo_img']         = $vars['logo'] ? theme('image', substr($vars['logo'], strlen(base_path())), t('Home'), t('Home')) : '';
  $vars['linked_logo_img']  = $vars['logo_img'] ? l($vars['logo_img'], '<front>', array('attributes' => array('rel' => 'home'), 'title' => t('Home'), 'html' => TRUE)) : '';
  $vars['linked_site_name'] = $vars['site_name'] ? l($vars['site_name'], '<front>', array('attributes' => array('rel' => 'home'), 'title' => t('Home'))) : '';
  $vars['main_menu_links']      = theme('links', $vars['primary_links'], array('class' => 'links main-menu'));
  $vars['secondary_menu_links'] = theme('links', $vars['secondary_links'], array('class' => 'links secondary-menu'));

  // Make sure framework styles are placed above all others.
  $vars['css_alt'] = ninesixty_css_alter($vars['css']);
  $vars['styles'] = drupal_get_css($vars['css_alt']);
}

/**
 * Contextually adds 960 Grid System classes.
 *
 * The first parameter passed is the *default class*. All other parameters must
 * be set in pairs like so: "$variable, 3". The variable can be anything available
 * within a template file and the integer is the width set for the adjacent box
 * containing that variable.
 *
 *  class="<?php print ns('grid-16', $var_a, 6); ?>"
 *
 * If $var_a contains data, the next parameter (integer) will be subtracted from
 * the default class. See the README.txt file.
 */
function ns() {
  $args = func_get_args();
  $default = array_shift($args);
  // Get the type of class, i.e., 'grid', 'pull', 'push', etc.
  // Also get the default unit for the type to be procesed and returned.
  list($type, $return_unit) = explode('-', $default);

  // Process the conditions.
  $flip_states = array('var' => 'int', 'int' => 'var');
  $state = 'var';
  foreach ($args as $arg) {
    if ($state == 'var') {
      $var_state = !empty($arg);
    }
    elseif ($var_state) {
      $return_unit = $return_unit - $arg;
    }
    $state = $flip_states[$state];
  }

  $output = '';
  // Anything below a value of 1 is not needed.
  if ($return_unit > 0) {
    $output = $type . '-' . $return_unit;
  }
  return $output;
}

/**
 * CSS altering magic.
 */
function ninesixty_css_alter($css) {
  global $language;

  $grid_options   = ninesixty_theme_info('css grid options', array())
                  + ninesixty_theme_info('css grid options', array(), 'ninesixty');
  $column_setting = ninesixty_setting('960 columns', '12+16');
  $active_grid    = $grid_options[$column_setting];

  $css_floats = array_merge_recursive(ninesixty_theme_info('css floats', array(), 'ninesixty'),
                                      ninesixty_theme_info('css floats', array()));

  $css_remove = array_merge_recursive(ninesixty_theme_info('css remove', array(), 'ninesixty'),
                                      ninesixty_theme_info('css remove', array()));

  $debug_setting = (int) ninesixty_setting('960 debug');
  $debug_styles  = array_merge(ninesixty_theme_info('css debug', array()),
                               ninesixty_theme_info('css debug', array(), 'ninesixty'));

  // Process removes.
  $_css_remove = $grid_options;
  if ($active_key = array_search($active_grid, $grid_options)) {
    unset($_css_remove[$active_key]);
  }
  $css_remove = array_merge($css_remove, array_values($_css_remove));
  if ($debug_setting == 0) {
    $css_remove = array_merge($css_remove, $debug_styles);
  }

  // Add in RTL style names.
  if ($language->direction == LANGUAGE_RTL) {
    // RTL floats.
    $_css_floats = array();
    foreach ($css_floats as $css_float) {
      $_css_floats[] = $css_float;
      $_css_floats[] = str_replace('.css', '-rtl.css', $css_float);
    }
    $css_floats = $_css_floats;
    // RTL removes.
    $_css_remove = array();
    foreach ($css_remove as $remove) {
      $_css_remove[] = $remove;
      $_css_remove[] = str_replace('.css', '-rtl.css', $remove);
    }
    $css_remove = $_css_remove;
  }

  foreach ($css as $media => $media_styles) {
    // Allow predictable base theme css overrides by sub-themes even when the
    // directory structure is not the same. This is more consistent with how
    // module styles can be overridden by a theme thanks to a core behavior.
    $css_overrides = array();
    foreach ($media_styles as $type => $type_styles) {
      foreach (array_keys($type_styles) as $style_path) {
        $style_name = basename($style_path);
        if (!isset($css_overrides[$style_name])) {
          $css_overrides[$style_name] = $style_path;
        }
        else {
          foreach (array('module', 'theme') as $_type) {
            if (isset($css[$media][$_type][$css_overrides[$style_name]])) {
              unset($css[$media][$_type][$css_overrides[$style_name]]);
              $css_overrides[$style_name] = $style_path;
              // Remove RTL styles if it is not paired to its LTR counterpart.
              if (strpos($style_name, '-rtl.css') === FALSE) {
                $style_name_rtl = str_replace('.css', '-rtl.css', $style_name);
                if (dirname($css_overrides[$style_name]) != dirname($css_overrides[$style_name_rtl])) {
                  unset($css[$media][$_type][$css_overrides[$style_name_rtl]]);
                }
              }
              break;
            }
          }
        }
      }
    }

    // Setup framework group.
    if (isset($css[$media])) {
      $css[$media] = array_merge(array('framework' => array()), $css[$media]);
    }
    else {
      $css[$media]['framework'] = array();
    }
    
    foreach ($css[$media] as $type => $type_styles) {
      foreach ($type_styles as $style_path => $preprocess) {
        $style_name = basename($style_path);
        if (in_array($style_name, $css_remove)) {
          // Remove first and skip next block.
          unset($css[$media][$type][$style_path]);
          continue;
        }
        if (in_array($style_name, $css_floats)) {
          // Shift to framework group and remove from theme group.
          $css[$media]['framework'][$style_path] = $preprocess;
          if (isset($css[$media][$type][$style_path])) {
            unset($css[$media][$type][$style_path]);
          }
        }
      }
    }
  }

  return $css;
}

/**
 * Pulls theme settings through core's theme_get_setting and falls back to the
 * 'settings' key found within the active themes .info file. An optional
 * default can be passed in case no value is found.
 */
function ninesixty_setting($setting_key, $default = NULL) {
  // Core's settings api.
  $theme_setting = theme_get_setting($setting_key);

  if (!isset($theme_setting)) {
    // Fallback to the active theme's .info.
    $info_settings = ninesixty_theme_info('settings', array());
    if (isset($info_settings[$setting_key])) {
      // Check for specific setting.
      // .info setting key = setting[SETTING_KEY] = ...
      $theme_setting = $info_settings[$setting_key];
    }
  }

  return isset($theme_setting) ? $theme_setting : $default;
}

/**
 * Get the values defined within the .info file.
 * 
 * @param $info_key
 *  (required) The key to retrieve.
 * @param $default
 *  Fall back value if nothing is found.
 * @param $theme
 *  Theme specific value. If not set, it will return the value for the active
 *  theme.
 */
function ninesixty_theme_info($info_key, $default = NULL, $theme = NULL) {
  global $theme_key, $theme_info, $base_theme_info;

  if (!isset($theme)) {
    $theme = $theme_key;
  }

  $theme_info_data = array();
  if ($theme == $theme_info->name) {
    $theme_info_data = $theme_info->info;
  }
  else {
    foreach ($base_theme_info as $base_info) {
      if ($theme == $base_info->name) {
        $theme_info_data = $base_info->info;
        break;
      }
    }
  }

  return isset($theme_info_data[$info_key]) ? $theme_info_data[$info_key] : $default;
}
