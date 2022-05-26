#include "BallPivotingAlgorithm.h"
#include <algorithm>
#include <deque>
#include <optional>
#include <string>
#include <iostream>
#include <sstream>
#include <numeric>
#include <numbers>

struct MeshEdge;

struct MeshPoint {
    GeneratedPoint point;
    bool used = false;
    std::vector<MeshEdge*> edges;
};

enum class EdgeStatus {
    active,
    inner,
    boundary
};

struct MeshEdge {
    MeshPoint* a;
    MeshPoint* b;
    MeshPoint* opposite;
    GeneratedPoint center;
    MeshEdge* prev;
    MeshEdge* next;
    EdgeStatus status = EdgeStatus::active;
};

struct MeshFace : std::array<MeshPoint*, 3>{
    GeneratedPoint GetNormUnitVector() const {
        float x1 = (*this)[0]->point.x - (*this)[1]->point.x,
                y1 = (*this)[0]->point.y - (*this)[1]->point.y,
                z1 = (*this)[0]->point.z - (*this)[1]->point.z;

        float x2 = (*this)[0]->point.x - (*this)[2]->point.x,
                y2 = (*this)[0]->point.y - (*this)[2]->point.y,
                z2 = (*this)[0]->point.z - (*this)[2]->point.z;

        float xNorm = y1 * z2 - z1 * y2,
                yNorm = z1 * x2 - x1 * z2,
                zNorm = x1 * y2 - y1 * x2;

        float vectorMagnitude = sqrt(xNorm * xNorm + yNorm * yNorm + zNorm * zNorm);

        float xUnit = xNorm / vectorMagnitude,
                yUnit = yNorm / vectorMagnitude,
                zUnit = zNorm / vectorMagnitude;

        return { xUnit, yUnit, zUnit, -1.0, -1.0, -1.0};
    }
};

using Vector3f = GeneratedPoint;
using Cell = std::vector<MeshPoint>;

struct Grid {
    Grid(const std::vector<GeneratedPoint>& points, float radius)
        : cell_size_(radius * 2) {
        lower_ = points.front();
        upper_ = points.front();

        for (const auto& p : points) {
            for (auto i = 0; i < 3; i++) {
                lower_[i] = std::min(lower_[i], p[i]);
                upper_[i] = std::max(upper_[i], p[i]);
            }
        }

        dims_ = GetMaxVector(Vector3f{CeilVector((upper_ - lower_) / cell_size_)}, Vector3f{1});

        cells_.resize(dims_.x * dims_.y * dims_.z);
        for (const auto& p : points)
            GetCell(GetCellIndex(p)).push_back({p, false, {}});
    }

    Vector3f GetCellIndex(const GeneratedPoint& point) {
        const auto index = IntCastVector((point - lower_) / cell_size_);
        return ClampVectors(index, Vector3f{}, dims_ - 1);
    }

    Cell& GetCell(const Vector3f& index) {
        return cells_[index.z * dims_.x * dims_.y + index.y * dims_.x + index.x];
    }

    std::vector<MeshPoint*> SphericalNeighborhood(GeneratedPoint point, std::initializer_list<Vector3f> ignore) {
        std::vector<MeshPoint*> result;
        const auto centerIndex = GetCellIndex(point);
        result.reserve(GetCell(centerIndex).size() * 27);
        for (auto xOff : {-1, 0, 1}) {
            for (auto yOff : {-1, 0, 1}) {
                for (auto zOff : {-1, 0, 1}) {
                    const auto index = centerIndex + IntCastVector(Vector3f{static_cast<float>(xOff),
                                                                            static_cast<float>(yOff),
                                                                            static_cast<float>(zOff)});
                    if (index.x < 0 || index.x >= dims_.x) continue;
                    if (index.y < 0 || index.y >= dims_.y) continue;
                    if (index.z < 0 || index.z >= dims_.z) continue;
                    for (auto& p : GetCell(index))
                        if (GetSquaredLength(p.point - point) < cell_size_ * cell_size_ && std::find(begin(ignore), end(ignore), p.point) == end(ignore))
                            result.push_back(&p);
                }
            }
        }
        return result;
    }

    GeneratedPoint lower_;
    GeneratedPoint upper_;
    float cell_size_;
    GeneratedPoint dims_;
    std::vector<Cell> cells_;
};

std::optional<Vector3f> ComputeBallCenter(MeshFace f, float radius) {
    const Vector3f ac = f[2]->point - f[0]->point;
    const Vector3f ab = f[1]->point - f[0]->point;
    const Vector3f abXac = CrossProduct(ab, ac);
    const Vector3f toCircumCircleCenter = (CrossProduct(abXac, ab) * DotProduct(ac, ac) + CrossProduct(ac, abXac) * DotProduct(ab, ab)) / (2 * DotProduct(abXac, abXac));
    const Vector3f circumCircleCenter = f[0]->point + toCircumCircleCenter;

    const auto heightSquared = radius * radius - DotProduct(toCircumCircleCenter, toCircumCircleCenter);
    if (heightSquared < 0)
        return {};
    auto ballCenter = circumCircleCenter + f.GetNormUnitVector() * std::sqrt(heightSquared);
    return ballCenter;
}

bool BallIsEmpty(const Vector3f& ballCenter, const std::vector<MeshPoint*>& points, float radius) {
    return !std::any_of(begin(points), end(points), [&](MeshPoint* p) {
        return GetSquaredLength(p->point - ballCenter) < radius * radius - 1e-4f;
    });
}

struct SeedResult {
    MeshFace f;
    Vector3f ballCenter;
};

std::optional<SeedResult> FindSeedTriangle(Grid& grid, float radius) {
    for (auto& cell : grid.cells_) {
        const auto avgNormal =
                GetUnitVector(std::accumulate(begin(cell),
                                              end(cell),
                                              Vector3f{},
                                              [](const Vector3f& acc, const MeshPoint& p) {
            return acc + Vector3f{ p.point.n_x, p.point.n_y, p.point.n_z};
        })
                              );

        for (auto& p1 : cell) {
            auto neighborhood = grid.SphericalNeighborhood(p1.point, {p1.point});
            std::sort(begin(neighborhood), end(neighborhood), [&](MeshPoint* a, MeshPoint* b) {
                return GetRegularLength(a->point - p1.point) < GetRegularLength(b->point - p1.point);
            });

            for (auto& p2 : neighborhood) {
                for (auto& p3 : neighborhood) {
                    if (p2 == p3) continue;
                    MeshFace f{{&p1, p2, p3}};
                    if (DotProduct(f.GetNormUnitVector(), avgNormal) < 0)
                        continue;
                    const auto ballCenter = ComputeBallCenter(f, radius);
                    if (ballCenter && BallIsEmpty(ballCenter.value(), neighborhood, radius)) {
                        p1.used = true;
                        p2->used = true;
                        p3->used = true;
                        return SeedResult{f, ballCenter.value()};
                    }
                }
            }
        }
    }
    return {};
}

std::optional<MeshEdge*> GetActiveEdge(std::vector<MeshEdge*>& front) {
    while (!front.empty()) {
        auto* e = front.back();
        if (e->status == EdgeStatus::active)
            return e;
        front.pop_back();
    }
    return {};
}

struct PivotResult {
    MeshPoint* p;
    Vector3f center;
};

std::optional<PivotResult> BallPivot(const MeshEdge* e, Grid& grid, float radius) {
    const auto m = (e->a->point + e->b->point) / 2.0f;
    const auto oldCenterVec = GetUnitVector(e->center - m);
    auto neighborhood = grid.SphericalNeighborhood(m, {e->a->point, e->b->point, e->opposite->point});

    static auto counter = 0;
    counter++;

    auto smallestAngle = std::numeric_limits<float>::max();
    MeshPoint* pointWithSmallestAngle = nullptr;
    Vector3f centerOfSmallest{};
    std::stringstream ss;

    auto i = 0;
    for (const auto& p : neighborhood) {
        i++;
        auto newFaceNormal = Triangle{e->b->point, e->a->point, p->point}.GetNormUnitVector();

        if (DotProduct(newFaceNormal, {p->point.n_x, p->point.n_y, p->point.n_z}) < 0)
            continue;

        const auto c = ComputeBallCenter(MeshFace{{e->b, e->a, p}}, radius);
        if (!c) {
            continue;
        }

        const auto newCenterVec = GetUnitVector(c.value() - m);
        const auto newCenterFaceDot = DotProduct(newCenterVec, newFaceNormal);
        if (newCenterFaceDot < 0) {
            continue;
        }

        for (const auto* ee : p->edges) {
            const auto* otherPoint = ee->a == p ? ee->b : ee->a;
            if (ee->status == EdgeStatus::inner && (otherPoint == e->a || otherPoint == e->b)) {
                goto nextneighbor;
            }
        }

        {
            auto angle = std::acos(std::clamp(DotProduct(oldCenterVec, newCenterVec), -1.0f, 1.0f));
            if (DotProduct(CrossProduct(newCenterVec, oldCenterVec), e->a->point - e->b->point) < 0)
                angle += M_PI;
            if (angle < smallestAngle) {
                smallestAngle = angle;
                pointWithSmallestAngle = p;
                centerOfSmallest = c.value();
            }

        }

    nextneighbor:;
    }

    if (smallestAngle != std::numeric_limits<float>::max()) {
        if (BallIsEmpty(centerOfSmallest, neighborhood, radius)) {
            return PivotResult{pointWithSmallestAngle, centerOfSmallest};
        }
    }

    return {};
}

bool NotUsed(const MeshPoint* p) {
    return !p->used;
}

bool OnFront(const MeshPoint* p) {
    return std::any_of(begin(p->edges), end(p->edges), [&](const MeshEdge* e) {
        return e->status == EdgeStatus::active;
    });
}

void Remove(MeshEdge* edge) {
    edge->status = EdgeStatus::inner;
}

void OutputTriangle(MeshFace f, std::vector<Triangle>& triangles) {
    triangles.push_back({f[0]->point, f[1]->point, f[2]->point});
}

std::tuple<MeshEdge*, MeshEdge*>
Join(MeshEdge* e_ij, MeshPoint* o_k, const Vector3f& o_k_ballCenter, std::vector<MeshEdge*>& front, std::deque<MeshEdge>& edges) {
    auto& e_ik = edges.emplace_back(MeshEdge{e_ij->a, o_k, e_ij->b, o_k_ballCenter});
    auto& e_kj = edges.emplace_back(MeshEdge{o_k, e_ij->b, e_ij->a, o_k_ballCenter});

    e_ik.next = &e_kj;
    e_ik.prev = e_ij->prev;
    e_ij->prev->next = &e_ik;
    e_ij->a->edges.push_back(&e_ik);

    e_kj.prev = &e_ik;
    e_kj.next = e_ij->next;
    e_ij->next->prev = &e_kj;
    e_ij->b->edges.push_back(&e_kj);

    o_k->used = true;
    o_k->edges.push_back(&e_ik);
    o_k->edges.push_back(&e_kj);

    front.push_back(&e_ik);
    front.push_back(&e_kj);
    Remove(e_ij);

    return {&e_ik, &e_kj};
}

void Glue(MeshEdge* a, MeshEdge* b, std::vector<MeshEdge*>& front) {

    if (a->next == b && a->prev == b && b->next == a && b->prev == a) {
        Remove(a);
        Remove(b);
        return;
    }

    if (a->next == b && b->prev == a) {
        a->prev->next = b->next;
        b->next->prev = a->prev;
        Remove(a);
        Remove(b);
        return;
    }
    if (a->prev == b && b->next == a) {
        a->next->prev = b->prev;
        b->prev->next = a->next;
        Remove(a);
        Remove(b);
        return;
    }

    a->prev->next = b->next;
    b->next->prev = a->prev;
    a->next->prev = b->prev;
    b->prev->next = a->next;
    Remove(a);
    Remove(b);
}

MeshEdge* FindReverseEdgeOnFront(MeshEdge* edge) {
    for (auto& e : edge->a->edges)
        if (e->a == edge->b)
            return e;
    return nullptr;
}

std::vector<Triangle> DoBallPivotingAlgorithm(const std::vector<GeneratedPoint>& points, float radius) {
    Grid grid(points, radius);

    const auto seedResult = FindSeedTriangle(grid, radius);
    if (!seedResult) {
        std::cerr << "No seed triangle found\n";
        return {};
    }

    std::vector<Triangle> triangles;
    std::deque<MeshEdge> edges;

    auto [seed, ballCenter] = seedResult.value();
    OutputTriangle(seed, triangles);
    auto& e0 = edges.emplace_back(MeshEdge{seed[0], seed[1], seed[2], ballCenter});
    auto& e1 = edges.emplace_back(MeshEdge{seed[1], seed[2], seed[0], ballCenter});
    auto& e2 = edges.emplace_back(MeshEdge{seed[2], seed[0], seed[1], ballCenter});
    e0.prev = e1.next = &e2;
    e0.next = e2.prev = &e1;
    e1.prev = e2.next = &e0;
    seed[0]->edges = { &e0, &e2 };
    seed[1]->edges = { &e0, &e1 };
    seed[2]->edges = { &e1, &e2 };
    std::vector<MeshEdge*> front{&e0, &e1, &e2};


    while (auto e_ij = GetActiveEdge(front)) {
        const auto o_k = BallPivot(e_ij.value(), grid, radius);
        if (o_k && (NotUsed(o_k->p) || OnFront(o_k->p))) {
            OutputTriangle({{e_ij.value()->a, o_k->p, e_ij.value()->b}}, triangles);
            auto [e_ik, e_kj] = Join(e_ij.value(), o_k->p, o_k->center, front, edges);
            if (auto* e_ki = FindReverseEdgeOnFront(e_ik)) Glue(e_ik, e_ki, front);
            if (auto* e_jk = FindReverseEdgeOnFront(e_kj)) Glue(e_kj, e_jk, front);
        } else {
            e_ij.value()->status = EdgeStatus::boundary;
        }
    }

    return triangles;
}
