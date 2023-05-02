#include "scene_graph.hpp"
//#include "camera_node.hpp"

// constructors
SceneGraph::SceneGraph() : name_("DefaultSceneGraph"), rootNode_(std::make_shared<Node>(Node())) {}
SceneGraph::SceneGraph(std::string const& name) : name_(name), rootNode_(std::make_shared<Node>(Node())){}
SceneGraph::SceneGraph(std::string const& name, std::shared_ptr<Node> const& rootNode) : name_(name), rootNode_(rootNode){}
//SceneGraph::SceneGraph() : rootNode_{std::make_shared<Node>(Node{nullptr, "rootNode"})} {}

// get attribute methods
std::string SceneGraph::getName()const{
    return name_;
}
std::shared_ptr<Node> SceneGraph::getRoot()const{
    return rootNode_;
}
std::list<std::shared_ptr<GeometryNode>> SceneGraph::getPlanets()const{
    return planets_;
}

// set attribute methods
void SceneGraph::setName(std::string const& name){
    name_ = name;
}
void SceneGraph::setRoot(std::shared_ptr<Node> const& rootNode){
    rootNode_ = rootNode;
}

// add planet
void SceneGraph::createPlanet(std::shared_ptr<GeometryNode> planet){
    planets_.push_back(planet);
}

//
std::shared_ptr<CameraNode> SceneGraph::getActiveCamera() {
  std::shared_ptr<CameraNode> camera;
  node_traverse_func find_camera = [&](std::shared_ptr<Node> node) {
    auto cam_node = std::dynamic_pointer_cast<CameraNode>(node);
    if (cam_node && cam_node->getEnabled()) {
      camera = cam_node;
    }
  };
  getInstance().traverse(find_camera);
  return camera;
}

// print methods
std::string SceneGraph::printGraph()const{
    return "Name: " + name_ + ", Nodes: " + printNode(rootNode_);
}

void SceneGraph::traverse(node_traverse_func func) {
    func(rootNode_);
    rootNode_->traverse(func);
}

std::string SceneGraph::printNode(std::shared_ptr<Node> const& node)const{
    // print name
    std::string outputString = node->getName()/* + "(" + glm::to_string(node->getWorldTransform()) + ")"*/;
    // print every child node
    std::list<std::shared_ptr<Node>> children = node->getChildren();
    if (children.size() > 0) {
        outputString.append(" -> (");
        for (auto child : children){
            outputString.append(printNode(child) + ", ");
        }
        // remove last comma
        outputString = outputString.substr(0, outputString.size()-2);
        outputString.append(")");
    }
    // return string
    return outputString;
}

SceneGraph::~SceneGraph() {}

SceneGraph& SceneGraph::getInstance() {
    static SceneGraph instance;
    return instance;
}

