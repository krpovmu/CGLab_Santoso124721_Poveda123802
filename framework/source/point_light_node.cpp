#include "point_light_node.hpp"

// constructors
PointLightNode::PointLightNode() {}
PointLightNode::PointLightNode(std::string const &name,
                               std::shared_ptr<Node> const &parent,
                               glm::fmat4 const &localTansform,
                               float lightIntensity,
                               glm::fvec3 lightColor)
    : Node(name, parent, localTansform)
    , lightIntensity_(lightIntensity)
    , lightColor_(glm::normalize(lightColor))
{
    setIsLight(true);
}

// get attribute methods
float PointLightNode::getIntensity() const
{
    return lightIntensity_;
}
glm::fvec3 PointLightNode::getColor() const
{
    return lightColor_;
}

// set attribute methods
void PointLightNode::setIntensity(float lightIntensity)
{
    lightIntensity_ = lightIntensity;
}
void PointLightNode::setColor(glm::fvec3 lightColor)
{
    lightColor_ = lightColor;
}
