#ifndef RECORDER_H
#define RECORDER_H

#include <vector>
#include <glm/glm.hpp>

class GeometryRecorder{
public:
    GeometryRecorder(int nverts);
    // write a vertex into the CPU buffer
    void record(glm::vec3 pos,
                glm::vec3 normal);
    // draw recorded points
    void draw();
    // empties the recording buffer.
    void clear();

private:
    int m_nverts;
    int m_current;
    std::vector<glm::vec3> m_position;
    std::vector<glm::vec3> m_normal;
};

#endif
