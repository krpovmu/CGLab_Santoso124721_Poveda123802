#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include "node.hpp"
#include <memory>
#include <string>

class GeometryNode : public Node
{
public:
    // constructors
    GeometryNode();
    GeometryNode(model const &geometry);
    GeometryNode(std::string const &name,
                 std::shared_ptr<Node> const &parent,
                 glm::fmat4 const &localTansform,
                 float size,
                 float speed,
                 float distance);

    // get attribute methods
    model getGeometry() const;
    float getSize() const;
    float getSpeed() const;
    float getDistance() const;

    void setDistanceToOrigin(float distance);
    glm::fvec3 getDistanceToOrigin() const;

    // set attribute methods
    void setGeometry(model const &geometry);

    // atribute radius
    float getRadius() const;
    void setRadius(float radius);

private:
    // attributes
    model geometry_;
    float size_;
    float speed_;
    float distance_;
    glm::fvec3 distance_to_origin_;
};

#endif
