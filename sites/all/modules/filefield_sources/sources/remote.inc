<?php

/**
 * @file
 * A FileField extension to allow referencing of existing files.
 *
 * The "hooks" in this file are not true hooks, they're called individually
 * from the main filefield_sources.module in the corresponding hook by the
 * same name. Any of these hooks could be broken out into a separate module.
 */

define('FILEFIELD_SOURCE_REMOTE_HINT_TEXT', 'http://example.com/files/file.png');

/**
 * Implements hook_filefield_source_info().
 */
function filefield_source_remote_info() {
  $source = array();
  $source['remote'] = array(
    'name' => t('Remote URL textfield'),
    'label' => t('Remote URL'),
    'description' => t('Download a file from a remote server.'),
    'process' => 'filefield_source_remote_process',
    'value' => 'filefield_source_remote_value',
  );
  return $source;
}

/**
 * Implements hook_menu().
 */
function filefield_source_remote_menu() {
  $items = array();

  $items['filefield/remote/progress/%/%/%'] = array(
    'page callback' => 'filefield_source_remote_progress',
    'page arguments' => array(3, 4, 5),
    'access arguments' => array('access content'),
    'file' => 'sources/remote.inc',
    'type' => MENU_CALLBACK,
  );
  return $items;
}

/**
 * Implements hook_theme().
 */
function filefield_source_remote_theme() {
  return array(
    'filefield_source_remote_element' => array(
      'arguments' => array('element' => NULL),
      'file' => 'sources/remote.inc',
    ),
 );
}

/**
 * Implements hook_filefield_source_settings().
 */
function filefield_source_remote_settings($op, $field) {
  $return = array();

  // Add settings to the FileField widget form.

  return $return;

}

/**
 * A #process callback to extend the filefield_widget element type.
 */
function filefield_source_remote_process($element, $edit, &$form_state, $form) {

  $element['filefield_remote'] = array(
    '#weight' => 100.5,
    '#theme' => 'filefield_source_remote_element',
    '#filefield_source' => TRUE, // Required for proper theming.
    '#filefield_sources_hint_text' => FILEFIELD_SOURCE_REMOTE_HINT_TEXT,
  );

  $element['filefield_remote']['url'] = array(
    '#type' => 'textfield',
    '#description' => filefield_sources_element_validation_help($element['#upload_validators']),
    '#maxlength' => NULL,
  );

  $element['filefield_remote']['transfer'] = array(
    '#name' => implode('_', $element['#array_parents']) . '_transfer',
    '#type' => 'submit',
    '#value' => t('Transfer'),
    '#validate' => array(),
    '#submit' => array('node_form_submit_build_node'),
    '#ahah' => array(
      'path' => 'filefield/ahah/' . $element['#type_name'] . '/' . $element['#field_name'] . '/' . $element['#delta'],
      'wrapper' => $element['#id'] . '-ahah-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'bar',
        'path' => 'filefield/remote/progress/' . $element['#type_name'] . '/' . $element['#field_name'] . '/' . $element['#delta'],
        'message' => t('Starting transfer...'),
      ),
    ),
  );

  return $element;
}

/**
 * A #filefield_value_callback function.
 */
function filefield_source_remote_value($element, &$item) {
  if (isset($item['filefield_remote']['url']) && strlen($item['filefield_remote']['url']) > 0 && valid_url($item['filefield_remote']['url']) && $item['filefield_remote']['url'] != FILEFIELD_SOURCE_REMOTE_HINT_TEXT) {
    $field = content_fields($element['#field_name'], $element['#type_name']);
    $url = $item['filefield_remote']['url'];

    // Check that the temporary directory is writable.
    $temporary_directory = file_directory_temp();
    if (!field_file_check_directory($temporary_directory, FILE_MODIFY_PERMISSIONS, $item['filefield_remote']['url'])) {
      return;
    }

    // Check that the destination is writable.
    $directory = filefield_widget_file_path($field);
    if (!field_file_check_directory($directory, FILE_CREATE_DIRECTORY, $item['filefield_remote']['url'])) {
      return;
    }

    // Check the headers to make sure it exists and is within the allowed size.
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, TRUE);
    curl_setopt($ch, CURLOPT_NOBODY, TRUE);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch, CURLOPT_HEADERFUNCTION, '_filefield_source_remote_parse_header');
    // Causes a warning if PHP safe mode is on.
    @curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_exec($ch);
    $info = curl_getinfo($ch);
    curl_close($ch);

    if ($info['http_code'] != 200) {
      switch ($info['http_code']) {
        case 403:
          form_error($element, t('The remote file could not be transfered because access to the file was denied.'));
          break;
        case 404:
          form_error($element, t('The remote file could not be transfered because it was not found.'));
          break;
        default:
          form_error($element, t('The remote file could not be transfered due to an HTTP error (@code).', array('@code' => $info['http_code'])));
      }
      return;
    }

    // Update the $url variable to reflect any redirects.
    $url = $info['url'];
    $url_info = parse_url($url);

    // Determine the proper filename by reading the filename given in the
    // Content-Disposition header. If the server fails to send this header,
    // fall back on the basename of the URL.
    //
    // We prefer to use the Content-Disposition header, because we can then
    // use URLs like http://example.com/get_file/23 which would otherwise be
    // rejected because the URL basename lacks an extension.
    $filename = _filefield_source_remote_filename();
    if (empty($filename)) {
      $filename = rawurldecode(basename($url_info['path']));
    }

    $pathinfo = pathinfo($filename);
    $filename = filefield_sources_clean_filename($filename);
    $filepath = file_create_filename($filename, $temporary_directory);

    if (empty($pathinfo['extension'])) {
      form_error($element, t('The remote URL must be a file and have an extension.'));
      return;
    }

    // Perform basic extension check on the file before trying to transfer.
    $extensions = $field['widget']['file_extensions'];
    $regex = '/\.('. preg_replace('/[ +]/', '|', preg_quote($extensions)) .')$/i';
    if (!empty($extensions) && !preg_match($regex, $filename)) {
      form_error($element, t('Only files with the following extensions are allowed: %files-allowed.', array('%files-allowed' => $extensions)));
      return;
    }

    // Check file size based off of header information.
    if (!empty($element['#upload_validators']['filefield_validate_size'][0])) {
      $max_size = $element['#upload_validators']['filefield_validate_size'][0];
      $file_size = $info['download_content_length'];
      if ($file_size > $max_size) {
        form_error($element, t('The remote file is %filesize exceeding the maximum file size of %maxsize.', array('%filesize' => format_size($file_size), '%maxsize' => format_size($max_size))));
        return;
      }
    }

    // Set progress bar information.
    $options = array(
      'key' => $element['#type_name'] . '_' . $element['#field_name'] . '_' . $element['#delta'],
      'filepath' => $filepath,
    );
    filefield_source_remote_set_transfer_options($options);

    // Then make the actual request to download the file.
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, FALSE);
    curl_setopt($ch, CURLOPT_WRITEFUNCTION, 'filefield_source_remote_curl_write');
    // Causes a warning if PHP safe mode is on.
    @curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
    if (curl_exec($ch) && $file = field_file_save_file($filepath, $element['#upload_validators'], $directory)) {
      $item = array_merge($item, $file);
    }
    curl_close($ch);

    // Delete the temporary file.
    @unlink($filepath);
  }
}

/**
 * Parse cURL header and record the filename specified in Content-Disposition.
 */
function _filefield_source_remote_parse_header(&$ch, $header) {
  if (preg_match('/Content-Disposition:.*?filename="(.+?)"/', $header, $matches)) {
    // Content-Disposition: attachment; filename="FILE NAME HERE"
    _filefield_source_remote_filename($matches[1]);
  }
  elseif (preg_match('/Content-Disposition:.*?filename=([^; ]+)/', $header, $matches)) {
    // Content-Disposition: attachment; filename=file.ext
    _filefield_source_remote_filename($matches[1]);
  }

  // This is required by cURL.
  return strlen($header);
}

/**
 * Get/set the remote file name in a static variable.
 */
function _filefield_source_remote_filename($curl_filename = NULL) {
  static $filename = NULL;
  if (isset($curl_filename)) {
    $filename = $curl_filename;
  }
  return $filename;
}

/**
 * Menu callback; progress.js callback to return upload progress.
 */
function filefield_source_remote_progress($type_name, $field_name, $delta) {
  $key = $type_name . '_' . $field_name . '_' . $delta;
  $progress = array(
    'message' => t('Starting transfer...'),
    'percentage' => -1,
  );

  if ($cache = cache_get('filefield_transfer:'. session_id() . ':' . $key)) {
    $current = $cache->data['current'];
    $total = $cache->data['total'];
    $progress['message'] = t('Transfering... (@current of @total)', array('@current' => format_size($current), '@total' => format_size($total)));
    $progress['percentage'] = round(100 * $current / $total);
  }

  drupal_json($progress);
}

/**
 * cURL write function to save the file to disk. Also updates progress bar.
 */
function filefield_source_remote_curl_write(&$ch, $data) {
  $progress_update = 0;
  $options = filefield_source_remote_get_transfer_options();

  // Get the current progress and update the progress value.
  // Only update every 64KB to reduce cache_set calls. cURL usually writes
  // in 16KB chunks.
  if (curl_getinfo($ch, CURLINFO_SIZE_DOWNLOAD) / 65536 > $progress_update) {
    $progress_update++;
    $progress = array(
      'current' => curl_getinfo($ch, CURLINFO_SIZE_DOWNLOAD),
      'total' => curl_getinfo($ch, CURLINFO_CONTENT_LENGTH_DOWNLOAD),
    );
    // Set a cache so that we can retrieve this value from the progress bar.
    $cid = 'filefield_transfer:'. session_id() . ':' . $options['key'];
    if ($progress['current'] != $progress['total']) {
      cache_set($cid, $progress, 'cache', time() + 300);
    }
    else {
      cache_clear_all($cid, 'cache');
    }
  }

  $data_length = 0;
  if ($fp = @fopen($options['filepath'], 'a')) {
    fwrite($fp, $data);
    fclose($fp);
    $data_length = strlen($data);
  }

  return $data_length;
}

/**
 * Set a transfer key that can be retreived by the progress function.
 */
function filefield_source_remote_set_transfer_options($options = NULL) {
  static $current = FALSE;
  if (isset($options)) {
    $current = $options;
  }
  return $current;
}

/**
 * Get a transfer key that can be retrieved by the progress function.
 */
function filefield_source_remote_get_transfer_options() {
  return filefield_source_remote_set_transfer_options();
}

/**
 * Theme the output of the autocomplete field.
 */
function theme_filefield_source_remote_element($element) {
  $element['url']['#field_suffix'] = theme('submit', $element['transfer']);
  return '<div class="filefield-source filefield-source-remote clear-block">' . theme('textfield', $element['url']) . '</div>';
}

