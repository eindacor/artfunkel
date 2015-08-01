Template.adminDashboard.helpers({
	'artwork': function() {
		return artworks.find({'status': 'pending'});
	},

	'artist': function() {
		return artists.find({'status': 'pending'});
	}
});

//Template.artworkEntry.events

Template.artworkEntry.rendered = function(){	
	console.log('artworkEntry rendered: ' + this.data.title)
	var img_url = this.data.imageURL;

	var img_width = $("#image_" + this.data._id).get(0).naturalWidth;
    var img_height = $("#image_" + this.data._id).get(0).naturalHeight;

	var scale_factor = 1.0;

	if (img_width > img_height) {
		if (img_width > 460)
			scale_factor = 460 / img_width;
	}

	else if (img_height > 460)
		scale_factor = 460 / img_height;

	$("#image_" + this.data._id).css('width', img_width * scale_factor);
	$("#image_" + this.data._id).css('height', img_height * scale_factor);

	var title = this.data.title;
	var _id = this.data._id;
	$("#accept_" + this.data._id).click( function () {
		artworks.update({"_id": _id}, {$set: {"status": "accepted"}});
		console.log("accept " + title);
	});

	$("#reject_" + this.data._id).click( function () {
		artworks.update({"_id": _id}, {$set: {"status": "rejected"}});
		console.log("reject " + title);
	});
}

Template.artistEntry.rendered = function(){	
	console.log('artistEntry rendered: ' + this.data.title)
	var name = this.data.artistName;
	var _id = this.data._id;
	$("#accept_" + this.data._id).click( function () {
		artists.update({"_id": _id}, {$set: {"status": "accepted"}});
		console.log("accept " + name);
	});

	$("#reject_" + this.data._id).click( function () {
		artists.update({"_id": _id}, {$set: {"status": "rejected"}});
		console.log("reject " + name);
	});
}

Template.adminDashboard.rendered = function() {
	console.log("admin rendered");
}