<?php
// $Id: stream.admin.inc,v 1.1.2.3 2010/06/08 20:30:32 prajwala Exp $

/**
 * @file
 * administrator form for facebook stream module
 *
 * Display the Facebook stream Settings form 
 * Configure comments, posts and status messages to publish on the facebook wall
 */
function facebook_stream_settings_form() {
  $facebook_stream_php_sdk_id= variable_get('facebook_stream_php_sdk', 0);
  $form['facebook_stream_php_sdk'] = array(
    '#type' => 'radios',
    '#title' => t('Are you using other facebook modules'),
    '#default_value' => variable_get('facebook_stream_php_sdk', 0),
    '#options' => array(t('Not Using other modules'), t('Drupal for facebook 6.x-3.x'), t('Drupal for facebook 6.x-2.x'), t('fbconnect 6.x-1.x')),
    '#ahah' => array(
      'path' => 'ahahform_callback/fbstream',
      'wrapper' => 'facebook_stream_admin_wrapper',
      'method' => 'replace',
    ),

  );

  //Display textfields when first option is selected
  if ($facebook_stream_php_sdk_id == 0) {
  $form['facebook_stream_facebook_api_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook API ID'),
    '#prefix' => '<div id="facebook_stream_admin_wrapper">',
    '#default_value' => variable_get('facebook_stream_facebook_api_id', NULL),
  );
  $form['facebook_stream_facebook_secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook Secret Key'),
    '#suffix' => '</div>',
    '#default_value' => variable_get('facebook_stream_facebook_secret_key', NULL),
  );
  } 
  else {
    $form['wrapper'] = array(
      '#value' => '<div id="facebook_stream_admin_wrapper"></div>',
    );
  }

  $form['stream_post_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to post comments'),
    '#default_value' => variable_get('stream_post_comments', 0),
    '#prefix' => "<b>". t('Set the options to post comments, likes and Status messages on the user wall of facebook') ."</b>",
  );
  $form['stream_post_likes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to post Likes'),
    '#default_value' => variable_get('stream_post_likes', 0),
  );
  $form['stream_post_status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to post Status messages'),
    '#default_value' => variable_get('stream_post_status', 0),
  );

  return system_settings_form($form);
}

