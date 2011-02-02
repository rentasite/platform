
// $Id: foxycart.js,v 1.1 2010/03/09 03:09:54 kilogauss Exp $ 

function fc_BuildFoxyCart() {
        fc_FoxyCart = "";
    	if (fc_json.products.length > 0)
        {
        	for (i=0;i<fc_json.products.length;i++) {
                fc_BuildFoxyCartRow(fc_json.products[i].name,fc_json.products[i].code,fc_json.products[i].options,fc_json.products[i].quantity,fc_json.products[i].price_each,fc_json.products[i].price);
	        }        
        }
        
        // fc_FoxyCart is a javascript variable that now holds your shopping cart data
 
        // if you have some products in your cart, why not display it?
        if (fc_json.products.length > 0) {
                $("#fc_cart #cart_content").html(fc_FoxyCart);
        } else {
                $("#fc_cart #cart_content").html("");
        }
}
// This function is called by fc_BuildFoxyCart() for each product in your cart.
// Feel free to edit this function as needed to display each row of your cart.
function fc_BuildFoxyCartRow(fc_name,fc_code,fc_options,fc_quantity,fc_price_each,fc_price) {
        fc_FoxyCart += "<tr>";
        fc_FoxyCart += "<td>" + fc_name + "</td>";
//      fc_FoxyCart += "<td>" + fc_options + "</td>";
//      fc_FoxyCart += "<td>" + fc_code + "</td>";
        fc_FoxyCart += "<td class=\"right-align\">" + fc_quantity + "</td>";
//      fc_FoxyCart += "<td>" + fc_price_each + "</td>";
        fc_FoxyCart += "<td class=\"right-align\">" + fc_price.toFixed(2) + "</td>";
        fc_FoxyCart += "</tr>";
}
