
#include "chess.h"

#include "node.h"

//  v1------v0
//  |       |
//  |       |
//  |       |
//  v2------v3

static GLuint quad_indices[] = {0, 1, 2,  0, 2, 3};


static GLfloat quad_normals[] = {
  0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front
};

static GLfloat quad_vertices[] = {
  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,    // v0-v1-v2-v3 front
};


static GLfloat quad_uvs[] =
    {
     1.0, 1.0, // v0
     0.0, 1.0, // v1
     0.0, 0.0, // v2
     1.0, 0.0, // v3
    };

#define CELL_SIZE 10.0

void make_grid(float *verts, float vert_capacity,
               u32 *indices, float index_capacity,
               float *normals, float normal_capacity,
               float *colors, float color_capacity,
               int width, int height)
{
    int c = 0;
    for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++, c++) {
        int grid_ind = y * height + x;
        int vind = grid_ind * ARRAY_SIZE(quad_vertices);
        int iind = grid_ind * ARRAY_SIZE(quad_indices);
        int i = 0;

        for (i = 0; i < 4; i++) {
            int nv = i*3;
            assert(vind+2+nv < vert_capacity);
            assert(vind+2+nv < color_capacity);
            assert(vind+2+nv < normal_capacity);
            verts[vind+0+nv] = (quad_vertices[0+nv] + x) * CELL_SIZE;
            verts[vind+1+nv] = (quad_vertices[1+nv] + y) * CELL_SIZE;
            verts[vind+2+nv] = quad_vertices[2+nv];

            // recenter
            verts[vind+0+nv] -= (width * CELL_SIZE)/2.0 - 0.5 * CELL_SIZE;
            verts[vind+1+nv] -= (height * CELL_SIZE)/2.0 - 0.5 * CELL_SIZE;
            verts[vind+2+nv] -= 0.5;

            normals[vind+0+nv] = quad_normals[0+nv];
            normals[vind+1+nv] = quad_normals[1+nv];
            normals[vind+2+nv] = quad_normals[2+nv];

            float checkers = (x ^ y) & 1;
            colors[vind+0+nv] = checkers;
            colors[vind+1+nv] = checkers;
            colors[vind+2+nv] = checkers;
        }

        for (i = 0; i < 2; i++) {
            int nv = i*3;
            assert(iind+2+nv < index_capacity);
            indices[iind+0+nv] = quad_indices[0+nv] + 4*c;
            indices[iind+1+nv] = quad_indices[1+nv] + 4*c;
            indices[iind+2+nv] = quad_indices[2+nv] + 4*c;
        }
    }
    }
}

static void make_chessboard_geom(geometry_id *id)
{
    struct make_geometry mkgeom;
    init_make_geometry(&mkgeom);

    static float verts[12*8*8]; // array_size(quad) 12 * 8 * 8
    static float colors[12*8*8]; // array_size(quad) 12 * 8 * 8
    static float normals[12*8*8]; // array_size(quad) 12 * 8 * 8
    static u32 indices[6*8*8]; // array_size(quad) 6 * 8 * 8

    make_grid(verts, ARRAY_SIZE(verts),
              indices, ARRAY_SIZE(indices),
              normals, ARRAY_SIZE(normals),
              colors, ARRAY_SIZE(colors),
              8, 8);

    mkgeom.indices  = indices;
    mkgeom.vertices = verts;
    mkgeom.normals  = normals;
    mkgeom.colors   = colors;
    mkgeom.num_indices = ARRAY_SIZE(indices);
    mkgeom.num_verts = ARRAY_SIZE(verts)/3;
    mkgeom.num_uv_components = 0;

    init_id(id);
    struct geometry *geom = new_geometry(id);
    make_buffer_geometry(&mkgeom, geom);
    check_gl();
}

static void position_piece(struct entity *piece, int x, int y)
{
    assert(x < 8);
    assert(y < 8);

    struct node *node = get_node(&piece->node_id);
    struct model *model = get_model(&piece->model_id);
    struct geometry *geom = get_geometry(&model->geom_id);
    assert(node);

    // reset to a1
    node->pos[0] = -3.5 * CELL_SIZE;
    node->pos[1] = -3.5 * CELL_SIZE;
    node->pos[2] = 0.0;

    node->pos[0] += x * CELL_SIZE;
    node->pos[1] += y * CELL_SIZE;
}

static void setup_pieces(node_id *chessboard) {
    struct model *pirate_officer;
    get_static_model(model_pirate_officer, &pirate_officer);

    // make pawn model
    model_id pawn_model_id;
    init_id(&pawn_model_id);
    struct model *pawn_model = new_model(&pawn_model_id);
    pawn_model->geom_id = pirate_officer->geom_id;
    pawn_model->shader = CHESS_PIECE_PROGRAM;

    // pawns
    for (int i = 0; i < 16; i++) {
        struct entity *pawn = new_entity(NULL);
        struct node *pawn_node = get_node(&pawn->node_id);
        pawn->model_id = pawn_model_id;
        pawn->flags |= ENT_IS_WHITE | ENT_CASTS_SHADOWS;

        node_scale(pawn_node, 5.0);
        if (i >= 8) {
            pawn->flags &= ~ENT_IS_WHITE;
            node_rotate(pawn_node, V3(0.0,0.0,135.0));
        }
        node_attach(&pawn->node_id, chessboard);

        position_piece(pawn, i % 8, i < 8 ? 1 : 6);
    }

    for (int i = 0; i < 4; i++) {
        // rooks
        struct entity *rook = new_entity(NULL);
        struct model *rook_model;
        struct node *rook_node = get_node(&rook->node_id);
        rook->model_id = get_static_model(model_tower, &rook_model);
        rook->flags |= ENT_CASTS_SHADOWS | ENT_IS_WHITE;
        rook_model->shader = CHESS_PIECE_PROGRAM;
        node_attach(&rook->node_id, chessboard);
        /* node_rotate(rook_node, V3(0.0,0.0,20.0)); */
        if (i >= 2) {
            rook->flags &= ~ENT_IS_WHITE;
            node_rotate(rook_node, V3(0.0,0.0,135.0));
        }
        position_piece(rook, i % 2 ? 0 : 7, i < 2 ? 0 : 7);
        node_scale(rook_node, 0.25);
    }


}


void chess_scene(struct game *game)
{
    struct entity *ent = new_entity(NULL);
    struct node *node  = get_node(&ent->node_id); assert(node);
    struct entity *player = get_player(&game->test_resources);

    ent->model_id = get_static_model(model_icosphere, NULL);
    node_set_label(node, "sphere");

    //
    // setup chessboard
    //
    geometry_id chessboard_geom;
    make_chessboard_geom(&chessboard_geom);

    model_id chessboard_model_id;
    init_id(&chessboard_model_id);

    struct model *chessboard_model = new_model(&chessboard_model_id);
    chessboard_model->shader = DEFAULT_PROGRAM;
    chessboard_model->geom_id = chessboard_geom;

    player->model_id = chessboard_model_id;
    player->flags &= ~ENT_CASTS_SHADOWS;

    //
    // setup camera
    //
    struct spherical *coords = &game->test_resources.orbit_camera.coords;
    coords->radius = 72.0;
    coords->inclination = 0.5;
    coords->azimuth = -7.86;

    struct node *pnode = get_node(&player->node_id);
    node_translate(pnode, V3(240.0, 252.0, 373.0));
    pnode->orientation[2] = -0.005;
    pnode->orientation[3] = -1.0;

    node_id *cam_id = &game->test_resources.orbit_camera.node_id;
    struct node *cam_node = get_node(cam_id); assert(cam_node);

    setup_pieces(&player->node_id);

    node_recalc(pnode);

    //player 1284.539062 1111.104126 14.273574

    // show terrain
    //terrain_ent->flags |= ENT_INVISIBLE;

    // show player
    /* player->flags |= ENT_INVISIBLE; */
}
