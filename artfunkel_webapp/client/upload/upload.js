
Template.upload.rendered = function() {
	artists = new Mongo.Collection("artists");
	artwork = new Mongo.Collection("artwork");

	$('.submit-button').click( function() {
		var artist_name = $('#artist-name').val();
		var work_title = $('#title').val();

		if (artwork.find({"title": work_title}).count() == 0)
			artwork.insert({"title": work_title, "artistName": artist_name});
	});

	$('.lookup-button').click( function() {
		var work_title = $('#lookup-title').val();

		if (artwork.find({"title": work_title}).count() != 0)
			console.log(artwork.findOne({"title": work_title}).artistName);

		else console.log("artist not found");
	});

	artists.insert({ "artistID": 0, "artistName": "Pablo Picasso", "birthDate": "1881/10/25", "deathDate": "1973/04/08" });
	artists.insert({ "artistID": 0, "artistName": "Vincent van Gogh", "birthDate": "1853/03/30", "deathDate": "1890/07/29" });
	var picasso_count = artists.find({"artistName": "Pablo Picasso"}).count();
	console.log(picasso_count);

	var birth_date = artists.findOne({"artistName": "Vincent van Gogh"});
	console.log(birth_date);
}