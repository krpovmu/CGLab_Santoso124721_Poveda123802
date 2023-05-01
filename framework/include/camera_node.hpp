#pragma once
#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include<glm/glm.hpp>

class CameraNode : Public Node{

private:
    bool isPerspective;//bool isPerspective
    bool isEnabled;//boll isEnabled
    glm::fmat4 projectionMatrix;//mat4 projectionMatrix

public:
    CameraNode();

    CameraNode(
            bool isPerspective, 
            bool isEnabled, 
            glm::fmat4 projectionMatrix
            );

    CameraNode(
            std::shared_ptr<Node> parent, 
            std::string name
            );

    bool getPerspective();
    bool getEnabled();
    glm::fmat4 getProjectionMatrix();

    void setEnabled(bool isEnabled);
    void setProjectionMatrix(glm::fmat4 projectionMatrix);


};

#endif // !CAMERA_NODE_HPP

