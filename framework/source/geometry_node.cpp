#include "geometry_node.hpp"

// constructors
GeometryNode::GeometryNode(){}

GeometryNode::GeometryNode(
	model const& geometry
) : 
	geometry_(geometry) 
{}

GeometryNode::GeometryNode(
	std::shared_ptr<Node> const& parent,
	std::string const& name
) :
	Node(parent, name)
  {}

// get attribute methods
model GeometryNode::getGeometry() const {
	return geometry_;
}

// set attribute methods
void GeometryNode::setGeometry(model const& geometry) {
	geometry_ = geometry;
}