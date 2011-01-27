// $Id: page_title.js,v 1.2.6.2 2010/02/09 15:41:57 njt1982 Exp $

Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.page_title = function() {
  var pt = $('input#edit-page-title').val();
  if (pt) {
    return Drupal.t('Page title: @pt', { '@pt': pt });
  }
  else {
    return Drupal.t('No page title');
  }
}
