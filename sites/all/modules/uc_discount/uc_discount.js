// $Id$

/**
 * @file
 * Handle asynchronous requests to calculate discounts.
 */

var pane = '';
if ($("input[@name*=delivery_]").length) {
  pane = 'delivery'
}
else if ($("input[@name*=billing_]").length) {
  pane = 'billing'
}

$(document).ready(function() {
  getDiscounts();
  $("select[@name*=delivery_country], "
    + "select[@name*=delivery_zone], "
    + "input[@name*=delivery_city], "
    + "input[@name*=delivery_postal_code], "
    + "select[@name*=billing_country], "
    + "select[@name*=billing_zone], "
    + "input[@name*=billing_city], "
    + "input[@name*=billing_postal_code]").change(getDiscounts);
  $('#edit-panes-payment-current-total').click(getDiscounts);
});

/**
 * Get discounts for the current cart and line items.
 */
function getDiscounts() {
  var order = serializeOrder();

  if (!!order) {
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + "?q=discounts/calculate",
      data: 'order=' + Drupal.encodeURIComponent(order),
      dataType: "json",
      success: function(discounts) {
        var key;
        var render = false;
        var i;
        var j;
        for (j in discounts) {
          key = 'discount_' + discounts[j].id;
          // Check that this discount is a new line item, or updates its amount.
          if (!li_values[key] || li_values[key] != discounts[j].amount) {
            set_line_item("discount_" + discounts[j].id, discounts[j].title, discounts[j].amount, discounts[j].weight, 1, false);

            // Set flag to render all line items at once.
            render = true;
          }
        }
        var found;
        // Search the existing tax line items and match them to a returned tax.
        for (key in li_titles) {
          // The discount id is the second part of the line item id if the
          // first part is "discount".
          i = key.split('_', 2);
          if (i[0] == 'discount') {
            found = false;
            for (j in discounts) {
              if (discounts[j].id == i[1]) {
                found = true;
                break;
              }
            }
            // No discount was matched this time, so remove the line item.
            if (!found) {
              delete li_titles[key];
              delete li_values[key];
              delete li_weight[key];
              delete li_summed[key];
              // Even if no discounts were added earlier, the display must be
              // updated.
              render = true;
            }
          }
        }
        if (render) {
          render_line_items();
        }
      }
    });
  }
}
