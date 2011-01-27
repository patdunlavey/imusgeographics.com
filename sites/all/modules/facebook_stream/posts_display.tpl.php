<?php
  //$Id: posts_display.tpl.php,v 1.6 2009/06/27 06:52:18 prajwala Exp $
?>
<div class="fbstream_short_container">
<?php  	if( variable_get('stream_post_status', 0)) { ?>
<div class="add_status"><a href ="#">Add Status Message </a></div>
<?php } ?>
<?php
  foreach ($stream['posts'] as $post) {
     $user_info = _get_user_info($stream['profiles'], $post['source_id']);
     $post['post_id'] = trim($post['post_id']);
?> 
     <div class = "facebook_stream_msgs" post_id = "<?php echo $post['post_id']; ?>">
          <div class = "loading hide" post_id = "<?php echo $post['post_id']; ?>" >Loading ...</div> 
          <?php
             echo theme('post_display', $post, $user_info);
          ?>
     </div>
<?php  } ?>
</div>
