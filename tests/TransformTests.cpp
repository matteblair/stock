#include "catch.hpp"
#include "transform/Transform.hpp"
#include "glm/vector_relational.hpp"
#include "glm/gtc/epsilon.hpp"
#include "glm/gtx/io.hpp"
#include <sstream>

using namespace stock;

template<typename V>
class ApproxVec : public Catch::MatcherBase<V> {
  V target;
  float epsilon;

public:
  ApproxVec(V t, float e) : target(t), epsilon(e) {}

  virtual bool match(const V& v) const override {
    return glm::all(glm::epsilonEqual(v, target, epsilon));
  }

  virtual std::string describe() const override {
    std::ostringstream ss;
    ss << "is equal to " << target << " within " << epsilon;
    return ss.str();
  }
};

inline ApproxVec<glm::vec3> IsApproxEqualToVec3(glm::vec3 target, float epsilon) {
  return ApproxVec<glm::vec3>(target, epsilon);
}

TEST_CASE("Default-constructed Transform does not alter coordinates", "[Transform]") {
  Transform transform;
  glm::vec3 a(0.f, 0.f, 0.f), b(1.f, 1.f, 1.f), c(46.f, -112.5f, 0.f);

  CHECK(transform.convertLocalPointToWorld(a) == a);
  CHECK(transform.convertLocalPointToWorld(b) == b);
  CHECK(transform.convertLocalPointToWorld(c) == c);

  CHECK(transform.convertWorldPointToLocal(a) == a);
  CHECK(transform.convertWorldPointToLocal(b) == b);
  CHECK(transform.convertWorldPointToLocal(c) == c);
}

TEST_CASE("Convert point world to local and back", "[Transform]") {
  Transform transform;
  glm::vec3 a(0.f, 0.f, 0.f), b(1.f, 1.f, 1.f), c(46.f, -112.5f, 0.f);
  float epsilon = 0.001;

  SECTION("Transform with translation only") {
    transform.position() = glm::vec3(1.f, 2.f, 3.f);

    auto aa = transform.convertWorldPointToLocal(a);
    auto bb = transform.convertWorldPointToLocal(b);
    auto cc = transform.convertWorldPointToLocal(c);

    CHECK_THAT(transform.convertLocalPointToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }

  SECTION("Transform with rotation only") {
    transform.rotation() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(-1.f, 1.f, 1.f)));

    auto aa = transform.convertWorldPointToLocal(a);
    auto bb = transform.convertWorldPointToLocal(b);
    auto cc = transform.convertWorldPointToLocal(c);

    CHECK_THAT(transform.convertLocalPointToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }

  SECTION("Transform with translation and rotation") {
    transform.position() = glm::vec3(1.f, 2.f, 3.f);
    transform.rotation() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(-1.f, 1.f, 1.f)));

    auto aa = transform.convertWorldPointToLocal(a);
    auto bb = transform.convertWorldPointToLocal(b);
    auto cc = transform.convertWorldPointToLocal(c);

    CHECK_THAT(transform.convertLocalPointToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalPointToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }
}

TEST_CASE("Convert vector world to local and back", "[Transform]") {
  Transform transform;
  glm::vec3 a(0.f, 0.f, 0.f), b(1.f, 1.f, 1.f), c(46.f, -112.5f, 0.f);
  float epsilon = 0.001;

  SECTION("Transform with translation only") {
    transform.position() = glm::vec3(1.f, 2.f, 3.f);

    auto aa = transform.convertWorldVectorToLocal(a);
    auto bb = transform.convertWorldVectorToLocal(b);
    auto cc = transform.convertWorldVectorToLocal(c);

    CHECK_THAT(transform.convertLocalVectorToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }

  SECTION("Transform with rotation only") {
    transform.rotation() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(-1.f, 1.f, 1.f)));

    auto aa = transform.convertWorldVectorToLocal(a);
    auto bb = transform.convertWorldVectorToLocal(b);
    auto cc = transform.convertWorldVectorToLocal(c);

    CHECK_THAT(transform.convertLocalVectorToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }

  SECTION("Transform with translation and rotation") {
    transform.position() = glm::vec3(1.f, 2.f, 3.f);
    transform.rotation() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(-1.f, 1.f, 1.f)));

    auto aa = transform.convertWorldVectorToLocal(a);
    auto bb = transform.convertWorldVectorToLocal(b);
    auto cc = transform.convertWorldVectorToLocal(c);

    CHECK_THAT(transform.convertLocalVectorToWorld(aa), IsApproxEqualToVec3(a, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(bb), IsApproxEqualToVec3(b, epsilon));
    CHECK_THAT(transform.convertLocalVectorToWorld(cc), IsApproxEqualToVec3(c, epsilon));
  }
}

TEST_CASE("Conversion function and matrix return the same result", "[Transform]") {
  Transform transform;
  transform.position() = glm::vec3(1.f, 2.f, 3.f);
  transform.rotation() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(-1.f, 1.f, 1.f)));
  glm::vec3 a(0.f, 0.f, 0.f), b(1.f, 1.f, 1.f), c(46.f, -112.5f, 0.f);
  float epsilon = 0.001;
  auto worldToLocalMatrix = transform.getWorldToLocalMatrix();

  SECTION("Transforming points") {
    auto aa = transform.convertWorldPointToLocal(a);
    auto bb = transform.convertWorldPointToLocal(b);
    auto cc = transform.convertWorldPointToLocal(c);

    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(a, 1.f)), IsApproxEqualToVec3(aa, epsilon));
    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(b, 1.f)), IsApproxEqualToVec3(bb, epsilon));
    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(c, 1.f)), IsApproxEqualToVec3(cc, epsilon));
  }

  SECTION("Transforming vectors") {
    auto aa = transform.convertWorldVectorToLocal(a);
    auto bb = transform.convertWorldVectorToLocal(b);
    auto cc = transform.convertWorldVectorToLocal(c);

    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(a, 0.f)), IsApproxEqualToVec3(aa, epsilon));
    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(b, 0.f)), IsApproxEqualToVec3(bb, epsilon));
    CHECK_THAT(glm::vec3(worldToLocalMatrix * glm::vec4(c, 0.f)), IsApproxEqualToVec3(cc, epsilon));
  }
}

