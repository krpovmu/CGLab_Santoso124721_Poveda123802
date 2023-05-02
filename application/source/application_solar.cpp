#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  initializeShaderPrograms();
  createSolarSystem();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

// function to initialize a solarsystem 
// this can be moved to the render so we don't need to add the initializeSolarSystem here *maybe*
void ApplicationSolar::createSolarSystem(){
    // root
    //setting up a root node
    std::shared_ptr<Node> root_node = std::make_shared<Node>(Node());
    
    //setting up a Scenegraph for Solar System
    solarSystem_ = SceneGraph("Solar System", root_node);

    // setting up the sun as the first "planet with Point Light as the Sun Holder 
    std::shared_ptr<Node> point_light = std::make_shared<Node>(Node(root_node, "Point Light"));
    root_node->addChild(point_light);
    //setting up the geometry node for the Sun 
    std::shared_ptr<GeometryNode> sun = std::make_shared<GeometryNode>(GeometryNode(point_light, "sun"));
    point_light->addChild(sun);

    // planets
    // TO DO 
    std::list<std::string> listPlanetNames = {"mercury", "venus", "earth", "mars", "jupiter", "saturn", "uranus", "neptune"};

    for (std::string name : listPlanetNames) {
        Node planet_holder = Node(root_node, name + " holder");

        std::shared_ptr<Node> planet_holder_pointer = std::make_shared<Node>(planet_holder);

        root_node->addChild(planet_holder_pointer);

        GeometryNode planet = GeometryNode(planet_holder_pointer, name);

        std::shared_ptr<GeometryNode> planet_pointer = std::make_shared<GeometryNode>(planet);

        planet_holder_pointer->addChild(planet_pointer);
    }

    /*makePlanet("mercury", root_node);
    makePlanet("venus", root_node);
    makePlanet("earth", root_node);
    makePlanet("mars", root_node);
    makePlanet("jupiter", root_node);
    makePlanet("saturn", root_node);
    makePlanet("uranus", root_node);
    makePlanet("neptune", root_node);*/

    //special case for moon
    std::shared_ptr<Node> earth_holder = root_node->getChild("earth holder");
    //makePlanet("moon", earth_holder);
    Node moon_holder = Node(earth_holder, "moon holder");
    std::shared_ptr<Node> moon_holder_pointer = std::make_shared<Node>(moon_holder);
    earth_holder->addChild(moon_holder_pointer);
    GeometryNode moon = GeometryNode(moon_holder_pointer, "moon holder");
    std::shared_ptr<GeometryNode> moon_pointer = std::make_shared<GeometryNode>(moon);
    moon_holder_pointer->addChild(moon_pointer);

    // camera node
    std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>(CameraNode(root_node, "camera"));
    root_node->addChild(camera);

    std::cout << solarSystem_.printGraph() << std::endl;
}

//this function is to create a planet
void ApplicationSolar::makePlanet(std::string const& planetName, std::shared_ptr<Node> const& parentNode) {
    
    Node planet_holder = Node(parentNode, planetName + " holder");

    std::shared_ptr<Node> planet_holder_pointer = std::make_shared<Node>(planet_holder);

    parentNode->addChild(planet_holder_pointer);

    GeometryNode planet = GeometryNode(planet_holder_pointer, planetName);

    std::shared_ptr<GeometryNode> planet_pointer = std::make_shared<GeometryNode>(planet);

    planet_holder_pointer->addChild(planet_pointer);
}

// this is used to draw all object
void ApplicationSolar::render() const {
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{ 0.0f, 1.0f, 0.0f });
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, -1.0f });
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
        1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
        1, GL_FALSE, glm::value_ptr(normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    // use this as a recursion function
    // to check every node that has the root within root_node and planet_name_holder or earth_holder (this one for moon)
    // need to set the size and stuff there make it as a properties to be inserted using 'auto' in for
    // to be created in the scene
    renderPlanet();
}

// this function is for rendering the created planets
void ApplicationSolar::renderPlanet()const {
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 model_matrix = glm::scale(glm::fmat4{}, glm::fvec3{ 0.5f, 0.5f, 0.5f });

    model_matrix = glm::rotate(model_matrix, float(glfwGetTime()), glm::fvec3{ 0.0f, 1.0f, 0.0f });
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, -3.0f });

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
        1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
        1, GL_FALSE, glm::value_ptr(normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}


//TESTING 2.0

// TESTING 2.0
//void ApplicationSolar::makePlanet(std::string const& name, std::shared_ptr<Node> const& parent, float size, float speed, float distance){
  // set up local transform matrix
  //glm::fmat4 localTransform = parent->getWorldTransform();
  //localTransform = glm::translate(localTransform, glm::fvec3{0.0f, 0.0f, distance});
  //localTransform = glm::rotate(localTransform, float(glfwGetTime()) * speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  //localTransform = glm::scale(localTransform, glm::fvec3{size, size, size});
//
  // create holder node
  //Node planet_holder = Node(parent, name + " holder", localTransform);
  //std::shared_ptr<Node> planet_holder_pointer = std::make_shared<Node>(planet_holder);
  //parent->addChild(planet_holder_pointer);
//
  // create geometry node
  //GeometryNode planet = GeometryNode(planet_holder_pointer, name, glm::fmat4(1)/*, size, speed, distance*/);
  //std::shared_ptr<GeometryNode> planet_pointer = std::make_shared<GeometryNode>(planet);
  //planet_holder_pointer->addChild(planet_pointer);
  //solarSystem_.createPlanet(planet_pointer);
//}

//TESTING 2.0
//void ApplicationSolar::render() const {
//  // render sun
//  auto sun = std::dynamic_pointer_cast<GeometryNode>(solarSystem_.getRoot()->getChild("sun holder")->getChild("sun"));
//  renderPlanet(sun);
//
//  // render planets
//  /*auto planets = solarSystem_.getPlanets();
//  for (auto planet : planets){
//    renderPlanet(planet);
//  }*/
//}

//TESTING 2.0
//void ApplicationSolar::renderPlanet(std::shared_ptr<GeometryNode> planet)const{
//  // bind shader to upload uniforms
//  glUseProgram(m_shaders.at("planet").handle);
//
//  glm::fmat4 planetWorldTransform = planet->getParent()->getWorldTransform();
//  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
//                     1, GL_FALSE, glm::value_ptr(planetWorldTransform));
//  
//  // extra matrix for normal transformation to keep them orthogonal to surface
//  glm::fmat4 planet_normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * planetWorldTransform);
//  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
//                     1, GL_FALSE, glm::value_ptr(planet_normal_matrix));
//  
//  // bind the VAO to draw
//  glBindVertexArray(planet_object.vertex_AO);
//
//  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
//
//  // calculate next step
//  //planetWorldTransform = glm::translate(solarSystem_.getRoot()->getWorldTransform(), glm::fvec3{0.0f, 0.0f, planet->getDistance()});
//  
//  //rotation
//  //planetWorldTransform = glm::rotate(planet->getParent()->getParent()->getWorldTransform(), float(glfwGetTime()) * planet->getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f});
//  
//  planet->getParent()->setWorldTransform(planetWorldTransform);
//}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -0.1f, 0.0f, 0.0f });
      uploadView();
  }
  else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.1f, 0.0f, 0.0f });
      uploadView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
  //auto cam = SceneGraph::getActiveCamera();
  // mouse handling, x position movement
  //cam->rotate(
  //    glm::radians(float(pos_x / 50)), // angle
  //    glm::vec3{0.0f, -1.0f, 0.0f}    // axis
  //);
  //// mouse handling, y position movement
  //cam->rotate(
  //    glm::radians(float(pos_y / 50)), // angle
  //    glm::vec3{-1.0f, 0.0f, 0.0f}    // axis
  //);
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
