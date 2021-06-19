#pragma once

#include <glm/glm.hpp>

#include <algorithm>

class AABB
{
public:
    ~AABB() = default;

    AABB();

    /// Builds an AABB that contains the two points.
    AABB(const glm::vec3& p1, const glm::vec3& p2);

    AABB(const AABB& aabb);


    /// Expand the AABB to include point \p p.
    void extend(const glm::vec3& p);

    /// Retrieves the center of the AABB.
    glm::vec3 getCenter() const
    {
        return isNull() ? glm::vec3(0) : (mMin + mMax) * 0.5f;
    }

    /// Retrieves the diagonal vector (computed as mMax - mMin).
    /// If the AABB is NULL, then a vector of all zeros is returned.
    glm::vec3 getDiagonal() const
    {
        return isNull() ? glm::vec3(0) : mMax - mMin;
    }

    /// Retrieves the longest edge.
    /// If the AABB is NULL, then 0 is returned.
    float getLongestEdge() const
    {
        auto d = getDiagonal();
        return std::max(d.x, std::max(d.y, d.z));
    }

    /// Retrieves the shortest edge.
    /// If the AABB is NULL, then 0 is returned.
    float getShortestEdge() const
    {
        auto d = getDiagonal();
        return std::min(d.x, std::min(d.y, d.z));
    }

    /// Retrieves the AABB's minimum point.
    glm::vec3 getMin() const { return mMin; }

    /// Retrieves the AABB's maximum point.
    glm::vec3 getMax() const { return mMax; }

    /// Set the AABB as NULL (not set).
    void setNull()
    {
        mMin = glm::vec3(1.0); mMax = glm::vec3(-1.0);
    }

    /// Returns true if AABB is NULL (not set).
    bool isNull() const
    {
        return mMin.x > mMax.x || mMin.y > mMax.y || mMin.z > mMax.z;
    }

private:

    glm::vec3 mMin;   ///< Minimum point.
    glm::vec3 mMax;   ///< Maximum point.

};


inline AABB::AABB()
{
    setNull();
}

inline AABB::AABB(const glm::vec3& p1, const glm::vec3& p2)
{
    setNull();
    extend(p1);
    extend(p2);
}

inline AABB::AABB(const AABB& aabb)
{
    setNull();
    extend(aabb.mMin);
    extend(aabb.mMax);
}

inline void AABB::extend(const glm::vec3& p)
{
    if (isNull())
    {
        mMin = p;
        mMax = p;
    }
    else
    {
        mMin = glm::min(p, mMin);
        mMax = glm::max(p, mMax);
    }
}


