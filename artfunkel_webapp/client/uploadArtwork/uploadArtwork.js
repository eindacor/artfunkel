var getArtistID = function(artist_db, artist_name) {
	if (artist_db.find({"artistName": artist_name}).count() == 0) {
		alert("Artist not found");
		return "";
	} 

	else if (artist_db.find({"artistName": artist_name}, {"status": "pending"}).count() == 0) {
		alert("This artist is currently pending approval");
		return "";
	}

	else return artist_db.findOne({"artistName": artist_name})._id;
}

var validateArtwork = function(artwork_object) {
	if (artwork_object.artistName == "") {
		alert("Artwork must have an artist name.");
		return false;
	}

	if (artwork_object.title == "") {
		alert("Artwork must have a title.");
		return false;
	}

	if (artwork_object.dateCreated == "") {
		alert("Artwork must have a date of creation.");
		return false;
	}

	if (artwork_object.medium == "") {
		alert("Artwork must have a specified medium.");
		return false;
	}

	if (artwork_object.style == "") {
		alert("Artwork must have a specified style.");
		return false;
	}

	return true;
}

Template.uploadArtwork.rendered = function() {
	artists = new Mongo.Collection("artists");
	artworks = new Mongo.Collection("artwork");

	$('.submit-button').click( function() {
		var artist_name = $('#artist-name').val();
		var title = $('#title').val();
		var date = $('#date').val();
		var medium = $('#medium').val();
		var style = $('#style').val();

		var artist_id = getArtistID(artists, artist_name);

		if (artist_id == "")
			return;

		var unique_id = new Mongo.ObjectID().toHexString();
		var artwork_object = {
			"_id": unique_id,
			"title": title,
			"artist_id": artist_id, 
			"dateCreated": date, 
			"medium": medium,
			"style": style,
			"status": "pending"
		}

		if (validateArtwork(artwork_object)) {
			artworks.insert(artwork_object);
			alert("\"" + title + "\" has been submitted for approval. Thank you!");
			console.log(artworks.findOne({"_id": unique_id}));

			$('textarea').each( function() {
				$(this).val("");
			});
		}		
	});
}