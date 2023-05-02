#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <string>
#include <memory>
#include "node.hpp"

class CameraNode : public Node {
public:
    // constructors
    CameraNode();
    CameraNode(bool isPerspective, bool isEnabled, glm::fmat4 const& projectionMatrix);
    CameraNode(std::string const& name, std::shared_ptr<Node> const& parent, glm::fmat4 const& localTansform);
    
    // get attribute methods
    bool getPerspective() const;
    bool getEnabled() const;
    glm::fmat4 getProjectionMatrix() const;

    // set attribute methods
    void setEnabled(bool isEnabled);
    void setProjectionMatrix(glm::fmat4 const& projectionMatrix);

private:
    bool isPerspective_;
    bool isEnabled_;
    glm::fmat4 projectionMatrix_;
};

#endif