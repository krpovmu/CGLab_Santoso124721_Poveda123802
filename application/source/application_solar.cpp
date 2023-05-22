#include "application_solar.hpp"
#include "window_handler.hpp"

#include "model_loader.hpp"
#include "shader_loader.hpp"
#include "utils.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;
const double PI = 3.14159265358979323846;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>

ApplicationSolar::ApplicationSolar(std::string const &resource_path)
    : Application{resource_path}
    , planet_object{}
    , star_object{}
    , orbit_object{}
    , orbit_container{}
    , m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
    , m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
    initializeGeometry();
    initializeShaderPrograms();
    initializeSolarSystem();
    initializeStars();
    initializeOrbits();
}

ApplicationSolar::~ApplicationSolar()
{
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::initializeSolarSystem()
{
    // root node
    Node rootNode = Node();
    std::shared_ptr<Node> rootNodePtr = std::make_shared<Node>(rootNode);

    // create scene graph
    planetarySystem_ = SceneGraph("Solar System", rootNodePtr);

    // sun
    createNewPlanet("sun", rootNodePtr, 0.5f, 0.0f, 0.0f);

    // planets
    createNewPlanet("mercury", rootNodePtr, 0.09f, 0.5f, 1.0f);
    createNewPlanet("venus", rootNodePtr, 0.2f, 0.4f, 1.5f);
    createNewPlanet("earth", rootNodePtr, 0.2f, 0.3f, 2.5f);
    createNewPlanet("mars", rootNodePtr, 0.1f, 0.2f, 3.5f);
    createNewPlanet("jupiter", rootNodePtr, 0.4f, 0.09f, 5.0f);
    createNewPlanet("saturn", rootNodePtr, 0.4f, 0.10f, 7.0f);
    createNewPlanet("uranus", rootNodePtr, 0.3f, 0.05f, 9.0f);
    createNewPlanet("neptune", rootNodePtr, 0.3f, 0.04f, 10.0f);
    createNewPlanet("pluto", rootNodePtr, 0.04f, 0.06f, 10.5f);

    // moons
    std::shared_ptr<Node> earthHolderPtr = rootNodePtr->getChild("earth holder");
    createNewPlanet("moon", earthHolderPtr, 1.0f, 1.3f, 0.6f);

    // camera
    CameraNode camera = CameraNode("camera", rootNodePtr, glm::fmat4(1));
    std::shared_ptr<CameraNode> cameraPointer = std::make_shared<CameraNode>(camera);
    rootNodePtr->addChild(cameraPointer);

    std::cout << planetarySystem_.printGraph() << std::endl;
}

void ApplicationSolar::createNewPlanet(std::string const &name,
                                       std::shared_ptr<Node> const &parent,
                                       float size,
                                       float speed,
                                       float distance)
{
    // set up local transform matrix
    glm::fmat4 localTransform = parent->getWorldTransform();
    localTransform = glm::translate(localTransform, glm::fvec3{0.0f, 0.0f, distance});
    localTransform = glm::rotate(localTransform,
                                 float(glfwGetTime()) * speed,
                                 glm::fvec3{0.0f, 1.0f, 0.0f});
    localTransform = glm::scale(localTransform, glm::fvec3{size, size, size});

    // create holder node
    Node planetHolder = Node(name + " holder", parent, localTransform);
    std::shared_ptr<Node> planetHolderPtr = std::make_shared<Node>(planetHolder);
    parent->addChild(planetHolderPtr);

    // create geometry node
    GeometryNode planet = GeometryNode(name, planetHolderPtr, glm::fmat4(1), size, speed, distance);
    std::shared_ptr<GeometryNode> planetPtr = std::make_shared<GeometryNode>(planet);
    planetHolderPtr->addChild(planetPtr);

    planetarySystem_.addPlanet(planetPtr);
}

void ApplicationSolar::render() const
{
    // render stars
    drawStarts();

    // render sun
    auto sun = std::dynamic_pointer_cast<GeometryNode>(
        planetarySystem_.getRoot()->getChild("sun holder")->getChild("sun"));
    renderPlanet(sun);

    // render planets
    auto planets = planetarySystem_.getPlanets();
    for (auto planet : planets) {
        renderPlanet(planet);
        drawOrbits(planet);
    }
}

void ApplicationSolar::initializeStars()
{
    // for loop to create 5000 stars
    for (int i = 0; i < 5000; ++i) {
        // random XYZ-value for position of the star
        GLfloat x = (rand() % 150) - 75.0f;
        GLfloat y = (rand() % 150) - 75.0f;
        GLfloat z = (rand() % 150) - 75.0f;
        stars_container.emplace_back(y);
        stars_container.emplace_back(x);
        stars_container.emplace_back(z);

        // random RGB-value for color of the star
        GLfloat red = (float)rand() / RAND_MAX;
        GLfloat green = (float)rand() / RAND_MAX;
        GLfloat blue = (float)rand() / RAND_MAX; 
        /*GLfloat red = ((rand() % 51) + 153) / 255.0f;
        GLfloat green = ((rand() % 80) + 110) / 255.0f;
        GLfloat blue = ((rand() % 64) + 140) / 255.0f;*/
        stars_container.emplace_back(red);
        stars_container.emplace_back(green);
        stars_container.emplace_back(blue);
    }
    // generation of the vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO); // (number of VAO, array which VAOs are stored)
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO); // (name of VA to bind)
    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO); // (number of VBO, array)
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER,
                 star_object.vertex_BO); // (target(purpose: Vertex attributes), buffer)
    // creates and initializes a buffer object's data store
    glBufferData(GL_ARRAY_BUFFER,
                 GLsizeiptr(sizeof(float) * stars_container.size()),
                 stars_container.data(),
                 GL_STATIC_DRAW);
    //Specify the attributes
    // atrribute 0 on GPU
    glEnableVertexAttribArray(0);
    // (index of attribute, number of components, type, normalized, stride = components*num_attributes, pointer to first component of the first attribute)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    // attribute 1 on GPU
    glEnableVertexAttribArray(1);
    // (....,start of the 2nd attribute is at index 3, type of this is void pointer)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *) (sizeof(float) * 3));

    // define draw mode to use when render
    star_object.draw_mode = GL_POINTS;
    // define number of elements: = size of the array devided by the number of componentsof attributes
    star_object.num_elements = GLsizei(stars_container.size() / 6);
}

void ApplicationSolar::drawStarts() const
{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);

    glm::fmat4 matrix = glm::fmat4();
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(matrix));

    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);

    // draw stars
    glDrawArrays(star_object.draw_mode, GLint(0), star_object.num_elements);
}

void ApplicationSolar::initializeOrbits()
{
    // drawing circle with vertex
    for (int i = 0; i < 361; ++i) {
        //current angle (first angle is 0)
        float angle = float(i) * float(PI) / 180.0f;
        GLfloat x = cos(angle);
        GLfloat y = 0.0f;
        GLfloat z = sin(angle);
        // add index to container
        if (x == 0 && z == 0) {
            continue;
        } else {
            orbit_container.emplace_back(x);
            orbit_container.emplace_back(y);
            orbit_container.emplace_back(z);
        }
    }
    // generate vertex array object
    glGenVertexArrays(1, &orbit_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(orbit_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &orbit_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    // creates and initializes a buffer object's data store
    glBufferData(GL_ARRAY_BUFFER,
                 GLsizeiptr(sizeof(float) * orbit_container.size()),
                 orbit_container.data(),
                 GL_STATIC_DRAW);

    // activate attribute 0 on GPU
    glEnableVertexAttribArray(0);
    // pass 3 floats for each orbit
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // primitive type to draw
    orbit_object.draw_mode = GL_LINE_LOOP;
    // number of indexs to model object
    orbit_object.num_elements = GLsizei(orbit_container.size() / 3);
}

//void ApplicationSolar::renderOrbits(Node *const &child_planet) const
void ApplicationSolar::drawOrbits(std::shared_ptr<GeometryNode> planet) const
{
    float radius = planet->getDistanceToOrigin().x;
    glm::fvec3 vectorToScale = {radius, radius, radius};
    glm::fmat4 orbitMatrix = glm::fmat4{};

    //rotation of the orbits of the moon, so it moves like the parent planet
    if (planet->getDepth() == 4) {
        glm::fmat4 parentMatrix = planet->getOrigin()->getLocalTransform();
        orbitMatrix = glm::rotate(parentMatrix,
                                  float(glfwGetTime()) * (planet->getOrigin()->getSpeed()),
                                  glm::fvec3{0.0f, 1.5f, 0.0f});
        orbitMatrix = glm::translate(orbitMatrix,
                                     -1.0f * planet->getDistanceToOrigin());
    }
    // scale the orbit with the radius
    orbitMatrix = glm::scale(orbitMatrix * planet->getLocalTransform(), vectorToScale);

    // bind shader to upload uniform
    glUseProgram(m_shaders.at("orbits").handle);
    // matrixes to shader
    glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("OrbitMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(orbitMatrix));
    // bind VAO to draw
    glBindVertexArray(orbit_object.vertex_AO);
    // draw bound vertex array using bound shader
    glDrawArrays(orbit_object.draw_mode, 0, planet_object.num_elements);
}

void ApplicationSolar::renderPlanet(std::shared_ptr<GeometryNode> planet) const
{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 planetWorldTransform = planet->getParent()->getWorldTransform();
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(planetWorldTransform));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 planet_normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)
                                                            * planetWorldTransform);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(planet_normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    // calculate next step
    //planetWorldTransform = glm::translate(solarSystem_.getRoot()->getWorldTransform(), glm::fvec3{0.0f, 0.0f, planet->getDistance()});
    planetWorldTransform = glm::rotate(planet->getParent()->getParent()->getWorldTransform(),
                                       float(glfwGetTime()) * planet->getSpeed(),
                                       glm::fvec3{0.0f, 1.0f, 0.0f});
    planet->getParent()->setWorldTransform(planetWorldTransform);
}

void ApplicationSolar::uploadView(std::string shader_name)
{
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ViewMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection(std::string shader_name)
{
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at(shader_name).u_locs.at("ProjectionMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms()
{
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

    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("orbits").handle);
    // upload uniform values to new locations
    uploadView("orbits");
    uploadProjection("orbits");
}

// load shader sources
void ApplicationSolar::initializeShaderPrograms()
{
    // store shader simple
    m_shaders.emplace("planet",
                      shader_program{
                          {{GL_VERTEX_SHADER, m_resource_path + "shaders/simple.vert"},
                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

    // store shader stars
    m_shaders.emplace("star",
                      shader_program{
                          {{GL_VERTEX_SHADER, m_resource_path + "shaders/stars.vert"},
                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/stars.frag"}}});
    m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

    // store shader orbits
    m_shaders.emplace("orbits",
                      shader_program{
                          {{GL_VERTEX_SHADER, m_resource_path + "shaders/orbits.vert"},
                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
    m_shaders.at("orbits").u_locs["OrbitMatrix"] = -1;
    m_shaders.at("orbits").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbits").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry()
{
    model planetModel = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * planetModel.data.size(),
                 planetModel.data.data(),
                 GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0,
                          model::POSITION.components,
                          model::POSITION.type,
                          GL_FALSE,
                          planetModel.vertex_bytes,
                          planetModel.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1,
                          model::NORMAL.components,
                          model::NORMAL.type,
                          GL_FALSE,
                          planetModel.vertex_bytes,
                          planetModel.offsets[model::NORMAL]);

    // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 model::INDEX.size * planetModel.indices.size(),
                 planetModel.indices.data(),
                 GL_STATIC_DRAW);

    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planetModel.indices.size());
}

// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods)
{
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
    } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
    } else if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.1f, 0.0f});
    } else if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.1f, 0.0f});
    }
    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y)
{
    float horizontal_rotate = float(pos_x / 20); //divide to slow down
    float vertical_rotate = float(pos_y / 20);

    //glm rotate transforms a matrix 4x4, created from axis of 3 scalars and angle in degree (glm::radians). Scalars define the axis for rotation
    m_view_transform = glm::rotate(m_view_transform,
                                   glm::radians(vertical_rotate),
                                   glm::vec3{1.0f, 0.0f, 0.0f});
    m_view_transform = glm::rotate(m_view_transform,
                                   glm::radians(horizontal_rotate),
                                   glm::vec3{0.0f, 1.0f, 0.0f});

    glUseProgram(m_shaders.at("planet").handle);
    uploadView("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadView("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height)
{
    // recalculate projection matrix for new aspect ration
    m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
    // upload new projection matrices
    glUseProgram(m_shaders.at("planet").handle);
    uploadProjection("planet");
    glUseProgram(m_shaders.at("star").handle);
    uploadProjection("star");
    glUseProgram(m_shaders.at("orbits").handle);
    uploadView("orbits");
}

// exe entry point
int main(int argc, char *argv[])
{
    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
