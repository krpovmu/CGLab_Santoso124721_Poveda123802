#pragma once
#ifndef GEOMERTY_NODE_HPP
#define GEOMERTY_NODE_HPP

#include <model.hpp>
#include <string>
#include <memory>
#include "node.hpp"

class GeometryNode{

private:
	model geometry;//model geomety

public:
	GeometryNode();

	GeometryNode(
		model geometry
	);

	GeometryNode(
		std::string name, 
		std::shared_ptr<Node> parent
	);

	model getGeometry();

	void setGeometry(model geometry);

};

#endif // !GEOMERTY_NODE_HPP
