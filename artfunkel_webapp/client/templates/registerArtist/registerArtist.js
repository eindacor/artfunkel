var validateArtist = function(artist_db, artist_object) {
	if (artist_db.find({"email": artist_object.email}).count() != 0 && artist_object.email != "artfunkelgame@gmail.com") {
		alert("An artist with this email has already been registered");
		return false;
	}

	if (artist_db.find({"artistName": artist_object.artistName}).count() != 0) {
		alert("An artist with this name has already been registered");
		return false;
	}

	if (artist_object.artistName == "") {
		alert("Artist must have a name");
		return false;
	}

	if (artist_object.birthDate == "") {
		alert("Artist must have a date of birth");
		return false;
	}

	if (artist_object.portfolio == "") {
		alert("Artist must have a portfolio url");
		return false;
	}

	if (artist_object.email == "") {
		alert("An email address must be provided");
		return false;
	}

	return true;
}

Template.registerArtist.rendered = function() {
	$('.submit-button').click( function() {
		var artist_name = $('#artist-name').val();
		var dob = $('#birth-date').val();
		var dod = $('#death-date').val();
		var portfolio = $('#portfolio').val();
		var email = $('#email').val();

		var unique_id = new Mongo.ObjectID().toHexString();

		var artist_object = {
			"_id": unique_id,
			"artistName": artist_name, 
			"birthDate": dob, 
			"deathDate": dod,
			"portfolio": portfolio,
			"email": email,
			"status": (email == "artfunkelgame@gmail.com" ? "approved" : "pending")
		}

		if (validateArtist(artists, artist_object)) {
			artists.insert(artist_object);
			if (artist_object.status == "pending")
				alert("The artist \"" + artist_name + "\" has been submitted for approval. Thank you!");

			else if (artist_object.status == "approved")
				alert("The artist \"" + artist_name + "\" has been added to the database. Thank you!");

			console.log(artists.findOne({"_id": unique_id}));

			$('textarea').each( function() {
				$(this).val("");
			});
		}		
	});
}