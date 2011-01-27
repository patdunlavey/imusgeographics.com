/*$Id: README.txt,v 1.2.2.1 2010/05/31 20:10:42 prajwala Exp $*/

== SUMMARY ==
Facebook stream module displays the facebook stream in a drupal block. To access the users stream the user has to give permission to registered application. This module will the ask the user to give permissions if the user did not give permission to access his stream. The posts will update automatically after some time. This time can be configured by site administrator in the block configuration page

We can able to post status message and and can add comments and likes to the status messages.


== REQUIREMENTS ==
PHP 5.2 or higher versions. 
Drupal 6.x. 
Facebook PHP Library: http://github.com/facebook/php-sdk. If you are already using Drupal for facebook module no need to add this library.
Facebook API key: http://www.facebook.com/developers/
jquery_ui,jquery_update modules


== INSTALLATION ==

=== jquery ui ===

  1.Download the jquery ui module from http://drupal.org/project/jquery_ui
  2.Follow steps of http://drupal.org/node/388384#comment-1530114(Note:jqery update must be devel modul itself)
  
=== facebook_stream ===
  1.Copy facebook_stream module your modules directory
  2.Enable the module at admin/build/modules
  3. Visit admin/settings/facebook_stream page and choose if you are already using Drupal for facebook module. Otherwise copy download php sdk from http://github.com/facebook/php-sdk and copy the extracted directory into facebook stream module as facebook-php-sdk. Specify the facebook Application ID and secret key details on the admin/settings/facebook_stream page.
  4.Enable facebook stream block at admin/build/blocks
  5.Configure the no of values to be displayed at user/%/fbstreamconnect
  6.Admin can configure the retrival interval at the facebook stream block configuration page
  7.Admin can configure whether to enable publish comments, likes, status message or not at admin/settings/facebook_stream


