#pragma once
#include "static_object.h"
class Chara_test:public StaticObject
{
public:
	Chara_test(std::shared_ptr<StaticMesh>& mesh) :
		StaticObject(mesh) {};
	virtual ~Chara_test() {};

private:

};