#pragma once

#include "soc_model.hpp"

// std
#include <memory>

namespace soc {

struct Transform2dComponent {
  glm::vec2 translation{};  // (position offset)
  glm::vec2 scale{1.f, 1.f};  
  float rotation;

  glm::mat2 mat2() {
    //二维旋转矩阵
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMatrix{{c, s}, {-s, c}};
    //缩放矩阵
    glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};

    return rotMatrix * scaleMat;
  }
};

class SocGameObject {
 public:
  using id_t = unsigned int;

  static SocGameObject createGameObject() {
    static id_t currentId = 0;
    return SocGameObject{currentId++};
  }

  SocGameObject(const SocGameObject &) = delete;
  SocGameObject &operator=(const SocGameObject &) = delete;
  SocGameObject(SocGameObject &&) = default;
  SocGameObject &operator=(SocGameObject &&) = default;

  id_t getId() { return id; }

  
  std::shared_ptr<SocModel> model{};

  glm::vec3 color{};
  //变换矩阵相关
  Transform2dComponent transform2d{};

 private:
  SocGameObject(id_t objId) : id{objId} {}

  id_t id;
};
}  // namespace lve
