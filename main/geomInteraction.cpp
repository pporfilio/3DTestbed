#include "glwidget.h"
#include "glu.h"


void GLWidget::doSelect(vec2<int> screenLoc) {

    double mvmat[16];
    double projmat[16];
    int viewport[4];
    double dX, dY, dZ;

    double clickX = screenLoc.x;
    //y is positive in up direction for OpenGL
    double clickY = this->height() - screenLoc.y;


    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmat);
    glGetDoublev(GL_PROJECTION_MATRIX, projmat);


    if (m_selectMode == POLYGON) {

        gluUnProject(clickX, clickY, 0.0, mvmat, projmat, viewport, &dX, &dY, &dZ);
        vec3<float> rp1 = vec3<float>(dX, dY, dZ);
        gluUnProject(clickX, clickY, 1.0, mvmat, projmat, viewport, &dX, &dY, &dZ);
        vec3<float> rp2 = vec3<float>(dX, dY, dZ);

        Ray r(rp1, rp2 - rp1);

        Mesh::Intersection result = m_mesh->getIntersectingPoly(r);
        typedef std::vector<Mesh::Intersection>::const_iterator i_iter;

        if (result.p) {
            if (m_selectedPolys.find(result.p) == m_selectedPolys.end()) {
                result.p->pushMaterial(allMaterials["poly_select"]);
                m_selectedPolys.insert(result.p);
            }
        }
    } else if (m_selectMode == VERTEX) {

        std::vector<Vertex *> vertices = m_mesh->getVertices();
        typedef std::vector<Vertex *>::const_iterator v_iter;

        Vertex *nearestVert = NULL;
        float distSquared = INFINITY;
        for (v_iter v = vertices.begin(); v != vertices.end(); ++v) {
            gluProject((*v)->x(), (*v)->y(), (*v)->z(), mvmat, projmat, viewport, &dX, &dY, &dZ);
            float tmpDist2 = (dX - clickX) * (dX - clickX) +
                    (dY - clickY) * (dY - clickY);
            if (tmpDist2 < distSquared) {
                distSquared = tmpDist2;
                nearestVert = (*v);
            }
        }
        //if nearestVert is NULL then distSquared will be INFINITY
        if (sqrt(distSquared) < 25) {
            if (m_selectedVerts.find(nearestVert) == m_selectedVerts.end()) {
                nearestVert->pushMaterial(allMaterials["vert_select"]);
                m_selectedVerts.insert(nearestVert);
            }
        }
    } else if (m_selectMode == EDGE) {

        std::vector<Edge *> edges = m_mesh->getEdges();

        float dist;
        Edge *nearestEdge = getNearestScreenspaceEdge(edges, vec2<float>(clickX, clickY), dist);

        if (dist < 25) {
            nearestEdge->pushMaterial(allMaterials["edge_select"]);
            m_selectedEdges.insert(nearestEdge);
        }

    } else if (m_selectMode == OBJECT) {

    } else {
        printf("invalid select mode!\n");
    }
}




vec3<float> GLWidget::getSelectionCenter() {
    typedef std::set<Vertex *>::const_iterator v_iter;
    typedef std::vector<Vertex *>::const_iterator vv_iter;
    typedef std::set<Edge *>::const_iterator e_iter;
    typedef std::set<Polygon *>::const_iterator p_iter;

    switch(m_selectMode) {
    case VERTEX: {
        vec3<float> sum(0, 0, 0);
        for (v_iter v = m_selectedVerts.begin(); v != m_selectedVerts.end(); ++v) {
            sum += (*v)->asVec3();
        }
        return sum / m_selectedVerts.size();
    }
    case EDGE: {
        vec3<float> sum(0, 0, 0);
        for (e_iter e = m_selectedEdges.begin(); e != m_selectedEdges.end(); ++e) {
            sum += (*e)->v1()->asVec3();
            sum += (*e)->v2()->asVec3();
        }
        return sum / (m_selectedEdges.size() * 2);
    }
    case POLYGON: {
        vec3<float> sum(0, 0, 0);
        int count;
        for (p_iter p = m_selectedPolys.begin(); p != m_selectedPolys.end(); ++p) {
            std::vector<Vertex *> verts = (*p)->getVertices();
            for (vv_iter v = verts.begin(); v != verts.end(); ++v) {
                sum += (*v)->asVec3();
                count ++;
            }
        }
        return sum / count;
    }
    case OBJECT: {

        break;
    }
    }

    //default to origin, since every location is valid unless I use NaNs,
    //which can cause big problems
    return vec3<float>(0, 0, 0);
}
