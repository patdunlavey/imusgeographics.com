<?php
// $Id: page_title.admin.inc,v 1.1.2.10 2010/02/09 13:35:22 njt1982 Exp $

/**
 * @file
 * Admin include file.
 */

/**
 * Displays the form for the standard settings tab.
 *
 * @return
 *   array A structured array for use with Forms API.
 */
function page_title_admin_settings() {
  // Define a default looking 'form element' for setting.
  $showfield_form_element = array('#type' => 'checkbox', );


  // Define a default looking 'form element' for setting.
  $pattern_form_element = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 256,
  );


  // Set the theme callback for the patterns section
  $form['patterns'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page Title Patterns'),
    '#collapsible' => TRUE,
    '#theme' => 'page_title_admin_settings'
  );


  // Define the basic scope column values
  $form['patterns']['scope'] = array(
    'page_title_default'             => array('#type' => 'markup', '#value' => t('Global'), ),
    'page_title_front'               => array('#type' => 'markup', '#value' => t('Global'), ),
    'page_title_comment_reply'       => array('#type' => 'markup', '#value' => t('Node'), ),
    'page_title_comment_child_reply' => array('#type' => 'markup', '#value' => t('Node') .'<br />'. t('Comment'), ),
    'page_title_pager_pattern'       => array('#type' => 'markup', '#value' => t('Global'), ),
    'page_title_user'                => array('#type' => 'markup', '#value' => t('User'), ),
  );


  // Define the 'default' token patterns
  $form['patterns']['pattern'] = array(
    //Define the default pattern - this is a "fallback" pattern.
    'page_title_default' => array(
      '#title' => t('Default'),
      '#default_value' => variable_get('page_title_default', '[page-title] | [site-name]'),
      '#required' => TRUE,
      '#description' => t('This pattern will be used as a <em>fallback</em> (ie, when no other pattern is defined)'),
    ) + $pattern_form_element,

    // Define the frontpage pattern. This is use on <front>
    'page_title_front' => array(
      '#title' => t('Frontpage'),
      '#default_value' => variable_get('page_title_front', '[site-name] | [site-slogan]'),
      '#description' => t('This pattern will be used for the site frontpage'),
    ) + $pattern_form_element,

    // Define the pager pattern. This is appended to any page requests containing 'page=[0-9]+' in the query string
    'page_title_pager_pattern' => array(
      '#title' => t('Pager Suffix'),
      '#default_value' => variable_get('page_title_pager_pattern', ''),
      '#description' => t('This pattern will be appended to a page title for any given page with a pager on it'),
    ) + $pattern_form_element,

    // Define the comment reply pattern. This is used for a root comment reply (ie comment/[0-9]+).
    'page_title_comment_reply' => array(
      '#title' => t('Comment Reply'),
      '#default_value' => variable_get('page_title_comment_reply', ''),
      '#description' => t('This pattern will be used for comment reply pages, where the reply is directly to a "node"'),
    ) + $pattern_form_element,

    // Define the comment reply to comment pattern. This is used for a reply to a comment (ie comment/[0-9]+/[0-9]+).
    'page_title_comment_child_reply' => array(
      '#title' => t('Comment Child Reply'),
      '#default_value' => variable_get('page_title_comment_child_reply', ''),
      '#description' => t('This pattern with be used for comment reply pages where the reply is to an existing "comment" (eg a comment thread)'),
    ) + $pattern_form_element,

    // Define the user profile page pattern. This is used on any 'user/[0-9]' pages
    'page_title_user' => array(
      '#title' => t('User Profile'),
      '#default_value' => variable_get('page_title_user', ''),
      '#description' => t('This pattern will be used for any user profile pages'),
    ) + $pattern_form_element,
  );


  // Define the "showfield" checkbox for the user profile page
  $form['patterns']['showfield']['page_title_user_showfield'] = array(
    '#default_value' => variable_get('page_title_user_showfield', 0),
  ) + $showfield_form_element;


  // Definate the patterns per-node-type
  $types = node_get_types();
  foreach ($types as $type) {
    // Define the node-type key
    $key = 'page_title_type_'. $type->type;

    // Pattern entry
    $form['patterns']['pattern'][$key] = array(
      '#title' => t('Content Type - %type', array('%type' => $type->name)),
      '#default_value' => variable_get($key, ''),
      '#description' => t('This pattern will be used for all %type node-type pages', array('%type' => $type->name)),
    ) + $pattern_form_element;

    $form['patterns']['showfield'][$key .'_showfield'] = array(
      '#default_value' => variable_get($key .'_showfield', 0),
    ) + $showfield_form_element;

    $form['patterns']['scope'][$key] = array('#type' => 'markup', '#value' => t('Node'), );
  }


  // Definate the patterns per-vocab-type - if Taxonomy Module is enabled
  if (module_exists('taxonomy')) {
    $vocabs = taxonomy_get_vocabularies();
    foreach ($vocabs as $vocab) {
      // Define the vocab key
      $key = 'page_title_vocab_'. $vocab->vid;

      // Pattern entry
      $form['patterns']['pattern'][$key] = array(
        '#title' => t('Vocabulary - %vocab_name', array('%vocab_name' => $vocab->name)),
        '#default_value' => variable_get($key, ''),
        '#description' => t('This pattern will be used for all %title term pages', array('%title' => $vocab->name)),
      ) + $pattern_form_element;

      $form['patterns']['showfield'][$key .'_showfield'] = array(
        '#default_value' => variable_get($key .'_showfield', 0),
      ) + $showfield_form_element;

      $form['patterns']['scope'][$key] = array('#type' => 'markup', '#value' => t('Taxonomy'), );
    }
  }


  // Add the blog homepage pattern field
  if (module_exists('blog')) {
    $key = 'page_title_blog';
    $form['patterns']['pattern'][$key] = array(
      '#title' => t('Blog Homepage'),
      '#default_value' => variable_get($key, ''),
      '#description' => t('This pattern will be used for a users blog page (ie <code>/blog/1</code>)'),
    ) + $pattern_form_element;

    $form['patterns']['scope'][$key] = array('#type' => 'markup', '#value' => t('User'), );
  }


  // Add the forum root pattern field
  if (module_exists('forum')) {
    $form['patterns']['pattern']['page_title_forum_root_title'] = array(
      '#title' => t('Forum Root'),
      '#default_value' => variable_get('page_title_forum_root_title', ''),
      '#description' => t('This pattern will be used on the forum root page (ie, <code>/forum</code>)'),
    ) + $pattern_form_element;
    $form['patterns']['scope']['page_title_forum_root_title'] = array('#type' => 'markup', '#value' => t('Global'));
  }


  // Add the token help to a collapsed fieldset at the end of the configuration page.
  $form['token_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens List'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['token_help']['content'] = array(
    '#type' => 'markup',
    '#value' => theme('token_help'),
  );

  // Return the form with the system buttons & action added to it
  return system_settings_form($form);
}

