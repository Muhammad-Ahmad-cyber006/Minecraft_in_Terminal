#pragma once

// 3D vector structure for positions and directions
typedef struct Vector {
    float x, y, z;
} vect;

// 2D angular vector for camera orientation
typedef struct Vector2 {
    float psi; // pitch (up/down rotation)
    float phi; // yaw (left/right rotation)
} vect2;

// Combined position and view structure for player
typedef struct PosView {
    vect pos;   // Players position in 3D space
    vect2 view; // Players view angles 
} player_pos_view;

// Information about a raycast hit face
typedef struct FaceInfo {
    int axis;// Which axis the face normal points along (0=x, 1=y, 2=z)
    float u, v;// Fractional position within the faces 2D plane (each in [0,1))
} FaceInfo;