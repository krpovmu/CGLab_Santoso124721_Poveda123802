#include "camera_node.hpp"

// constructors
CameraNode::CameraNode(){}
CameraNode::CameraNode(
    bool isPerspective, bool isEnabled, glm::mat4 const& projectionMatrix
    ) :
    isPerspective_(isPerspective), isEnabled_(isEnabled), projectionMatrix_(projectionMatrix)
{}
CameraNode::CameraNode(std::string const& name, std::shared_ptr<Node> const& parent, glm::fmat4 const& localTansform) : Node(name, parent, localTansform){}

// get attribute methods
bool CameraNode::getPerspective() const {
    return isPerspective_;
}
bool CameraNode::getEnabled() const {
    return isEnabled_;
}
glm::mat4 CameraNode::getProjectionMatrix() const {
    return projectionMatrix_;
}

// set attribute methods
void CameraNode::setEnabled(bool is_enabled) {
    isEnabled_ = is_enabled;
}
void CameraNode::setProjectionMatrix(glm::mat4 const& projection_matrix) {
    projectionMatrix_ = projection_matrix;
}
