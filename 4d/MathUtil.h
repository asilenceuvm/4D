#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace MathUtil {
    //rotations around W planes
    inline glm::mat4 rotXW(float theta) {
        return glm::mat4(
            glm::vec4(cos(theta), 0, 0, sin(theta)),
            glm::vec4(0, 1, 0, 0),
            glm::vec4(0, 0, 1, 0),
            glm::vec4(-sin(theta), 0, 0, cos(theta))
        );
    }

    inline glm::mat4 rotYW(float theta) {
        return glm::mat4(
            glm::vec4(1, 0, 0, 0),
            glm::vec4(0, cos(theta), 0, -sin(theta)),
            glm::vec4(0, 0, 1, 0),
            glm::vec4(0, sin(theta), 0, cos(theta))
        );
    }

    inline glm::mat4 rotZW(float theta) {
        return glm::mat4(
            glm::vec4(1, 0, 0, 0),
            glm::vec4(0, 1, 0, 0),
            glm::vec4(0, 0, cos(theta), -sin(theta)),
            glm::vec4(0, 0, sin(theta), cos(theta))
        );
    }

    inline glm::mat4 doubleRot(float theta) {
        return glm::mat4(
            glm::vec4(cos(theta), -sin(theta), 0, 0),
            glm::vec4(sin(theta), cos(theta), 0, 0),
            glm::vec4(0, 0, cos(theta), -sin(theta)),
            glm::vec4(0, 0, sin(theta), cos(theta))
        );
    }

    /* 4d rotation function meant to act like glm's rotate
     * m is the matrix to rotate
     * rad is degrees in radians
     * u is the vector to rotate in x/y/z
     * w is the vector to rotate in xw/yw/zw
     * ex. m = rotate4D(m, glm::radians(45.0f), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
     * this would rotate it by 45 degrees in the z direction
     */
    inline void rotate4D(glm::mat4& m, float rad, glm::vec3 u, glm::vec3 w) {
        m = glm::rotate(m, glm::radians(rad), u);
        m = m * rotXW(w.x) * rotYW(w.y) * rotZW(w.z);
    }

    inline void translate4D(glm::mat4& m, glm::vec4 offset) {
        glm::mat4 translate = glm::mat4(
            glm::vec4(1, 0, 0, offset.x),
            glm::vec4(0, 1, 0, offset.y),
            glm::vec4(0, 0, 1, offset.z),
            glm::vec4(0, 0, 0, offset.w == 0 ? 1 : offset.w)
        );
        m *= translate;

    }
}
