#include "node.hpp"

Node::Node()
    : name_("root")
    , parent_(nullptr)
    , children_()
    , path_("")
    , depth_(0)
    , worldTransform_(glm::fmat4(1))
    , localTransform_(glm::fmat4(1))
{}
Node::Node(std::string const &name,
           std::shared_ptr<Node> const &parent,
           glm::fmat4 const &localTansform)
    : name_(name)
    , parent_(parent)
    , path_("")
    , depth_(parent->getDepth() + 1)
    , localTransform_(localTansform)
{
    setLocalTransform(localTansform);
}
Node::Node(std::string const &name,
           std::shared_ptr<Node> const &parent,
           std::list<std::shared_ptr<Node>> const &children,
           std::string const &path,
           glm::fmat4 const &localTansform)
    : name_(name)
    , parent_(parent)
    , children_(children)
    , path_(path)
    , depth_(parent->getDepth() + 1)
    , localTransform_(localTansform)
{
    setLocalTransform(localTansform);
}

// get attribute methods
std::string Node::getName()
{
    return name_;
}
std::shared_ptr<Node> Node::getParent()
{
    return parent_;
}
std::list<std::shared_ptr<Node>> Node::getChildren()
{
    return children_;
}
std::string Node::getPath()
{
    return path_;
}
int Node::getDepth()
{
    return depth_;
}
glm::fmat4 Node::getWorldTransform()
{
    return worldTransform_;
}
glm::fmat4 Node::getLocalTransform()
{
    return localTransform_;
}

Node *Node::getOrigin() const
{
    return origin_;
}

// get methods of derived class
float Node::getSize() const
{
    return 1.0f;
}
float Node::getSpeed() const
{
    return 1.0f;
}
float Node::getDistance() const
{
    return 1.0f;
}
bool Node::getIsLight()
{
    return isLight_;
}

// set methods
void Node::setParent(std::shared_ptr<Node> const &parent)
{
    parent_ = parent;
}
void Node::setWorldTransform(glm::fmat4 const &worldTransform)
{
    for (auto child : children_) {
        //child->setWorldTransform(worldTransform);
    }
    worldTransform_ = worldTransform * localTransform_;
}
void Node::setLocalTransform(glm::fmat4 const &localTransform)
{
    localTransform_ = localTransform;

    if (depth_ != 0) {
        worldTransform_ = parent_->getWorldTransform() * localTransform_;
    } else {
        worldTransform_ = localTransform_;
    }

    for (auto child : children_) {
        child->setWorldTransform(worldTransform_);
    }
}
void Node::setIsLight(bool isLight)
{
    isLight_ = isLight;
}

// get one specific child
std::shared_ptr<Node> Node::getChild(std::string const &childName)
{
    for (std::shared_ptr<Node> node : children_) {
        if (node->getName() == childName) {
            return node;
        }
    }
    return nullptr;
}

// add one child
void Node::addChild(std::shared_ptr<Node> const &node)
{
    children_.push_back(node);
}

// remove one child
void Node::removeChild(std::string const &childName)
{
    for (std::shared_ptr<Node> child : children_) {
        if (child->getName().compare(childName) == 0) {
            children_.remove(child);
        }
    }
}
