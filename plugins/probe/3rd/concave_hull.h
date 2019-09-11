/*
 * Software License Agreement(BSD License)
 *
 *  Point Cloud Library(PCL) - www.pointclouds.org
 *  Copyright(c) 2009-2012, Willow Garage, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 *
 */

#pragma once

#include <Eigen/src/Core/util/Memory.h>
#include <memory>
#include <vector>
#include "libqhull/libqhull.h"
#include <Eigen/src/Core/util/Macros.h>
#include <Eigen/StdVector>
#include <Eigen/Dense>
#include "vislib/sys/Log.h"

namespace pcl {

using Indices = std::vector<int>;
using IndicesPtr = std::shared_ptr<Indices>;
using IndicesConstPtr = std::shared_ptr<const Indices>;

struct Vertices {
    Vertices() {}

    std::vector<uint32_t> vertices;

public:
    using Ptr = std::shared_ptr<Vertices>;
    using ConstPtr = std::shared_ptr<const Vertices>;
}; // struct Vertices


struct PCLHeader {
    PCLHeader() : seq(0), stamp() {}

    /** \brief Sequence number */
    uint32_t seq;
    /** \brief A timestamp associated with the time when the data was acquired
     *
     * The value represents microseconds since 1970-01-01 00:00:00 (the UNIX epoch).
     */
    uint64_t stamp;
    /** \brief Coordinate frame ID */
    std::string frame_id;

    using Ptr = std::shared_ptr<PCLHeader>;
    using ConstPtr = std::shared_ptr<const PCLHeader>;
}; // struct PCLHeader


struct PointIndices {
    PointIndices() = default;

    PCLHeader header;

    std::vector<int> indices;

public:
    using Ptr = std::shared_ptr<PointIndices>;
    using ConstPtr = std::shared_ptr<const PointIndices>;
}; // struct PointIndices


///////////////////////////////////////////////////////////////////////////////////
template <typename PointT> class PointCloud {
public:
    PointCloud() = default;
    ~PointCloud() = default;
    /** \brief The point data. */
    std::vector<PointT, Eigen::aligned_allocator<PointT>> points;
    using Ptr = std::shared_ptr<PointCloud<PointT>>;
    using ConstPtr = std::shared_ptr<const PointCloud<PointT>>;

    /** \brief The point cloud header. It contains information about the acquisition time. */
    PCLHeader header;
    /** \brief The point cloud width (if organized as an image-structure). */
    uint32_t width;
    /** \brief The point cloud height (if organized as an image-structure). */
    uint32_t height;
    /** \brief True if no points are invalid (e.g., have NaN or Inf values in any of their floating point fields). */
    bool is_dense;
};


/////////////////////////////////////////////////////////////////////////////////////////
/** \brief PCL base class. Implements methods that are used by most PCL algorithms.
 * \ingroup common
 */
template <typename PointT> class PCLBase {
public:
    using PointCloud = pcl::PointCloud<PointT>;
    using PointCloudPtr = typename PointCloud::Ptr;
    using PointCloudConstPtr = typename PointCloud::ConstPtr;

    using PointIndicesPtr = std::shared_ptr<PointIndices>;
    using PointIndicesConstPtr = std::shared_ptr<PointIndices const>;

    /** \brief Empty constructor. */
    PCLBase();

    /** \brief Copy constructor. */
    PCLBase(const PCLBase& base);

protected:
    /** \brief Destructor. */
    ~PCLBase() {
        input_.reset();
        indices_.reset();
    }

public:
    /** \brief Provide a pointer to the input dataset
     * \param[in] cloud the const boost shared pointer to a PointCloud message
     */
    void setInputCloud(const PointCloudConstPtr& cloud);

    /** \brief Get a pointer to the input point cloud dataset. */
    inline PointCloudConstPtr const getInputCloud() const { return (input_); }

    /** \brief Provide a pointer to the vector of indices that represents the input data.
     * \param[in] indices a pointer to the indices that represent the input data.
     */
    virtual void setIndices(const IndicesPtr& indices);

    /** \brief Provide a pointer to the vector of indices that represents the input data.
     * \param[in] indices a pointer to the indices that represent the input data.
     */
    void setIndices(const IndicesConstPtr& indices);

    /** \brief Provide a pointer to the vector of indices that represents the input data.
     * \param[in] indices a pointer to the indices that represent the input data.
     */
    void setIndices(const PointIndicesConstPtr& indices);

    /** \brief Set the indices for the points laying within an interest region of
     * the point cloud.
     * \note you shouldn't call this method on unorganized point clouds!
     * \param[in] row_start the offset on rows
     * \param[in] col_start the offset on columns
     * \param[in] nb_rows the number of rows to be considered row_start included
     * \param[in] nb_cols the number of columns to be considered col_start included
     */
    void setIndices(size_t row_start, size_t col_start, size_t nb_rows, size_t nb_cols);

    /** \brief Get a pointer to the vector of indices used. */
    inline IndicesPtr const getIndices() { return (indices_); }

    /** \brief Get a pointer to the vector of indices used. */
    inline IndicesConstPtr const getIndices() const { return (indices_); }

    /** \brief Override PointCloud operator[] to shorten code
     * \note this method can be called instead of (*input_)[(*indices_)[pos]]
     * or input_->points[(*indices_)[pos]]
     * \param[in] pos position in indices_ vector
     */
    inline const PointT& operator[](size_t pos) const { return ((*input_)[(*indices_)[pos]]); }

protected:
    /** \brief The input point cloud dataset. */
    PointCloudConstPtr input_;

    /** \brief A pointer to the vector of point indices to use. */
    IndicesPtr indices_;

    /** \brief Set to true if point indices are used. */
    bool use_indices_;

    /** \brief If no set of indices are given, we construct a set of fake indices that mimic the input PointCloud. */
    bool fake_indices_;

    /** \brief This method should get called before starting the actual computation.
     *
     * Internally, initCompute() does the following:
     *   - checks if an input dataset is given, and returns false otherwise
     *   - checks whether a set of input indices has been given. Returns true if yes.
     *   - if no input indices have been given, a fake set is created, which will be used until:
     *     - either a new set is given via setIndices(), or
     *     - a new cloud is given that has a different set of points. This will trigger an update on the set of fake
     * indices
     */
    bool initCompute();

    /** \brief This method should get called after finishing the actual computation.
     */
    bool deinitCompute();

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


////////////////////////////////////////////////////////////////////////////////////////////
/** \brief @b ConcaveHull(alpha shapes) using libqhull library.
 * \author Aitor Aldoma
 * \ingroup surface
 */
template <typename PointInT> class ConcaveHull {
protected:
    using Ptr = std::shared_ptr<ConcaveHull<PointInT>>;
    using ConstPtr = std::shared_ptr<const ConcaveHull<PointInT>>;

public:
    using PointCloud = PointCloud<PointInT>;
    using PointCloudPtr = typename PointCloud::Ptr;
    using PointCloudConstPtr = typename PointCloud::ConstPtr;

    /** \brief Empty constructor. */
    ConcaveHull() : alpha_(0), keep_information_(false), voronoi_centers_(), dim_(0){};

    /** \brief Empty destructor */
    ~ConcaveHull() {}

    /** \brief Compute a concave hull for all points given
     *
     * \param points the resultant points lying on the concave hull
     * \param polygons the resultant concave hull polygons, as a set of
     * vertices. The Vertices structure contains an array of point indices.
     */
    void reconstruct(PointCloud& points, std::vector<pcl::Vertices>& polygons);

    /** \brief Set the alpha value, which limits the size of the resultant
     * hull segments(the smaller the more detailed the hull).
     *
     * \param alpha positive, non-zero value, defining the maximum length
     * from a vertex to the facet center(center of the voronoi cell).
     */
    inline void setAlpha(double alpha) { alpha_ = alpha; }

    /** \brief Returns the alpha parameter, see setAlpha(). */
    inline double getAlpha() { return (alpha_); }

    /** \brief If set, the voronoi cells center will be saved in _voronoi_centers_
     * \param voronoi_centers
     */
    inline void setVoronoiCenters(PointCloudPtr voronoi_centers) { voronoi_centers_ = voronoi_centers; }

    /** \brief If keep_information_is set to true the convex hull
     * points keep other information like rgb, normals, ...
     * \param value where to keep the information or not, default is false
     */
    void setKeepInformation(bool value) { keep_information_ = value; }

    /** \brief Returns the dimensionality(2 or 3) of the calculated hull. */
    inline int getDimension() const { return (dim_); }

    /** \brief Sets the dimension on the input data, 2D or 3D.
     * \param[in] dimension The dimension of the input data.  If not set, this will be determined automatically.
     */
    void setDimension(int dimension) {
        if ((dimension == 2) || (dimension == 3)) dim_ = dimension;
         else
        vislib::sys::Log::DefaultLog.WriteError("[pcl::%s::setDimension] Invalid input dimension specified!\n", getClassName().c_str());
    }

    /** \brief Retrieve the indices of the input point cloud that for the convex hull.
     *
     * \note Should only be called after reconstruction was performed and if the ConcaveHull is
     * set to preserve information via setKeepInformation().
     *
     * \param[out] hull_point_indices The indices of the points forming the point cloud
     */
    void getHullPointIndices(pcl::PointIndices& hull_point_indices) const;

protected:
    /** \brief Class get name method. */
    static std::string getClassName() { return ("ConcaveHull"); }

protected:
    /** \brief The actual reconstruction method.
     *
     * \param points the resultant points lying on the concave hull
     * \param polygons the resultant concave hull polygons, as a set of
     * vertices. The Vertices structure contains an array of point indices.
     */
    void performReconstruction(PointCloud& points, std::vector<pcl::Vertices>& polygons);


    /** \brief The method accepts facets only if the distance from any vertex to the facet->center
     *(center of the voronoi cell) is smaller than alpha
     */
    double alpha_;

    /** \brief If set to true, the reconstructed point cloud describing the hull is obtained from
     * the original input cloud by performing a nearest neighbor search from Qhull output.
     */
    bool keep_information_;

    /** \brief the centers of the voronoi cells */
    PointCloudPtr voronoi_centers_;

    /** \brief the dimensionality of the concave hull */
    int dim_;

    /** \brief vector containing the point cloud indices of the convex hull points. */
    PointIndices hull_indices_;
};



//#include "concave_hull.hpp"


//////////////////////////////////////////////////////////////////////////
template <typename PointInT>
void pcl::ConcaveHull<PointInT>::reconstruct(PointCloud& output, std::vector<pcl::Vertices>& polygons) {
    output.header = input_->header;
    if (alpha_ <= 0) {
        vislib::sys::Log::DefaultLog.WriteError("[pcl::%s::reconstruct] Alpha parameter must be set to a positive number!\n", getClassName().c_str());
        output.points.clear();
        return;
    }

    if (!initCompute()) {
        output.points.clear();
        return;
    }

    // Perform the actual surface reconstruction
    performReconstruction(output, polygons);

    output.width = static_cast<uint32_t>(output.points.size());
    output.height = 1;
    output.is_dense = true;

    deinitCompute();
}

#ifdef __GNUC__
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
//////////////////////////////////////////////////////////////////////////
template <typename PointInT>
void pcl::ConcaveHull<PointInT>::performReconstruction(PointCloud& alpha_shape, std::vector<pcl::Vertices>& polygons) {
    Eigen::Vector4d xyz_centroid;
    compute3DCentroid(*input_, *indices_, xyz_centroid);
    EIGEN_ALIGN16 Eigen::Matrix3d covariance_matrix = Eigen::Matrix3d::Zero();
    computeCovarianceMatrixNormalized(*input_, *indices_, xyz_centroid, covariance_matrix);

    // Check if the covariance matrix is finite or not.
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (!std::isfinite(covariance_matrix.coeffRef(i, j))) return;

    EIGEN_ALIGN16 Eigen::Vector3d eigen_values;
    EIGEN_ALIGN16 Eigen::Matrix3d eigen_vectors;
    pcl::eigen33(covariance_matrix, eigen_vectors, eigen_values);

    Eigen::Affine3d transform1;
    transform1.setIdentity();

    // If no input dimension is specified, determine automatically
    if (dim_ == 0) {
        PCL_DEBUG("[pcl::%s] WARNING: Input dimension not specified.  Automatically determining input dimension.\n",
            getClassName().c_str());
        if (std::abs(eigen_values[0]) < std::numeric_limits<double>::epsilon() ||
            std::abs(eigen_values[0] / eigen_values[2]) < 1.0e-3)
            dim_ = 2;
        else
            dim_ = 3;
    }

    if (dim_ == 2) {
        // we have points laying on a plane, using 2d convex hull
        // compute transformation bring eigen_vectors.col(i) to z-axis

        transform1(2, 0) = eigen_vectors(0, 0);
        transform1(2, 1) = eigen_vectors(1, 0);
        transform1(2, 2) = eigen_vectors(2, 0);

        transform1(1, 0) = eigen_vectors(0, 1);
        transform1(1, 1) = eigen_vectors(1, 1);
        transform1(1, 2) = eigen_vectors(2, 1);
        transform1(0, 0) = eigen_vectors(0, 2);
        transform1(0, 1) = eigen_vectors(1, 2);
        transform1(0, 2) = eigen_vectors(2, 2);
    } else {
        transform1.setIdentity();
    }

    PointCloud cloud_transformed;
    pcl::demeanPointCloud(*input_, *indices_, xyz_centroid, cloud_transformed);
    pcl::transformPointCloud(cloud_transformed, cloud_transformed, transform1);

    // True if qhull should free points in qh_freeqhull() or reallocation
    boolT ismalloc = True;
    // option flags for qhull, see qh_opt.htm
    char flags[] = "qhull d QJ";
    // output from qh_produce_output(), use NULL to skip qh_produce_output()
    FILE* outfile = nullptr;
    // error messages from qhull code
    FILE* errfile = stderr;
    // 0 if no error from qhull
    int exitcode;

    // Array of coordinates for each point
    coordT* points = reinterpret_cast<coordT*>(calloc(cloud_transformed.points.size() * dim_, sizeof(coordT)));

    for (size_t i = 0; i < cloud_transformed.points.size(); ++i) {
        points[i * dim_ + 0] = static_cast<coordT>(cloud_transformed.points[i].x);
        points[i * dim_ + 1] = static_cast<coordT>(cloud_transformed.points[i].y);

        if (dim_ > 2) points[i * dim_ + 2] = static_cast<coordT>(cloud_transformed.points[i].z);
    }

    // Compute concave hull
    exitcode = qh_new_qhull(
        dim_, static_cast<int>(cloud_transformed.points.size()), points, ismalloc, flags, outfile, errfile);

    if (exitcode != 0) {
        vislib::sys::Log::DefaultLog.WriteError("[pcl::%s::performReconstrution] ERROR: qhull was unable to compute a concave hull for the given "
                  "point cloud (%lu)!\n",
            getClassName().c_str(), cloud_transformed.points.size());

        // check if it fails because of NaN values...
        if (!cloud_transformed.is_dense) {
            bool NaNvalues = false;
            for (size_t i = 0; i < cloud_transformed.size(); ++i) {
                if (!std::isfinite(cloud_transformed.points[i].x) || !std::isfinite(cloud_transformed.points[i].y) ||
                    !std::isfinite(cloud_transformed.points[i].z)) {
                    NaNvalues = true;
                    break;
                }
            }

            if (NaNvalues)
                vislib::sys::Log::DefaultLog.WriteError("[pcl::%s::performReconstruction] ERROR: point cloud contains NaN values, consider running "
                          "pcl::PassThrough filter first to remove NaNs!\n",
                    getClassName().c_str());
        }

        alpha_shape.points.resize(0);
        alpha_shape.width = alpha_shape.height = 0;
        polygons.resize(0);

        qh_freeqhull(!qh_ALL);
        int curlong, totlong;
        qh_memfreeshort(&curlong, &totlong);

        return;
    }

    qh_setvoronoi_all();

    int num_vertices = qh num_vertices;
    alpha_shape.points.resize(num_vertices);

    vertexT* vertex;
    // Max vertex id
    int max_vertex_id = 0;
    FORALLvertices {
        if (vertex->id + 1 > unsigned(max_vertex_id)) max_vertex_id = vertex->id + 1;
    }

    facetT* facet; // set by FORALLfacets

    ++max_vertex_id;
    std::vector<int> qhid_to_pcidx(max_vertex_id);

    int num_facets = qh num_facets;
    int dd = 0;

    if (dim_ == 3) {
        setT* triangles_set = qh_settemp(4 * num_facets);
        if (voronoi_centers_) voronoi_centers_->points.resize(num_facets);

        int non_upper = 0;
        FORALLfacets {
            // Facets are tetrahedrons (3d)
            if (!facet->upperdelaunay) {
                vertexT* anyVertex = static_cast<vertexT*>(facet->vertices->e[0].p);
                double* center = facet->center;
                double r = qh_pointdist(anyVertex->point, center, dim_);
                facetT* neighb;

                if (voronoi_centers_) {
                    voronoi_centers_->points[non_upper].x = static_cast<float>(facet->center[0]);
                    voronoi_centers_->points[non_upper].y = static_cast<float>(facet->center[1]);
                    voronoi_centers_->points[non_upper].z = static_cast<float>(facet->center[2]);
                }

                non_upper++;

                if (r <= alpha_) {
                    // all triangles in tetrahedron are good, add them all to the alpha shape (triangles_set)
                    qh_makeridges(facet);
                    facet->good = true;
                    facet->visitid = qh visit_id;
                    ridgeT *ridge, **ridgep;
                    FOREACHridge_(facet->ridges) {
                        neighb = otherfacet_(ridge, facet);
                        if ((neighb->visitid != qh visit_id)) qh_setappend(&triangles_set, ridge);
                    }
                } else {
                    // consider individual triangles from the tetrahedron...
                    facet->good = false;
                    facet->visitid = qh visit_id;
                    qh_makeridges(facet);
                    ridgeT *ridge, **ridgep;
                    FOREACHridge_(facet->ridges) {
                        facetT* neighb;
                        neighb = otherfacet_(ridge, facet);
                        if ((neighb->visitid != qh visit_id)) {
                            // check if individual triangle is good and add it to triangles_set

                            PointInT a, b, c;
                            a.x = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[0].p))->point[0]);
                            a.y = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[0].p))->point[1]);
                            a.z = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[0].p))->point[2]);
                            b.x = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[1].p))->point[0]);
                            b.y = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[1].p))->point[1]);
                            b.z = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[1].p))->point[2]);
                            c.x = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[2].p))->point[0]);
                            c.y = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[2].p))->point[1]);
                            c.z = static_cast<float>((static_cast<vertexT*>(ridge->vertices->e[2].p))->point[2]);

                            double r = pcl::getCircumcircleRadius(a, b, c);
                            if (r <= alpha_) qh_setappend(&triangles_set, ridge);
                        }
                    }
                }
            }
        }

        if (voronoi_centers_) voronoi_centers_->points.resize(non_upper);

        // filter, add points to alpha_shape and create polygon structure

        int num_good_triangles = 0;
        ridgeT *ridge, **ridgep;
        FOREACHridge_(triangles_set) {
            if (ridge->bottom->upperdelaunay || ridge->top->upperdelaunay || !ridge->top->good || !ridge->bottom->good)
                num_good_triangles++;
        }

        polygons.resize(num_good_triangles);

        int vertices = 0;
        std::vector<bool> added_vertices(max_vertex_id, false);

        int triangles = 0;
        FOREACHridge_(triangles_set) {
            if (ridge->bottom->upperdelaunay || ridge->top->upperdelaunay || !ridge->top->good ||
                !ridge->bottom->good) {
                polygons[triangles].vertices.resize(3);
                int vertex_n, vertex_i;
                FOREACHvertex_i_((*ridge).vertices) // 3 vertices per ridge!
                {
                    if (!added_vertices[vertex->id]) {
                        alpha_shape.points[vertices].x = static_cast<float>(vertex->point[0]);
                        alpha_shape.points[vertices].y = static_cast<float>(vertex->point[1]);
                        alpha_shape.points[vertices].z = static_cast<float>(vertex->point[2]);

                        qhid_to_pcidx[vertex->id] = vertices; // map the vertex id of qhull to the point cloud index
                        added_vertices[vertex->id] = true;
                        vertices++;
                    }

                    polygons[triangles].vertices[vertex_i] = qhid_to_pcidx[vertex->id];
                }

                triangles++;
            }
        }

        alpha_shape.points.resize(vertices);
        alpha_shape.width = static_cast<uint32_t>(alpha_shape.points.size());
        alpha_shape.height = 1;
    } else {
        // Compute the alpha complex for the set of points
        // Filters the delaunay triangles
        setT* edges_set = qh_settemp(3 * num_facets);
        if (voronoi_centers_) voronoi_centers_->points.resize(num_facets);

        FORALLfacets {
            // Facets are the delaunay triangles (2d)
            if (!facet->upperdelaunay) {
                // Check if the distance from any vertex to the facet->center
                // (center of the voronoi cell) is smaller than alpha
                vertexT* anyVertex = static_cast<vertexT*>(facet->vertices->e[0].p);
                double r = (sqrt((anyVertex->point[0] - facet->center[0]) * (anyVertex->point[0] - facet->center[0]) +
                                 (anyVertex->point[1] - facet->center[1]) * (anyVertex->point[1] - facet->center[1])));
                if (r <= alpha_) {
                    pcl::Vertices facet_vertices; // TODO: is not used!!
                    qh_makeridges(facet);
                    facet->good = true;

                    ridgeT *ridge, **ridgep;
                    FOREACHridge_(facet->ridges) qh_setappend(&edges_set, ridge);

                    if (voronoi_centers_) {
                        voronoi_centers_->points[dd].x = static_cast<float>(facet->center[0]);
                        voronoi_centers_->points[dd].y = static_cast<float>(facet->center[1]);
                        voronoi_centers_->points[dd].z = 0.0f;
                    }

                    ++dd;
                } else
                    facet->good = false;
            }
        }

        int vertices = 0;
        std::vector<bool> added_vertices(max_vertex_id, false);
        std::map<int, std::vector<int>> edges;

        ridgeT *ridge, **ridgep;
        FOREACHridge_(edges_set) {
            if (ridge->bottom->upperdelaunay || ridge->top->upperdelaunay || !ridge->top->good ||
                !ridge->bottom->good) {
                int vertex_n, vertex_i;
                int vertices_in_ridge = 0;
                std::vector<int> pcd_indices;
                pcd_indices.resize(2);

                FOREACHvertex_i_((*ridge).vertices) // in 2-dim, 2 vertices per ridge!
                {
                    if (!added_vertices[vertex->id]) {
                        alpha_shape.points[vertices].x = static_cast<float>(vertex->point[0]);
                        alpha_shape.points[vertices].y = static_cast<float>(vertex->point[1]);

                        if (dim_ > 2)
                            alpha_shape.points[vertices].z = static_cast<float>(vertex->point[2]);
                        else
                            alpha_shape.points[vertices].z = 0;

                        qhid_to_pcidx[vertex->id] = vertices; // map the vertex id of qhull to the point cloud index
                        added_vertices[vertex->id] = true;
                        pcd_indices[vertices_in_ridge] = vertices;
                        vertices++;
                    } else {
                        pcd_indices[vertices_in_ridge] = qhid_to_pcidx[vertex->id];
                    }

                    vertices_in_ridge++;
                }

                // make edges bidirectional and pointing to alpha_shape pointcloud...
                edges[pcd_indices[0]].push_back(pcd_indices[1]);
                edges[pcd_indices[1]].push_back(pcd_indices[0]);
            }
        }

        alpha_shape.points.resize(vertices);

        std::vector<std::vector<int>> connected;
        PointCloud alpha_shape_sorted;
        alpha_shape_sorted.points.resize(vertices);

        // iterate over edges until they are empty!
        std::map<int, std::vector<int>>::iterator curr = edges.begin();
        int next = -1;
        std::vector<bool> used(vertices, false); // used to decide which direction should we take!
        std::vector<int> pcd_idx_start_polygons;
        pcd_idx_start_polygons.push_back(0);

        // start following edges and removing elements
        int sorted_idx = 0;
        while (!edges.empty()) {
            alpha_shape_sorted.points[sorted_idx] = alpha_shape.points[(*curr).first];
            // check where we can go from (*curr).first
            for (const int& i : (*curr).second) {
                if (!used[i]) {
                    // we can go there
                    next = i;
                    break;
                }
            }

            used[(*curr).first] = true;
            edges.erase(curr); // remove edges starting from curr

            sorted_idx++;

            if (edges.empty()) break;

            // reassign current
            curr = edges.find(next); // if next is not found, then we have unconnected polygons.
            if (curr == edges.end()) {
                // set current to any of the remaining in edge!
                curr = edges.begin();
                pcd_idx_start_polygons.push_back(sorted_idx);
            }
        }

        pcd_idx_start_polygons.push_back(sorted_idx);

        alpha_shape.points = alpha_shape_sorted.points;

        polygons.reserve(pcd_idx_start_polygons.size() - 1);

        for (size_t poly_id = 0; poly_id < pcd_idx_start_polygons.size() - 1; poly_id++) {
            // Check if we actually have a polygon, and not some degenerated output from QHull
            if (pcd_idx_start_polygons[poly_id + 1] - pcd_idx_start_polygons[poly_id] >= 3) {
                pcl::Vertices vertices;
                vertices.vertices.resize(pcd_idx_start_polygons[poly_id + 1] - pcd_idx_start_polygons[poly_id]);
                // populate points in the corresponding polygon
                for (int j = pcd_idx_start_polygons[poly_id]; j < pcd_idx_start_polygons[poly_id + 1]; ++j)
                    vertices.vertices[j - pcd_idx_start_polygons[poly_id]] = static_cast<uint32_t>(j);

                polygons.push_back(vertices);
            }
        }

        if (voronoi_centers_) voronoi_centers_->points.resize(dd);
    }

    qh_freeqhull(!qh_ALL);
    int curlong, totlong;
    qh_memfreeshort(&curlong, &totlong);

    Eigen::Affine3d transInverse = transform1.inverse();
    pcl::transformPointCloud(alpha_shape, alpha_shape, transInverse);
    xyz_centroid[0] = -xyz_centroid[0];
    xyz_centroid[1] = -xyz_centroid[1];
    xyz_centroid[2] = -xyz_centroid[2];
    pcl::demeanPointCloud(alpha_shape, xyz_centroid, alpha_shape);

    // also transform voronoi_centers_...
    if (voronoi_centers_) {
        pcl::transformPointCloud(*voronoi_centers_, *voronoi_centers_, transInverse);
        pcl::demeanPointCloud(*voronoi_centers_, xyz_centroid, *voronoi_centers_);
    }

    if (keep_information_) {
        // build a tree with the original points
        pcl::KdTreeFLANN<PointInT> tree(true);
        tree.setInputCloud(input_, indices_);

        std::vector<int> neighbor;
        std::vector<float> distances;
        neighbor.resize(1);
        distances.resize(1);

        // for each point in the concave hull, search for the nearest neighbor in the original point cloud
        hull_indices_.header = input_->header;
        hull_indices_.indices.clear();
        hull_indices_.indices.reserve(alpha_shape.points.size());

        for (size_t i = 0; i < alpha_shape.points.size(); i++) {
            tree.nearestKSearch(alpha_shape.points[i], 1, neighbor, distances);
            hull_indices_.indices.push_back(neighbor[0]);
        }

        // replace point with the closest neighbor in the original point cloud
        pcl::copyPointCloud(*input_, hull_indices_.indices, alpha_shape);
    }
}
#ifdef __GNUC__
#    pragma GCC diagnostic warning "-Wold-style-cast"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT> void pcl::ConcaveHull<PointInT>::performReconstruction(PolygonMesh& output) {
    // Perform reconstruction
    pcl::PointCloud<PointInT> hull_points;
    performReconstruction(hull_points, output.polygons);

    // Convert the PointCloud into a PCLPointCloud2
    pcl::toPCLPointCloud2(hull_points, output.cloud);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT>
void pcl::ConcaveHull<PointInT>::performReconstruction(std::vector<pcl::Vertices>& polygons) {
    pcl::PointCloud<PointInT> hull_points;
    performReconstruction(hull_points, polygons);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT>
void pcl::ConcaveHull<PointInT>::getHullPointIndices(pcl::PointIndices& hull_point_indices) const {
    hull_point_indices = hull_indices_;
}

#define PCL_INSTANTIATE_ConcaveHull(T) template class PCL_EXPORTS pcl::ConcaveHull<T>;





} // namespace pcl