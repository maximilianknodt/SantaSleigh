#ifndef City_hpp
#define City_hpp

#include "Model.h"
#include <list>

class City : public BaseModel {
public:
	City();
	virtual ~City();
	typedef std::list<Model*> ModelList;
	bool loadModels(const char* building, int width, int length, int streetWidth);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	ModelList getModels() { return this->models; };
	void reset();
protected:
	ModelList models;
};

#endif