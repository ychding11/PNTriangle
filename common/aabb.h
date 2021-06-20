#pragma once

#include <glm/glm.hpp>

#include <string>
#include <sstream>
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
    void Extend(const glm::vec3& p);

    /// Retrieves the center of the AABB.
    glm::vec3 Center() const
    {
        return isNull() ? glm::vec3(0) : (mMin + mMax) * 0.5f;
    }

    /// Retrieves the diagonal vector (computed as mMax - mMin).
    /// If the AABB is NULL, then a vector of all zeros is returned.
    glm::vec3 Diagonal() const
    {
        return isNull() ? glm::vec3(0) : mMax - mMin;
    }

    /// Retrieves the longest edge.
    /// If the AABB is NULL, then 0 is returned.
    float LongestEdge() const
    {
        auto d = Diagonal();
        return std::max(d.x, std::max(d.y, d.z));
    }

    /// Retrieves the shortest edge.
    /// If the AABB is NULL, then 0 is returned.
    float ShortestEdge() const
    {
        auto d = Diagonal();
        return std::min(d.x, std::min(d.y, d.z));
    }

    /// Retrieves the AABB's minimum point.
    glm::vec3 Min() const { return mMin; }

    /// Retrieves the AABB's maximum point.
    glm::vec3 Max() const { return mMax; }

    std::string str() const
    {
        std::stringstream ss;
        ss  << " AABB : "
            << "[ "
            << mMin.x << ","
            << mMin.y << ","
            << mMin.z
            << " ] "
            << " ==> "
            << "[ "
            << mMax.x << ","
            << mMax.y << ","
            << mMax.z
            << " ] "
            ;
        return ss.str();
    }

private:

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
    Extend(p1);
    Extend(p2);
}

inline AABB::AABB(const AABB& aabb)
{
    setNull();
    Extend(aabb.mMin);
    Extend(aabb.mMax);
}

inline void AABB::Extend(const glm::vec3& p)
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


