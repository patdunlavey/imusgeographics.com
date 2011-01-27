// $Id: facebook_stream.js,v 1.7.2.2 2010/05/31 20:01:52 prajwala Exp $
/*jsl:option explicit*/
/*jsl:declare window*/
/*jsl:declare document*/
/*jsl:declare $*/
/*jsl:declare Drupal*/
/*jsl:declare FB*/
/*jsl:declare setTimeout*/
// got the permissions success or failure from the facebook
// just reloads the page after getting the permissions

function got_permissions(value){
    if(value === 'publish_stream') {
	$('.post_comments_form').show();
	$('.get_perm').html(" ");
	Drupal.settings.publish_perm = true;
	return;
    }
    if (value === 'read_stream') {
	window.location=document.location.href;
    }		
}

// prompt the user for the specified permission
function prompt_for_permissions(permission) {
    FB.Connect.showPermissionDialog(permission,got_permissions);
}

function load_comments(ele){
    var post_id = ele.attr('post_id');
    $('#post_dialog').dialog('option', 'title', 'Comments of post');
    $('#post_dialog').html('<p>Loading comments please wait ...</p>');
    $('#post_dialog').dialog('open');
    $.post(Drupal.settings.basePath+'?q=fbstream/get/post/comments',
	   {'post_id':post_id},function(data){
	       $('#post_dialog').html('<p>'+data+'</p>');
	       $('.fb_stream_comment_add_form').submit(comments_form_submit);
	   });
}

function comments_click_handler(){
    var ele = $(this);
    if(Drupal.settings.publish_perm == '0'){
	FB.Connect.showPermissionDialog('publish_stream',function(value){
	    if(value == 'publish_stream'){
		Drupal.settings.publish_perm = true;
	    }
	    load_comments(ele);
	});
	return false;
    }
    load_comments(ele);
    return false;
}

function reload_post_data(post_id){
    $(".facebook_stream_msgs[post_id ='"+post_id+"'] ").fadeOut(4000, function(){    $(".loading[post_id ='"+post_id+"'] ").show();});
    $.post(Drupal.settings.basePath+'?q=fbstream/post/update',{post_id:post_id},
	   function(data){
	       $('.facebook_stream_msgs[post_id="'+post_id+'"]').html(data);
	       $(".facebook_stream_msgs[post_id ='"+post_id+"'] ").fadeIn(3500,function(){  $(".loading[post_id ='"+post_id+"'] ").hide();});
	       post_event_handlers(post_id);
	   });
}

function comments_form_submit(event){
    var comment = this.comment.value;       
    var post_id = this.post_id.value;       
    var form = this;
    $.post(Drupal.settings.basePath+'?q=fbstream/post/comments',{id:post_id,comments:comment},function(data){
	form.comment.value = '';
	$(data).appendTo($('.fb_stream_post_comments'));
	// reload the post data
	reload_post_data(post_id);
    });
    event.preventDefault();
    event.stopPropagation();
    return false;
}

function got_publish_permissions(value){
    if(value == 'publish_stream'){
	Drupal.settings.publish_perm = true;
    }
}
function post_likes(ele){
    var post_id = ele.attr("post_id");
    var name = ele.attr("name");
    var likevalue = (name == "Like it")? "Like" : "Unlike";
    $.post(Drupal.settings.basePath+'?q=fbstream/post/likes',{id:post_id,value:likevalue}, function(data){
	reload_post_data(post_id);
    });
    if($('#post_dialog').dialog('isOpen') && $('#post_dialog').attr('opened_for') === 'likes'){
	if(likevalue == 'Like'){
	    $('.youlikestring', $('#post_dialog')).text('You like this');
	    ele.attr('name','Unlike');
	    ele.html('Unlike');
	}
	else{
	    $('.youlikestring', $('#post_dialog')).text('');
	    ele.attr('name','Like it');
	    ele.html('Like it');
	}
    }  
}



function youlike_this_click_handler(){
    var objectele = $(this);
    if(Drupal.settings.publish_perm == '0'){
	FB.Connect.showPermissionDialog('publish_stream',function(value){
	    if(value == 'publish_stream'){
		Drupal.settings.publish_perm = true;
	    }
	    post_likes(objectele);  	
	});
	return false;
    }
    post_likes(objectele);  
    return false;
}

function likes_click_handler(event){
    var ele = $(this);
    var post_id = ele.attr('post_id');
    $('#post_dialog').attr('opened_for','likes');
    $('#post_dialog').dialog('option', 'title', 'Friends who like this post');
    $('#post_dialog').html('<p>Loading likes information please wait ...</p>');
    $('#post_dialog').dialog('open');
    $.post(Drupal.settings.basePath+'?q=fbstream/get/post/likes',
	   {'post_id':post_id},function(data){
	       $('#post_dialog').html('<p>'+data+'</p>');
	       $('.youlike a', $('#post_dialog')).click(youlike_this_click_handler);
	   });
    return false;
}


function post_event_handlers(post_id){
    // the display of fbstream block full or short
    var postEle = null;
    if(post_id){
	postEle = $('.facebook_stream_msgs[post_id="'+post_id+'"]');
    }
    if (!Drupal.settings.fbstream_dispaly) {
	// if display is short enable events for the more link
	var moreele;
	moreele = $('.more_link a', postEle);
	moreele.click(function(){
	    var ele = $(this);
	    var post_id = ele.attr('post_id');
	    var moreInfoEle = $('.more_info[post_id="'+post_id+'"]');
	    if (moreInfoEle.css('display') == 'none') {
		moreInfoEle.show('slow');
		ele.text('Hide more info');
	    }
	    else{
		moreInfoEle.hide('slow');
		ele.text('more');
	    }
	    return false;
	});
    }
    // assign events for the comments link
    $('.youlike a',postEle).click(youlike_this_click_handler);
    
    // assign events for the comments link
    $('.time_comments_likes .comments a, .add-comments-likes .comments a',postEle).click(comments_click_handler);
    
    // assign events for the .likes link
    $('.time_comments_likes .likes a',postEle).click(likes_click_handler);
}

Drupal.behaviors.fbstream = function(){
    $('#post_dialog').dialog({
	autoOpen: false,
	width: 415,
	buttons: {
	    "Ok": function() { 
		$(this).dialog("close"); 
	    }
	}
    });

    post_event_handlers();
    
    // update the stream data for based on the updation time
    if (typeof(Drupal.settings.fbstream_update_time) != 'undefined') {
	var time = Drupal.settings.fbstream_update_time*60*1000;
	setTimeout('fbstream_update()',time);
    }
    $('.add_status a').click(function(){
	if(Drupal.settings.publish_perm == '0'){
	    FB.Connect.showPermissionDialog('publish_stream',function(value){
		if(value == 'publish_stream'){
		    Drupal.settings.publish_perm = true;
		}
		post_status();
	    });
	    return false;
	}
	post_status();
	return false;
    });
};
//Function for posting status message
function post_status() {
    $('#post_dialog').attr('opened_for','status');
    $('#post_dialog').dialog('option', 'title', 'Post Status Message');
    $('#post_dialog').empty();
    var divele = $('<div/>');
    var input = $('<input/>');
    input.attr("class","status_input");
    input.attr("size",50);
    input.appendTo(divele);
    
    var status_submit = $("<input type='submit'>");
    status_submit.attr("value","submit");
    status_submit.attr("class","status_submit");
    status_submit.appendTo(divele);
    $('#post_dialog').append(divele);
    $('#post_dialog').dialog('open');
    
    status_submit.click(function(){
	var value = $('.status_input').val();
	if(value){
	    $.post(Drupal.settings.basePath+'?q=fbstream/post/status',
		   {'value':value},function(data){
		       $('.status_input').val('');
		       if(data != 'error')
			   fbstream_update();
		   });
	}
    }); 
    return false;
}

function fbstream_update(){
    $.get(Drupal.settings.basePath+'?q=fbstream/get',function(data){
	$('.fbstream_short_container').replaceWith(data);
	Drupal.behaviors.fbstream();
    });
}