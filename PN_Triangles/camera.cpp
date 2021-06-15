
#include "camera.h"

/////////////////////////// Helper functions //////////////////////////////////

static inline float degreesToRadians(float degrees)
{
  return degrees * M_PI / 180.f;
}

static glm::vec3 azelToDirection(float az, float el, OrbitAxis axis)
{
  const float x = std::sin(az) * std::cos(el);
  const float y = std::cos(az) * std::cos(el);
  const float z = std::sin(el);
  switch (axis)
  {
  case OrbitAxis::POS_X:
    return -normalize(glm::vec3(z, y, x));
  case OrbitAxis::POS_Y:
    return -normalize(glm::vec3(x, z, y));
  case OrbitAxis::POS_Z:
    return -normalize(glm::vec3(x, y, z));
  case OrbitAxis::NEG_X:
    return normalize(glm::vec3(z, y, x));
  case OrbitAxis::NEG_Y:
    return normalize(glm::vec3(x, z, y));
  case OrbitAxis::NEG_Z:
    return normalize(glm::vec3(x, y, z));
  }
  return {};
}

static OrbitAxis negateAxis(OrbitAxis current)
{
  switch (current)
  {
  case OrbitAxis::POS_X:
    return OrbitAxis::NEG_X;
  case OrbitAxis::POS_Y:
    return OrbitAxis::NEG_Y;
  case OrbitAxis::POS_Z:
    return OrbitAxis::NEG_Z;
  case OrbitAxis::NEG_X:
    return OrbitAxis::POS_X;
  case OrbitAxis::NEG_Y:
    return OrbitAxis::POS_Y;
  case OrbitAxis::NEG_Z:
    return OrbitAxis::POS_Z;
  }
  return {};
}

static float maintainUnitCircle(float inDegrees)
{
  while (inDegrees > 360.f)
    inDegrees -= 360.f;
  while (inDegrees < 0.f)
    inDegrees += 360.f;
  return inDegrees;
}

////////////////////////// Class Camera definitions //////////////////////////////////

Camera::Camera(glm::vec3 at, float dist, glm::vec2 azel)
    : m_at(at), m_distance(dist), m_azel(azel)
{
  m_speed = m_distance;
  update();
}

void Camera::startNewRotation()
{
  m_invertRotation = m_azel.y > 90.f && m_azel.y < 270.f;
}

void Camera::rotate(glm::vec2 delta)
{
    delta *= 100;
    delta.x = m_invertRotation ? delta.x : -delta.x;
    delta.y = m_distance < 0.f ? -delta.y : delta.y;
    m_azel += delta;
    m_azel.x = maintainUnitCircle(m_azel.x);
    m_azel.y = maintainUnitCircle(m_azel.y);
    update();
}

void Camera::zoom(float delta)
{
    m_distance += m_speed * delta;
    update();
}

void Camera::pan(glm::vec2 delta)
{
    delta  *= m_speed;
    delta.y = -delta.y;

    const glm::vec3 amount = delta.x * m_right + delta.y * m_up;

    m_eye += amount;
    m_at += amount;

    update();
}

void Camera::setAxis(OrbitAxis axis)
{
  m_axis = axis;
  update();
}

glm::vec2 Camera::azel() const
{
  return m_azel;
}

glm::vec3 Camera::eye() const
{
  return m_eye;
}

glm::vec3 Camera::dir() const
{
  return glm::normalize(m_at - m_eye);
}

glm::vec3 Camera::up() const
{
  return m_up;
}

#include <cassert>
#include <limits>
void Camera::update()
{
    const float distance = std::abs(m_distance);

    const OrbitAxis axis = m_distance < 0.f ? negateAxis(m_axis) : m_axis;

    const float azimuth = degreesToRadians(m_azel.x);
    const float elevation = degreesToRadians(m_azel.y);

    const glm::vec3 toLocalOrbit = azelToDirection(azimuth, elevation, axis);

    const glm::vec3 localOrbitPos = toLocalOrbit * distance;
    const glm::vec3 fromLocalOrbit = -localOrbitPos;

    const glm::vec3 alteredElevation = azelToDirection(azimuth, elevation + 3, m_axis);

    const glm::vec3 cameraRight = glm::cross(toLocalOrbit, alteredElevation);
    const glm::vec3 cameraUp    = glm::cross(cameraRight, fromLocalOrbit);

    m_eye = localOrbitPos + m_at;
    m_up    = glm::normalize(cameraUp);
    m_right = glm::normalize(cameraRight);

    m_forward = this->dir();

    m_viewMatrix[0][0] = m_right.x;
    m_viewMatrix[1][0] = m_right.y;
    m_viewMatrix[2][0] = m_right.z;
    m_viewMatrix[0][1] = m_up.x;
    m_viewMatrix[1][1] = m_up.y;
    m_viewMatrix[2][1] = m_up.z;
    m_viewMatrix[0][2] = -m_forward.x;
    m_viewMatrix[1][2] = -m_forward.y;
    m_viewMatrix[2][2] = -m_forward.z;
    m_viewMatrix[3][0] = -dot(m_right, m_eye);
    m_viewMatrix[3][1] = -dot(m_up, m_eye);
    m_viewMatrix[3][2] =  dot(m_forward, m_eye);


    assert(std::fabs(m_aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

    float const tanHalfFovy = std::tanf( degreesToRadians(m_fovy * 0.5f) );

    m_projMatrix[0][0] = static_cast<float>(1) / (m_aspect * tanHalfFovy);
    m_projMatrix[1][1] = static_cast<float>(1) / (tanHalfFovy);
    m_projMatrix[2][2] = -(m_zFar + m_zNear) / (m_zFar - m_zNear);
    m_projMatrix[2][3] = -static_cast<float>(1);
    m_projMatrix[3][2] = -(static_cast<float>(2) * m_zFar * m_zNear) / (m_zFar - m_zNear);
}

glm::mat4 Camera::viewMatrix() const
{
    //< use RH coordinate
    return m_viewMatrix;
}

glm::mat4 Camera::projMatrix() const
{
    //< use RH coordinate
    return m_projMatrix;
}
