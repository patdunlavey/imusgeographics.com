<?php
// $Id: page.tpl.php,v 1.1.2.2 2010/04/20 22:52:04 dvessel Exp $
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>

<body>
<div id="main">
<?php  if ($top) : ?>
  <div id="top">
    <?php print($top); ?>
    </div>
<?php endif; ?>
  <div id="header">
    <table><tr><td id="td_imusgeologo">
    <div id="imusgeologo">
      <?php
      // Prepare header
      $site_fields = array();
      if ($site_name) {
        $site_fields[] = check_plain($site_name);
      }
      if ($site_slogan) {
        $site_fields[] = check_plain($site_slogan);
      }
      $site_title = implode(' ', $site_fields);
      if ($site_fields) {
        $site_fields[0] = '<span>'. $site_fields[0] .'</span>';
      }
      $site_html = implode(' ', $site_fields);

      if ($logo || $site_title) {
        print '<a href="'. check_url($front_page) .'" title="'. $site_title .'">';
        if ($logo) {
          print '<img src="'. check_url($logo) .'" alt="'. $site_title .'" id="logo" />';
        }
        print $site_html .'</a>';
      }
    ?>
    </div>
    </td>
    <td id="td_navlinks">
    <table id="navlinks">
    <?php if (isset($primary_links)) : ?>
      <tr id="primary-navlinks"><td <?php if(isset($secondary_links) & isset($header)): ?> colspan="2"<?php endif; ?> >
      <?php print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
      </td></tr>
    <?php endif; ?>
    <tr id="secondary-links">
      <?php if (isset($secondary_links)) : ?>
        <td id="secondary-menu">
        <?php print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
      </td>
      <?php endif; ?>
      <?php if (isset($navigation)) : ?>
        <td id="navigation-menu" class="links navigation-links">
        <?php print $navigation; ?> 
      </td>
      <?php endif; ?>
   </tr></table>
    </td></tr></table>
  </div> <!-- /header -->
<div id="page">    
     <?php print $breadcrumb; ?>
     <?php if ($title & 0): ?>
       <h1 class="title" id="page-title"><?php print $title; ?></h1>
     <?php endif; ?>
     <?php if ($tabs): ?>
       <div class="tabs"><?php print $tabs; ?></div>
     <?php endif; ?>
     <?php print $messages; ?>
     <?php print $help; ?>


     <div id="content" class="region clearfix">
       <?php print $content; ?>
     </div>

     <?php print $feed_icons; ?>

   <?php if ($left): ?>
     <div id="sidebar-left">
       <?php print $left; ?>
     </div>
   <?php endif; ?>

   <?php if ($right): ?>
     <div id="sidebar-right">
       <?php print $right; ?>
     </div>
   <?php endif; ?>
   
   <?php if ($content_trailer): ?>
     <div id="content-trailer"" class="clearfix">
       <?php print $content_trailer; ?>
     </div>
   <?php endif; ?>

   <div id="footer">
     <?php if ($footer): ?>
       <div id="footer-region">
         <?php print $footer; ?>
       </div>
     <?php endif; ?>

     <?php if ($footer_message): ?>
       <div id="footer-message">
         <?php print $footer_message; ?>
       </div>
     <?php endif; ?>
     </div><!--page-->
     </div><!-- main -->
     <?php print $closure; ?>

</body>
</html>
