#ifndef GALLERY_H
#define GALLERY_H

#include "header.h"

class gallery
{
public:
	//TODO modify constructors to take a model file, containing specific display points
	gallery(const char* model_path);
	~gallery(){};

	void addPainting(int index, const shared_ptr<artwork> &work);
	void renderGallery(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam) const;

private:
	//int is the index of the specific position, mat4 is the position matrix
	int max_paintings;
	map <int, mat4> work_positions;
	map <int, shared_ptr<artwork> > works_displayed;
	float width;
	float height;

	//vector<player> players_present;
};

#endif