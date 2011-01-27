<?php
  //$Id: comments_display.tpl.php,v 1.3.2.4 2010/06/08 20:30:32 prajwala Exp $
  global $facebook_sdk;
?>
<?php  /*$fb = facebook_client();*/ $fb = $_facebook_stream_sdk; ?> 
 <div class = "fb_stream_post_comments">
    <?php  
       foreach ($comments as $comment) {
    $user_info = call_facebook_api('users.getInfo', array('uids' => $comment['fromid'], 'fields' => 'name,pic_square,profile_url'));
    $user_info = $user_info[0];
    ?>
      <div class = "fb_post_comment <?php echo $string = str_replace (" ", "",$comment['id']) ?> ">
         <div class = "fb_comment_user_picture"><img src = "<?php echo $user_info[pic_square]; ?>" />
         </div>
         <div class = "fb_comment_message">
            <a href = "<?php echo $user_info[profile_url]; ?> "><?php echo $user_info['name'] ?> </a>
	 <span class = "fb_comment_user_message"><?php echo str_parse_url(check_plain($comment['text'])); ?> </span>
      <?php $time = time_duration(time() - $comment['time']).' ago'; ?>
            <div class = "fb_comment_created"><?php echo $time ?>
            </div>
      </div>
      <div style = "clear:both;"></div>
     </div>
    <?php  } ?>
 </div>
 

