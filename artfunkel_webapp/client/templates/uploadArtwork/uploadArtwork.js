function readURL(input) {
    if (input.files && input.files[0]) {
        var reader = new FileReader();
        var img = new Image();

        reader.onload = function(e) {
        	$('#image-preview').attr('src', e.target.result);
        	console.log("width: " + img.width);
        	var img_width = img.width;
        	var img_height = img.height;

        	var scale_factor = 1.0;

        	if (img_width > img_height) {
        		if (img_width > 600)
        			scale_factor = 600 / img_width;
        	}

        	else if (img_height > 600)
        		scale_factor = 600 / img_height;

        	console.log("scale factor: " + scale_factor);
        	$('#image-preview').css('width', img_width * scale_factor);
        	$('#image-preview').css('height', img_height * scale_factor);
        }

        var _URL = window.URL || window.webkitURL;
        img.src = _URL.createObjectURL(input.files[0]);
        reader.readAsDataURL(input.files[0]);
    }
}

var getArtistID = function(artist_db, artist_name) {
	if (artist_db.find({"artistName": artist_name}).count() == 0) {
		var register_artist = confirm("Artist not found. Would you like to register this artist?");

		if (register_artist)
			Router.go('/registerArtist');

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

	if (artwork_object.height == "") {
		alert("Artwork must have a height.");
		return false;
	}

	if (artwork_object.width == "") {
		alert("Artwork must have a width");
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

var uploadImage = function(artwork_object) {
	var filename = artwork_object.title.replace(" ", "_");
	var file = $('#image-file')[0].files[0];
	var fsFile = new FS.File(file);

	images.insert(fsFile, function(err, fileObj) {
		console.log("file id: " + fileObj._id);
		fileObj.name(filename + '.bmp');
	});

	return;
}

Template.uploadArtwork.rendered = function() {
	artists = new Mongo.Collection("artists");
	artworks = new Mongo.Collection("artwork");

	$("#image-file").change(function(){
	    readURL(this);
	});

	$('.submit-button').click( function() {
		var artist_name = $('#artist-name').val();
		var title = $('#title').val();
		var height = $('#height').val();
		var width = $('#width').val();
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
			"height": height,
			"width": width,
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

			uploadImage(artwork_object);
		}		
	});
}