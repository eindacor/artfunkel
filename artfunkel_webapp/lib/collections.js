resizeImage = function(fileObj, readStream, writeStream) {
	gm(readStream, fileObj.name()).resize('100', '100').stream().pipe(writeStream);
};

images = new FS.Collection("images", {
	stores: [new FS.Store.FileSystem("images", {path: "../../../../../private/uploaded_images"})]
});

image_previews = new FS.Collection("image_previews", {
	stores: [new FS.Store.FileSystem("image_previews", {path: "../../../../../client/image_previews", transformWrite: resizeImage})]
});

testGM = function() {
	console.log("gm: " + gm.isAvailable);
}

//testGM();

// artists = new Mongo.Collection("artists");
// artworks = new Mongo.Collection("artwork");