#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <glm/glm.hpp>
#include <memory>
#include <string>

class Node {
	private:
		std::shared_ptr<Node> parent_;//node parent
		std::list<std::shared_ptr<Node>> children_;//list node children
		std::string name_;//string name
		std::string path_;//string path
		int depth_;//int depth
		glm::fmat4 localTransform_;//mat4 localtransform
		glm::fmat4 worldTransform_;//mat4 worldtransform

	public:
		Node();
		
		Node(
			std::shared_ptr<Node> parent,
			std::string name
		);
		
		Node(
			std::shared_ptr<Node> parent,
			std::list<std::shared_ptr<Node>> children,
			std::string name,
			std::string path,
			glm::fmat4 localTansform,
			glm::fmat4 worldTransform
		);

		std::shared_ptr<Node> getParent();
		std::shared_ptr<Node> getChild(std::string childName);
		std::list<std::shared_ptr<Node>> getChildren();
		std::string getName();
		std::string getPath();
		int getDepth();
		glm::fmat4 getLocalTransform();
		glm::fmat4 getWorldTransform();

		void setParent(std::shared_ptr<Node> parent);
		void setLocalTransform(glm::fmat4 localTransform);
		void setWorldTransform(glm::fmat4 worldTransform);
		void addChild(std::shared_ptr<Node> childNode);
		void removeChild(std::string childName);

};

#endif // !NODE_HPP
