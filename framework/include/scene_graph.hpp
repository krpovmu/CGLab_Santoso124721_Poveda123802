#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include <iostream>
#include <string>
#include <memory>
#include "node.hpp"

class SceneGraph {
private:
    // attributes
    std::string name_;
    std::shared_ptr<Node> rootNode_;

public:
    // constructors
    SceneGraph();

    SceneGraph(
            std::string const& name
            );

    SceneGraph(
            std::string const& name, 
            std::shared_ptr<Node> const& rootNode
            );

    // get attribute methods
    std::string getName() const;
    std::shared_ptr<Node> getRoot() const;

    // set attribute methods
    void setName(std::string const& name);
    void setRoot(std::shared_ptr<Node> const& rootNode);

    // print method
    std::string printGraph() const;

    // recursive print method for individual node
    std::string printNode(std::shared_ptr<Node> const& node) const;

};

#endif
