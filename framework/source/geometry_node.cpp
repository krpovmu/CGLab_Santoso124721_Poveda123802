#include "geometry_node.hpp"

GeometryNode::GeometryNode() {}
GeometryNode::GeometryNode(model const &geometry)
    : geometry_(geometry)
{}
GeometryNode::GeometryNode(std::string const &name,
                           std::shared_ptr<Node> const &parent,
                           glm::fmat4 const &localTansform,
                           float size,
                           float speed,
                           float distance,
                           glm::fvec3 color,
                           std::string texture,
                           int index)
    : Node(name, parent, localTansform)
    , size_(size)
    , speed_(speed)
    , distance_(distance)
    , distance_to_origin_(distance)
    //, color_(glm::normalize(color))
    , color_(color),
    texture_(texture),
    index_(index)
{}

// get attribute methods
model GeometryNode::getGeometry() const
{
    return geometry_;
}
float GeometryNode::getSize() const
{
    return size_;
}
float GeometryNode::getSpeed() const
{
    return speed_;
}
float GeometryNode::getDistance() const
{
    return distance_;
}

// Distance to the center
void GeometryNode::setDistanceToOrigin(float distance)
{
    distance_to_origin_ = glm::fvec3{distance, 0.0f, 0.0f};
}

glm::fvec3 GeometryNode::getDistanceToOrigin() const
{
    return distance_to_origin_;
}
// set attribute methods
void GeometryNode::setGeometry(model const &geometry)
{
    geometry_ = geometry;
}

// set color
glm::fvec3 GeometryNode::getColor() const
{
    return color_;
}
std::string GeometryNode::getTexture() const {
    return texture_;
}
texture_object GeometryNode::getTextureObject() const {
    return texture_object_;
}
int GeometryNode::getIndex() const {
    return index_;
}
void GeometryNode::setColor(glm::fvec3 const &color)
{
    color_ = color;
}
void GeometryNode::setTextureObject(texture_object texture_object){
    texture_object_ = texture_object;
}
