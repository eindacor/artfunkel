function readURL(input) {
    if (input.files && input.files[0]) {
        var reader = new FileReader();
        var img = new Image();

        reader.onload = function(e) {
        	$('.image-preview').attr('src', e.target.result);
        	var img_width = img.width;
        	var img_height = img.height;

        	var scale_factor = 1.0;

        	if (img_width > img_height) {
        		if (img_width > 460)
        			scale_factor = 460 / img_width;
        	}

        	else if (img_height > 460)
        		scale_factor = 460 / img_height;

        	$('.image-preview').css('width', img_width * scale_factor);
        	$('.image-preview').css('height', img_height * scale_factor);
        }

        var _URL = window.URL || window.webkitURL;
        img.src = _URL.createObjectURL(input.files[0]);
        reader.readAsDataURL(input.files[0]);
    }
}

var getImageDimensions = function() {
	var img_url = $('.image-preview').attr('src');

	if (img_url == '#')
		return {};

	var img = new Image();
	img.src = img_url;

    var img_width = img.naturalWidth;
    var img_height = img.naturalHeight;

	return {"width": img_width, "height": img_height};
}

var getArtworkWidth = function(entered_height) {
	var img_url = $('.image-preview').attr('src');

	if (img_url == '#')
		return false;

	var img = new Image();
	img.src = img_url;

    var img_width = img.naturalWidth;
    var img_height = img.naturalHeight;

	var img_aspect = img_height / img_width;

	var width = entered_height / img_aspect;

	return width.toString();
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

//verify strings are not empty and quantities are numbers
var validateArtwork = function(artwork_object) {
	if (artwork_object.artist_id == "") {
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

	if (artwork_object.height == "" || isNaN(artwork_object.height)) {
		alert("Artwork must have a valid height.");
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
	var filename = artwork_object.title.replace(/ /g, "_");
	var file = $('#image-file')[0].files[0];
	var fsFile = new FS.File(file);

	var inserted_object = images.insert(fsFile, function(err, fileObj) {
		fileObj.name(filename + '.bmp');		
	});

	console.log("inserted into images FS collection:")
	console.log(inserted_object);
	return inserted_object._id;
}

var generateImageURL = function(artwork_object, image_id) {
	var filename = artwork_object.title.replace(/ /g, "_");
	var image_url = "/uploaded_images/images-" + image_id + "-" + filename + ".bmp";
	return image_url;
}

Template.uploadArtwork.rendered = function() {
	//swap image preview when loaded in browser
	$("#image-file").change(function(){
		console.log("this: " + this);
	    readURL(this);
	});

	//attempt to generate json artwork object once submit button is pressed
	$('.submit-button').click( function() {
		if ($('.image-preview').attr('src') == '#') {
			alert("An image must be selected.");
			return;
		}

		//retrieve values from form
		var artist_name = $('#artist-name').val();
		var title = $('#title').val();
		var height = $('#height').val();
		var date = $('#date').val();
		var medium = $('#medium').val();
		var style = $('#style').val();
		var unique_id = new Mongo.ObjectID().toHexString();

		var image_dimensions = getImageDimensions();
		console.log(image_dimensions);

		var artwork_object = {
			"_id": unique_id,
			"title": title,
			"artist_id": "", 
			"image_id": "",
			"dateCreated": date, 
			"height": height,
			"width": getArtworkWidth(height),
			"image_height": image_dimensions.height,
			"image_width": image_dimensions.width,
			"medium": medium,
			"style": style,
			"imageURL": "",
			"status": "pending"
		}

		if (artist_name == "") {
			alert("Artwork must have an artist name.");
			return;
		}

		//returns id from artist database if name found, returns "" if none exist
		artwork_object.artist_id = getArtistID(artists, artist_name);

		//verify the object is valid and the proportions match those entered by the user
		if (validateArtwork(artwork_object)) {

			artworks.insert(artwork_object);
			alert("\"" + title + "\" has been submitted for approval. Thank you!");

			var image_id = uploadImage(artwork_object);
			var image_url = generateImageURL(artwork_object, image_id);

			console.log("image id: " + image_id);
			console.log("image url: " + image_url);

			artworks.update({"_id": unique_id}, {$set: {"image_id": image_id}});
			artworks.update({"_id": unique_id}, {$set: {"imageURL": image_url}});
			console.log("inserted into artworks mongo database:")
			console.log(artworks.findOne({"_id": unique_id}));

			$('textarea').each( function() {
				$(this).val("");
			});	

			$('.image-preview').attr('src', '#');
			document.getElementById('image-file').value = "";			
		}		

		console.log("artworks pending approval: " + artworks.find({'status': 'pending'}).count());
	});
}