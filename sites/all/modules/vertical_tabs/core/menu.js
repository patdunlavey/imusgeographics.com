// $Id: menu.js,v 1.1.2.2 2011/01/26 18:23:33 davereid Exp $

Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.menu = function() {
  if ($('#edit-menu-link-title').val()) {
    return Drupal.checkPlain($('#edit-menu-link-title').val());
  }
  else {
    return Drupal.t('Not in menu');
  }
}
