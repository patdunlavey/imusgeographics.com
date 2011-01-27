<?php
// $Id: transliteration.admin.inc,v 1.1.2.6 2010/03/15 12:16:42 smk Exp $

/**
 * @file
 * Retroactive transliteration and admin settings UI.
 */

/**
 * Form builder function; generate retroactive transliteration confirm form.
 *
 * @see transliteration_retroactive_submit()
 * @ingroup forms
 */
function transliteration_retroactive() {
  if (!$query = transliteration_file_query(TRUE)) {
    drupal_set_message(t('Database not supported.'), 'error');
    $form['description']['#value'] = t('Retroactive transliteration is not supported for the database system of this Drupal installation. If you think this should be fixed please <a href="@issues-url">file an issue</a> in the project issue queue.', array('@issues-url' => 'http://drupal.org/project/issues/transliteration'));
    return $form;
  }

  $count = db_result(db_query($query));
  if (!$count) {
    drupal_set_message(t('Transliteration is not required.'), 'status');
    $form['description']['#value'] = t('There are currently no files names containing non-ASCII characters.');
    return $form;
  }

  $form['#redirect'] = 'admin/settings/file-system/settings';
  $question = t('Are you sure you want to transliterate existing file names?');

  // Generate a sample list.
  $items = array();
  $result = db_query_range(transliteration_file_query(), 0, 10);
  while($file = db_fetch_object($result)) {
    $filename = basename($file->filepath);
    $items[] = t('!original becomes %renamed', array('!original' => l($filename, file_create_url($file->filepath)), '%renamed' => transliteration_clean_filename($filename)));
  }
  $description = '<p><strong>' . t('The database currently lists @x_filenames containing non-ASCII characters.', array('@x_filenames' => format_plural($count, '1 file name', '@count file names'))) . '</strong> ' . t('This count might be inaccurate, though, since some files may not need to be renamed.') . '</p>';
  $description .= '<p>' . theme('item_list', array('items' => $items)) . ($count > 10 ? t('(Note: only the first 10 entries are shown.)') : '') . '</p>';
  $description .= '<p>' . t('<strong>Warning:</strong> if there are manually inserted links in contents, renaming these files will break them. Since there is currently no automated way to also fix the links, it is a good idea to backup the database and files directory beforehand.') . '</p>';
  $description .= '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, $question, 'admin/settings/file-system/settings', $description, t('Transliterate'));
}

/**
 * Form submit function; retroactively transliterate existing file names.
 *
 * @see transliteration_retroactive()
 */
function transliteration_retroactive_submit($form, &$form_state) {
  $count = 0;
  $errors = array();
  $result = db_query(transliteration_file_query());

  while ($file = db_fetch_object($result)) {
    if (!file_exists('./'. $file->filepath)) {
      // Missing file.
      $errors[] = $file->filepath;
      continue;
    }

    // Sanitize file name.
    $filename = transliteration_clean_filename(basename($file->filepath));
    // Build destination path.
    $destination = dirname($file->filepath) . '/' . $filename;

    // Rename and update the file record accordingly.
    if (file_move($file->filepath, $destination, FILE_EXISTS_RENAME)) {
      db_query("UPDATE {files} SET filepath = '%s' WHERE fid = %d", $file->filepath, $file->fid);
      $count++;
    }
    else {
      $errors[] = $file->filepath;
    }
  }

  if ($errors) {
    $message = t('Not all file names could be converted. The following files could not be accessed and have been ignored:');
    $message .= theme('item_list', $errors);
    drupal_set_message($message, 'error');
  }
  else {
    drupal_set_message(t('@filenames have been successfully transliterated.', array('@filenames' => format_plural($count, '1 file name', '@count file names'))));
  }

  // Flush page cache.
  cache_clear_all();
}

/**
 * Build a query that returns all file names from the database containing non-ASCII characters.
 *
 * @param $count
 *   Set to TRUE to return a count query.
 */
function transliteration_file_query($count = FALSE) {
  // Regular expressions are not supported by Drupal's database layer and
  // operators differ between manufacturers.
  switch ($GLOBALS['db_type']) {
    case 'mysql':
    case 'mysqli':
      $operator = 'NOT REGEXP';
      $regex = '/[a-z0-9_.-]+$';
      break;

    case 'pgsql':
      $operator = '!~*';
      $regex = '/[a-z0-9_.-]+$';
      break;

    default:
      return FALSE;
  }

  $fields = ($count ? 'COUNT(*)' : '*');
  return "SELECT $fields FROM {files} WHERE filepath $operator '$regex'";
}

