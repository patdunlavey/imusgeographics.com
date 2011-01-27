<?php
  //$Id: post_display.tpl.php,v 1.4 2009/06/27 06:52:18 prajwala Exp $
?>
      <div class = "fb_user_picture"><img src = "<?php echo $user_info[pic_square];?> " /></div>
      <div class = "fb_post_message">
          <a href = "<?php echo $user_info[url]; ?> " ><?php echo $user_info['name']; ?></a>
     <span class = "message"><?php echo str_parse_url(check_plain($post['message'])); ?></span>
      </div>
      <div style = "clear:both;"></div>
      <?php
          if (!empty($post['attachment'])) {
              if (!variable_get('fbstream_block_post_display_more', 1)) {
         ?>
      <div class = "more_link" ><a href = "#" post_id = "<?php echo $post[post_id]; ?> " >more</a></div>
      <?php
   	  $classname = 'hide';
        } 
        else {
	  $classname = '';
        }
      ?>
      <div class = "more_info <?php echo $classname; ?> " post_id = "<?php echo $post[post_id]; ?> ">
          <div class = "media_wrapper">
          <?php
              if (!empty($post['attachment']['media'])) {
      	         foreach ($post['attachment']['media'] as $media) {
          ?>
          <a href = "<?php echo $media[href]; ?> "><img src = "<?php echo $media[src]; ?> " /></a>
          <?php	
	      }
            }
          ?>		
          </div>
          <div class = "attachments_info">
              <div class = "att_name"><a href = "<?php echo $post['attachment']['href']; ?> "><?php echo $post['attachment']['name']; ?></a></div>
              <div class = "att_caption"><?php echo $post['attachment']['caption']; ?></div>
              <div class = "att_desc"><?php echo $post['attachment']['description']; ?></div>
          </div>
      </div>
      <?php
          }
      ?>
      <div class = "time_comments_likes">
          <?php
              $time_duration = time_duration(time() - $post['created_time']).' ago';
          ?>    
          <span class = "time"><?php echo $time_duration; ?></span>
          <?php
              if ($post['comments']['count'] > 0) {
		$comments_str = ($post['comments']['count'] == 1) ? 'comment':'comments';
		$comments_str = $post['comments']['count'] .' '. $comments_str;
          ?>
          <span class = "comments"><a href = "#" post_id = "<?php echo trim($post['post_id']); ?> "><?php echo $comments_str; ?> </a></span>

          <?php										      } 
	      $you_like = $post['likes']['user_likes'];
              $count = $post['likes']['count'];
	      $post_id = trim($post['post_id']);
      	      if ($post['likes']['count'] > 0) {
	        $likes_str = ($post['likes']['count'] == 1) ? 'like':'likes';
          ?>
          <span class = "likes"><a href = "#" you_like = "<?php echo $you_like; ?>" count = "<?php echo $count; ?>"  post_id = "<?php echo $post_id; ?>"><?php echo $post['likes']['count']. ' ' . $likes_str; ?> </a></span>
          <?php }
	       $string = $you_like ? "Unlike it" : "Like it";
	     ?>

         </div>

         <div class="add-comments-likes">
           <?php  	if( variable_get('stream_post_comments', 0)) { ?>
               <span class = "comments"><a href = "#" post_id = "<?php echo trim($post['post_id']); ?>">comment it </a></span>
           <?php } ?>
				   <?php  	if( variable_get('stream_post_likes', 0)) { ?>
             <span class = "youlike"><a href = "#" name = "<?php echo $string; ?>" you_like = "<?php echo $you_like; ?>" count = "<?php echo $count; ?>"  post_id = "<?php echo $post_id; ?>" > <?php echo $string; ?> </a></span>
          <?php } ?>
         </div>

