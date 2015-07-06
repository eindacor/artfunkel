Template.admin.rendered = function() {
	artists = new Mongo.Collection("artists");
	artwork = new Mongo.Collection("artwork");

	$('#clear-artists').click( function() {
		artists.remove({});
	});

	$('#clear-artwork').click( function() {
		artwork.remove({});
	});
}