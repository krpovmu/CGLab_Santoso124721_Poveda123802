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
#include <random>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
    :Application{resource_path}
    ,planet_object{}
    ,star_object{}
    ,trajectory_object{}
    ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 2.0f})} // initial camera position
    ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
    initializeGeometry();
    initializeShaderPrograms();
    initializeSolarSystem();
    initializeStars();
    calculateTrajectory();
}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::initializeSolarSystem(){
    // root node
    Node root_node = Node();
    std::shared_ptr<Node> root_node_pointer = std::make_shared<Node>(root_node);

    // create scene graph
    solarSystem_ = SceneGraph("Solar System", root_node_pointer);

    // sun
    makePlanet("sun", root_node_pointer, 0.5f, 0.0f, 0.0f);

    // planets
    makePlanet("mercury", root_node_pointer, 0.09f, 0.5f, 1.0f);
    makePlanet("venus", root_node_pointer, 0.2f, 0.4f, 1.5f);
    makePlanet("earth", root_node_pointer, 0.2f, 0.3f, 2.5f);
    makePlanet("mars", root_node_pointer, 0.1f, 0.2f, 3.5f);
    makePlanet("jupiter", root_node_pointer, 0.4f, 0.09f, 5.0f);
    makePlanet("saturn", root_node_pointer, 0.4f, 0.10f, 7.0f);
    makePlanet("uranus", root_node_pointer, 0.3f, 0.05f, 9.0f);
    makePlanet("neptune", root_node_pointer, 0.3f, 0.04f, 10.0f);
    makePlanet("pluto", root_node_pointer, 0.04f, 0.06f, 10.5f);

    // moons
    std::shared_ptr<Node> earth_holder_pointer = root_node_pointer->getChild("earth holder");
    makePlanet("moon", earth_holder_pointer, 1.0f, 1.3f, 0.6f);

    // camera
    CameraNode camera = CameraNode("camera", root_node_pointer, glm::fmat4(1));
    std::shared_ptr<CameraNode> camera_pointer = std::make_shared<CameraNode>(camera);
    root_node_pointer->addChild(camera_pointer);

    std::cout << solarSystem_.printGraph() << std::endl;
}

void ApplicationSolar::makePlanet(std::string const& name, std::shared_ptr<Node> const& parent, float size, float speed, float distance){
    // set up local transform matrix
    glm::fmat4 localTransform = parent->getWorldTransform();
    localTransform = glm::translate(localTransform, glm::fvec3{0.0f, 0.0f, distance});
    localTransform = glm::rotate(localTransform, float(glfwGetTime()) * speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    localTransform = glm::scale(localTransform, glm::fvec3{size, size, size});

    // create holder node
    Node planet_holder = Node(name + " holder", parent, localTransform);
    std::shared_ptr<Node> planet_holder_pointer = std::make_shared<Node>(planet_holder);
    parent->addChild(planet_holder_pointer);

    // create geometry node
    GeometryNode planet = GeometryNode(name, planet_holder_pointer, glm::fmat4(1), size, speed, distance);
    std::shared_ptr<GeometryNode> planet_pointer = std::make_shared<GeometryNode>(planet);
    planet_holder_pointer->addChild(planet_pointer);

    solarSystem_.addPlanet(planet_pointer);
}

void ApplicationSolar::initializeStars(){
    int number_stars = 1000;
    for(int i = 0; i < number_stars; i++){
        // generate position
        stars_.push_back(float(rand() % 100 - 50));
        stars_.push_back(float(rand() % 100 - 50));
        stars_.push_back(- float(rand() % 100));
        // generate color
        stars_.push_back(float(rand() % 256));
        stars_.push_back(float(rand() % 256));
        stars_.push_back(float(rand() % 256));
    }

    // generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * number_stars * 6, stars_.data(), GL_STATIC_DRAW);

    // first attribute (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float) * 3), 0);

    // second attribute (color)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float) * 3), (void*) (sizeof(float)*3));

    // store type of primitive to draw
    star_object.draw_mode = GL_POINTS;
    // transfer number of indices to model object 
    star_object.num_elements = GLsizei(number_stars);
}

void ApplicationSolar::render() const {
    // render stars
    renderStars();

    // render sun
    auto sun = std::dynamic_pointer_cast<GeometryNode>(solarSystem_.getRoot()->getChild("sun holder")->getChild("sun"));
    renderPlanet(sun);

    // render planets
    auto planets = solarSystem_.getPlanets();
    for (auto planet : planets){
        renderPlanet(planet);
    }

    drawOrbits(solarSystem_);

}

void ApplicationSolar::renderStars()const{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);

    glm::fmat4 matrix = glm::fmat4();
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelMatrix"),
            1, GL_FALSE, glm::value_ptr(matrix));

    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);

    // draw stars
    glDrawArrays(star_object.draw_mode, GLint(0), star_object.num_elements);
}

void ApplicationSolar::renderPlanet(std::shared_ptr<GeometryNode> planet)const{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 planetWorldTransform = planet->getParent()->getWorldTransform();
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
            1, GL_FALSE, glm::value_ptr(planetWorldTransform));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 planet_normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * planetWorldTransform);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
            1, GL_FALSE, glm::value_ptr(planet_normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    // calculate next step
    //planetWorldTransform = glm::translate(solarSystem_.getRoot()->getWorldTransform(), glm::fvec3{0.0f, 0.0f, planet->getDistance()});
    planetWorldTransform = glm::rotate(planet->getParent()->getParent()->getWorldTransform(), float(glfwGetTime()) * planet->getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f});
    planet->getParent()->setWorldTransform(planetWorldTransform);
}


void ApplicationSolar::calculateTrajectory() {
    std::vector<float> orbits;
    uint points_in_circle = 360;

    for (int i = 0; i < points_in_circle; ++i) {
        GLfloat x = cos(i*M_PI/180); //calculating points on a circle in right order
        GLfloat y = 0;
        GLfloat z = sin(i*M_PI/180);
        orbits.push_back(x);
        orbits.push_back(y);
        orbits.push_back(z);
    }

    //Same as in draw stars
    glGenVertexArrays(1, &trajectory_object.vertex_AO);
    glBindVertexArray(trajectory_object.vertex_AO);

    glGenBuffers(1, &trajectory_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, trajectory_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float)*orbits.size()), orbits.data(), GL_STATIC_DRAW);

    //Attributes - index, size(3-dimensional), dtype, normalize data, byte-distance, offsets in bytes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*3), 0);

    trajectory_object.draw_mode = GL_LINE_LOOP;
    trajectory_object.num_elements = GLsizei(points_in_circle);
}

void ApplicationSolar::drawOrbits(SceneGraph const& scene) const{

    //go through planets and get distance and set as radius -> scale of circle. not yet correct
    //auto planets = solarSystem_.getPlanets();
    auto planets = scene.getPlanets();
    for (auto planet : planets){
        //for (auto const& planet : scene.geometry_nodes_) { 
        glm::fmat4 orbit_matrix = glm::fmat4{1.0f};
        float radius = planet->getDistanceToCenter();

        //orbit_matrix = glm::scale(orbit_matrix, glm::fvec3{radius, radius, radius});
        orbit_matrix = glm::scale(orbit_matrix, glm::fvec3{0.0f,1.0f,0.0f});
        //glUseProgram(m_shaders.at("orbit").handle);
        //glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("OrbitMatrix"), 1, GL_FALSE, glm::value_ptr(orbit_matrix)); 
        //glBindVertexArray(trajectory_object.vertex_AO); 
        //glDrawArrays(trajectory_object.draw_mode, GLint(0), trajectory_object.num_elements); 
    }
}

void ApplicationSolar::uploadView(std::string shader_name) {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ViewMatrix"),
            1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection(std::string shader_name) {
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ProjectionMatrix"),
            1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("planet").handle);
    // upload uniform values to new locations
    uploadView("planet");
    uploadProjection("planet");

    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("star").handle);
    // upload uniform values to new locations
    uploadView("star");
    uploadProjection("star");
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

    // store shader program objects in container
    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/vao.vert"},
            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/vao.frag"}}});
    m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
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
    }
    else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    }
    else if (key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
    }
    else if (key == GLFW_KEY_D  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
    }
    else if (key == GLFW_KEY_SPACE  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.1f, 0.0f});
    }
    else if (key == GLFW_KEY_LEFT_SHIFT  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.1f, 0.0f});
    }
    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y){
    float horizontal_rotate = float(pos_x/20);//divide to slowdown
    float vertical_rotate = float(pos_y/20);

    //glmrotatetransformsamatrix4x4,createdfromaxisof3scalarsandangleindegree(glm::radians).Scalarsdefinetheaxisforrotation
    m_view_transform = glm::rotate(m_view_transform,glm::radians(vertical_rotate),glm::vec3{1.0f,0.0f,0.0f});
    m_view_transform = glm::rotate(m_view_transform,glm::radians(horizontal_rotate),glm::vec3{0.0f,1.0f,0.0f});

    uploadView("planet");
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
    // recalculate projection matrix for new aspect ration
    m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
    // upload new projection matrices
    glUseProgram(m_shaders.at("planet").handle);
    uploadProjection("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadProjection("star");
}


// exe entry point
int main(int argc, char* argv[]) {
    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
