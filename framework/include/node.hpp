#ifndef NODE_HPP
#define NODE_HPP

//#include "node.hpp"
#include <../../external/glm-0.9.6.3/glm/ext.hpp>
#include <../../external/glm-0.9.6.3/glm/glm.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <model.hpp>

class Node
{
public:
    // constructors
    Node();
    Node(std::string const &name,
         std::shared_ptr<Node> const &parent,
         glm::fmat4 const &localTansform);
    Node(std::string const &name,
         std::shared_ptr<Node> const &parent,
         std::list<std::shared_ptr<Node>> const &children,
         std::string const &path,
         glm::fmat4 const &localTansform);

    // get attribute methods
    std::string getName();
    std::shared_ptr<Node> getParent();
    std::list<std::shared_ptr<Node>> getChildren();
    std::string getPath();
    int getDepth();
    glm::fmat4 getWorldTransform();
    glm::fmat4 getLocalTransform();

    // Distance to the origin
    Node *getOrigin() const;
    void setDistanceToOrigin(glm::fvec3 const &distance_to_origin);
    glm::fvec3 getDistanceToOrigin() const;

    // get methods of derived class
    virtual float getSize() const;
    virtual float getSpeed() const;
    virtual float getDistance() const;

    float getRadius() const;
    void setRadius(float const& radius);

    // set attribute methods
    void setParent(std::shared_ptr<Node> const &parent);
    void setWorldTransform(glm::fmat4 const &worldTransform);
    void setLocalTransform(glm::fmat4 const &localTransform);

    // child specific methods
    std::shared_ptr<Node> getChild(std::string const &childName);
    void addChild(std::shared_ptr<Node> const &);
    void removeChild(std::string const &childName);

private:
    // attributes
    std::string name_;
    std::shared_ptr<Node> parent_;
    Node *origin_;
    std::list<std::shared_ptr<Node>> children_;
    std::string path_;
    int depth_;
    glm::fmat4 worldTransform_;
    glm::fmat4 localTransform_;
    glm::fvec3 distance_to_origin_;
    float radius_;
};

#endif
