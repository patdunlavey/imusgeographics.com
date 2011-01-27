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
<?php // dpm($node); ?>
<div id="node-<?php print $node->nid; ?>" class="endorsement-node node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <span class="submitted"><?php print $submitted ?></span>
  <?php endif; ?>

  <?php if ($terms): ?>
    <div class="terms terms-inline"><?php print $terms ?></div>
  <?php endif;?>
  </div>

  <table class="content"><tr>
    <td colspan="2">
      <div class="field field-type-nodereference field-field-product-reference">  <div class="field-items">  <div class="field-item odd">  <?php print($node->field_product_reference[0]['view']); ?></div>  </div> </div> 
      </td></tr>
      <tr>
    <td>
    <div class="product-reference-block">
    <div class="field field-type-product-picture field-field-product-picture">  <div class="field-items">  <div class="field-item odd">
      <?php
      print '<a href="/'.$node->product_node->field_image[0]['filepath'].'" class="thickbox"><img src="'.$node->product_picture['filepath'].'"></a>';
      ?>
    </div>  </div> </div> 
    </td>
    <td>
    <div class="field field-type-endorsement-body">  <div class="field-items">  
    <?php print($node->content['body']['#value']); ?>
    </div>  </div>
    <div class="field field-type-endorsement-author endorsement_author">  <div class="field-items">&ndash;&nbsp; 
    <?php
    print($node->field_endorsement_author[0]['view']);
    ?>
  </div>  </div>
  </td></tr></table>

  <?php print $links; ?>
</div>