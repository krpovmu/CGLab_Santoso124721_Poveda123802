#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "node.hpp"

class CameraNode : public Node {
private:
    bool isPerspective_;
    bool isEnabled_;
    glm::fmat4 projectionMatrix_;

public:
    // constructors
    CameraNode();

    CameraNode(
        bool isPerspective, 
        bool isEnabled, 
        glm::fmat4 const& projectionMatrix
    );

    CameraNode(
        std::shared_ptr<Node> const& parent,
        std::string const& name
    );
    
    // get attribute methods
    bool getPerspective() const;
    bool getEnabled() const;
    glm::fmat4 getProjectionMatrix() const;

    // set attribute methods
    void setEnabled(bool isEnabled);
    void setProjectionMatrix(glm::fmat4 const& projectionMatrix);

};

#endif