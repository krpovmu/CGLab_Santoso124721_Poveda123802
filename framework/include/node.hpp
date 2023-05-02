#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include "model.hpp"
#include "node.hpp"

//class Node;
//typedef std::function<void(std::shared_ptr<Node>)> node_traverse_func;

class Node {
private:
    // attributes
    std::shared_ptr<Node> parent_;
    std::list<std::shared_ptr<Node>> children_;
    std::string name_;
    std::string path_;
    int depth_;
    glm::fmat4 localTransform_;
    glm::fmat4 worldTransform_;

 public:
    // constructors
    Node();

    Node(
        std::shared_ptr<Node> const& parent,
        std::string const& name
        //, glm::fmat4 const& localTransform
    );

    Node(
        std::shared_ptr<Node> const& parent,
        std::list<std::shared_ptr<Node>> const& children,
        std::string const& name,
        std::string const& path,
        glm::fmat4 const& localTransform,
        glm::mat4 const& worldTransform
    );

    // getter
    std::shared_ptr<Node> getParent();
    std::shared_ptr<Node> getChild(std::string const& childName);
    std::list<std::shared_ptr<Node>> getChildren();
    std::string getName();
    std::string getPath();
    int getDepth();
    glm::fmat4 getWorldTransform();
    glm::fmat4 getLocalTransform();

    // setter
    void setParent(std::shared_ptr<Node> const& parent);
    void setWorldTransform(glm::fmat4 const& worldTransform);
    void setLocalTransform(glm::fmat4 const& localTransform);

    // add + remove child
    void addChild(std::shared_ptr<Node> const&);
    void removeChild(std::string const& childName);

    // get methods of derived class
    /*virtual float getSize() const;
    virtual float getSpeed() const;
    virtual float getDistance() const;*/

    //testing
    //void rotate(float angle, glm::vec3 const& axis);
 
};

#endif
