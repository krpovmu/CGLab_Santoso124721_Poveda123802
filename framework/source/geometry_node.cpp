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
	//, glm::fmat4 const& localTansform
	/*, float size,
	float speed,
	float distance*/
) :
	Node(parent, name /*, localTansform*/)
	/*,size_(size),
	speed_(speed),
	distance_(distance_)*/
  {}

// get attribute methods
model GeometryNode::getGeometry() const {
	return geometry_;
}

//float GeometryNode::getSize() const {
//  return size_;
//}
//float GeometryNode::getSpeed() const {
//  return speed_;
//}
//float GeometryNode::getDistance() const {
//  return distance_;
//}

// set attribute methods
void GeometryNode::setGeometry(model const& geometry) {
	geometry_ = geometry;
}