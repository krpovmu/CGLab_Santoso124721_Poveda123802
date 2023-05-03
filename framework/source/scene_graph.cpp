#include "scene_graph.hpp"
//#include "camera_node.hpp"

// constructors
SceneGraph::SceneGraph() : 
    name_("TestSceneGraph"), 
    rootNode_(std::make_shared<Node>(Node())) 
{}

SceneGraph::SceneGraph(
    std::string const& name
) : 
    name_(name), 
    rootNode_(std::make_shared<Node>(Node()))
{}

SceneGraph::SceneGraph(
    std::string const& name, 
    std::shared_ptr<Node> const& rootNode
) : 
    name_(name), 
    rootNode_(rootNode)
{}

// get attribute methods
std::string SceneGraph::getName()const{
    return name_;
}
std::shared_ptr<Node> SceneGraph::getRoot()const{
    return rootNode_;
}

// set attribute methods
void SceneGraph::setName(std::string const& name){
    name_ = name;
}
void SceneGraph::setRoot(std::shared_ptr<Node> const& rootNode){
    rootNode_ = rootNode;
}

// print methods
std::string SceneGraph::printGraph()const{
    return "Name: " + name_ + ", Nodes: " + printNode(rootNode_);
}
// Print the nodes recursively
std::string SceneGraph::printNode(std::shared_ptr<Node> const& node)const{
    
    std::string outString = node->getName();

    std::cout << " : INITIAL : " << std::endl;
    std::cout << outString << std::endl;

    // print every child node
    std::list<std::shared_ptr<Node>> children = node->getChildren();
    
    if (children.size() > 0) {
        outString.append(" -> (");
        std::cout << " : 1st appent : " << std::endl;
        std::cout << outString << std::endl;

        for (auto child : children){
            outString.append(printNode(child) + ", ");
            std::cout << " : repetition appent : " << std::endl;
            std::cout << outString << std::endl;
        }
        // remove last comma
        outString = outString.substr(0, outString.size() - 2);
        std::cout << " : substring : " << std::endl;
        std::cout << outString << std::endl;

        outString.append(")");
        std::cout << " : last append : " << std::endl;
        std::cout << outString << std::endl;

    }

    // return string
    return outString;
}
