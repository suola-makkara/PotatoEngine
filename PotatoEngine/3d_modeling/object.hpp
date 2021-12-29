#pragma once

#include <list>

class Camera;

class Object
{
public:
	virtual ~Object();

	virtual void render(const Camera* camera) const;

	void add(Object* object);

protected:
	std::list<Object*> children;
};