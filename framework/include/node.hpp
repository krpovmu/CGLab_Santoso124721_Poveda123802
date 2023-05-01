#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <glm/glm.hpp>

class Node {
	private:
		Node parent;//node parent
		std::list<Node> children;//list node children
		std::string name;//string name
		std::string path;//string path
		int depth;//int depth
		glm::mat4 localTransform;//mat4 localtransform
		glm::mat4 worldTransform;//mat4 worldtransform

	public:

};

#endif // !NODE_HPP
