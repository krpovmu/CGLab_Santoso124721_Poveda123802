#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include <iostream>
#include <memory>
#include "node.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"

class SceneGraph {
 public:
  // constructors
  SceneGraph();
  SceneGraph(std::string const& name);
  SceneGraph(std::string const& name, std::shared_ptr<Node> const& rootNode);

  // get attribute methods
  std::string getName()const;
  std::shared_ptr<Node> getRoot()const;
  std::list<std::shared_ptr<GeometryNode>> getPlanets()const;

  // add planet
  void createPlanet(std::shared_ptr<GeometryNode> planet);

  // print method
  std::string printGraph()const;

  void traverse(node_traverse_func);
  ~SceneGraph();
  static SceneGraph& getInstance();
  static std::shared_ptr<CameraNode> getActiveCamera();
 
 private:
  // set attribute methods
  void setName(std::string const& name);
  void setRoot(std::shared_ptr<Node> const& rootNode);

  // recursive print method for individual node
  std::string printNode(std::shared_ptr<Node> const& node)const;

  // attributes
  std::string name_;
  std::shared_ptr<Node> rootNode_;
  std::list<std::shared_ptr<GeometryNode>> planets_;
};

#endif
