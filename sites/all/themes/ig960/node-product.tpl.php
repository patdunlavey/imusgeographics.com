<?php
// $Id: node.tpl.php,v 1.4.2.1 2009/08/10 10:48:33 goba Exp $

/**
 * @file node.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

  <div id="node-content" class="clearfix">
    <div id="node-content-top">
      <?php if ($node->content['field_full_title']['#children']): ?>
        <?php print $node->content['field_full_title']['#children']; ?>
      <?php endif; ?>
      
      <table id="content-top-table"><tr>
        <td class="product content-top left">
      <div id="node-content-top-left" class="node-content-top">
        <?php if ($endorsements): ?>
          <div id="endorsements">
          <?php print $endorsements; ?>
          </div>
        <?php endif; ?>
        </div>
        </td><td class="product content-top divider">&nbsp;</td>
        <td class="product content-top right">
      <div id="node-content-top-right" class="node-content-top">
        <?php if ($node->content['field_tag_line']['#children']): ?>
          <?php print $node->content['field_tag_line']['#children']; ?>
        <?php endif; ?>
      </div>
        </td>
        </tr></table>
      </div>
  <div id="content-columns">
    
    <div id="right-content">  
      <?php if ($node->content['add_to_cart']['#value']): ?>
        <div id="product-cart-stuff">
        <div id="buynow">
          <div id="freeshipping" class="clearfix"><span class="freeshipping">Free Shipping</span><span class="standard-domestic">(Standard / Domestic)</span> </div>
          <div class="small">Select a product option (folded, flat, laminated or 10-pack), enter a quantity, and click "Add to Cart"</div>
        <?php print $node->content['add_to_cart']['#value']; ?>
        <div class="save-alert">Save 40% per folded map<br>when you buy in 10-packs</div>
      </div>
      <?php if ($shopping_cart): ?>
        <div id="page-cart-links" class="clearfix map">
        <?php print $shopping_cart; ?>
      </div>
      <?php endif; ?>
      </div>
      <?php endif; ?>


      <?php if ($node->content['field_product_features']['#children']): ?>
        <?php print $node->content['field_product_features']['#children']; ?>
      <?php endif; ?>
      <?php if ($blog_region): ?>
        <?php print $blog_region; ?>
      <?php endif; ?>

    <!-- right-content --></div>
    <div id="left-content">
    <?php if ($node->content['image']['#value']): ?>
      <div id="product-images">
      <?php print $node->content['image']['#value']; ?>
      <div class="small center">click to view detailed images (not to scale)</div>
      </div>
    <?php endif; ?>
    
    <?php if ($node->content['body']['#value']): ?>
      <?php print $node->content['body']['#value']; ?>
    <?php endif; ?>
    </div><!--left-content-->


  <?php print $links; ?>
</div>  <!-- content -->
<?php // dpm($node); ?>
