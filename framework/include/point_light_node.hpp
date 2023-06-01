#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include "node.hpp"
#include "structs.hpp"
#include <memory>
#include <string>

class PointLightNode : public Node
{
public:
    // constructors
    PointLightNode();
    PointLightNode(std::string const &name,
                   std::shared_ptr<Node> const &parent,
                   glm::fmat4 const &localTansform,
                   float lightIntensity,
                   glm::fvec3 lightColor);

    // get attribute methods
    float getIntensity() const;
    glm::fvec3 getColor() const;

    // set attribute methods
    void setIntensity(float lightIntensity);
    void setColor(glm::fvec3 lightColor);

private:
    // attributes
    float lightIntensity_;
    glm::fvec3 lightColor_;
};

#endif
