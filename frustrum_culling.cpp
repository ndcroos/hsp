/*

Frustum culling is the process of removing objects that are not visible in the current view of the camera. Octrees can be used to efficiently implement frustum culling by representing a 3D scene as a hierarchical tree structure, where each node in the tree represents a portion of the scene.

Here's a basic algorithm for implementing frustum culling using an octree:

1. Create an octree representation of the 3D scene, dividing the scene into smaller and smaller nodes until each node represents a manageable portion of the scene.

2. For each node in the octree, calculate its bounding box.

3. For each node in the octree, perform a simple intersection test between the node's bounding box and the view frustum. This can be done by testing the six planes of the view frustum against the eight vertices of the bounding box.

4. If a node is completely inside the view frustum, render all of its child nodes and objects.

5. If a node is completely outside the view frustum, discard it and don't render any of its child nodes or objects.

6. If a node is partially inside the view frustum, render all of its child nodes and objects.

By using an octree, you can significantly reduce the number of objects that need to be considered for frustum culling, since only nodes near the camera are processed in detail. This can result in significant performance gains, especially in large and complex scenes.

*/

#include <vector>

#include <glm/glm.hpp>

struct BoundingBox {
  glm::vec3 min, max;
};

struct OctreeNode {
  BoundingBox boundingBox;
  std::vector < OctreeNode > children;
  std::vector < int > objects; // indices of objects contained in this node
};

class Octree {
  public: Octree(const std::vector < glm::vec3 > & vertices, int maxDepth);

  void FrustumCull(const std::vector < glm::vec4 > & frustumPlanes);

  private: void BuildTree(int nodeIndex, int depth, BoundingBox boundingBox);
  bool IntersectsFrustum(const BoundingBox & boundingBox,
    const std::vector < glm::vec4 > & frustumPlanes) const;

  std::vector < OctreeNode > nodes_;
  std::vector < int > visibleNodes_;
};

Octree::Octree(const std::vector < glm::vec3 > & vertices, int maxDepth) {
  // Calculate the bounding box of the entire scene
  BoundingBox sceneBoundingBox;
  sceneBoundingBox.min = vertices[0];
  sceneBoundingBox.max = vertices[0];
  for (const auto & vertex: vertices) {
    sceneBoundingBox.min = glm::min(sceneBoundingBox.min, vertex);
    sceneBoundingBox.max = glm::max(sceneBoundingBox.max, vertex);
  }

  // Create the root node and build the octree
  nodes_.push_back({});
  BuildTree(0, 0, sceneBoundingBox);
}

void Octree::FrustumCull(const std::vector < glm::vec4 > & frustumPlanes) {
  visibleNodes_.clear();

  // Perform the frustum culling
  FrustumCullRecursive(0, frustumPlanes);
}

void Octree::BuildTree(int nodeIndex, int depth, BoundingBox boundingBox) {
  auto & node = nodes_[nodeIndex];
  node.boundingBox = boundingBox;

  if (depth == maxDepth) {
    // This is a leaf node; add objects to this node
    // ...
    return;
  }

  // Divide the node into eight smaller nodes
  const glm::vec3 center = (boundingBox.min + boundingBox.max) / 2.0 f;
  const glm::vec3 extents = (boundingBox.max - boundingBox.min) / 2.0 f;

  // Create child nodes
  node.children.resize(8);
  for (int z = 0; z < 2; ++z) {
    for (int y = 0; y < 2; ++y) {
      for (int x = 0; x < 2; ++x) {
        int childIndex = x + y * 2 + z * 4;
        BoundingBox childBoundingBox;
        childBoundingBox.min = center - extents + glm::vec3(x * extents.x, y * extents.y, z * extents.z);
        m::vec3(x * extents.x, y * extents.y, z * extents.z);
        childBoundingBox.max = center + extents + glm::vec3(x * extents.x, y * extents.y, z * extents.z);
        node.children[childIndex].boundingBox = childBoundingBox;
        BuildTree(nodeIndex + childIndex, depth + 1, childBoundingBox);
      }
    }
  }
}

bool Octree::IntersectsFrustum(const BoundingBox & boundingBox,
  const std::vectorglm::vec4 & frustumPlanes) const {
  for (const auto & plane: frustumPlanes) {
    int out = 0;
    out += (plane.x * boundingBox.min.x + plane.w > 0) ? 1 : 0;
    out += (plane.x * boundingBox.max.x + plane.w > 0) ? 1 : 0;
    out += (plane.y * boundingBox.min.y + plane.w > 0) ? 1 : 0;
    out += (plane.y * boundingBox.max.y + plane.w > 0) ? 1 : 0;
    out += (plane.z * boundingBox.min.z + plane.w > 0) ? 1 : 0;
    out += (plane.z * boundingBox.max.z + plane.w > 0) ? 1 : 0;
    if (out == 6) {
      return false;
    }
  }

  return true;
}

void Octree::FrustumCullRecursive(int nodeIndex,
  const std::vectorglm::vec4 & frustumPlanes) {
  auto & node = nodes_[nodeIndex];
  if (!IntersectsFrustum(node.boundingBox, frustumPlanes)) {
    return;
  }

  visibleNodes_.push_back(nodeIndex);

  for (const auto & child: node.children) {
    FrustumCullRecursive(child.nodeIndex, frustumPlanes);
  }
}

// This is a simple implementation of frustum culling using an octree. You can further optimize this code by using a stack to avoid recursion, and by using a more efficient data structure for storing the visible nodes. You can also optimize the intersection test between the bounding box and the view frustum by using a more efficient method, such as the Separating Axis Theorem.
