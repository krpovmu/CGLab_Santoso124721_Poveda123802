#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scene_graph.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"
#include <glm/ext.hpp>

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;
  
  //NEW
  // draw single planet
  //void renderPlanet(std::shared_ptr<GeometryNode> planet)const;
  
  //NEW
  // create Scene Graph
  void createSolarSystem();
  // create/add planet
  void makePlanet(std::string const& name, std::shared_ptr<Node> const& parent);
  // render the planet
  //void renderPlanet(std::shared_ptr<Node> const& node) const;
  void renderPlanet(std::string name, glm::fvec3 scale, glm::vec3 translate) const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  // cpu representation of model
  model_object planet_object;
  
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;

  //NEW
  SceneGraph solarSystem_;
};

#endif