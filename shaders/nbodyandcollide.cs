// compute shader for velocities and collisions
#version 440 compatibility

// array positions
layout(binding=4) buffer posbuf1 { vec4 pos1[]; };
layout(binding=5) buffer posbuf2 { vec4 pos2[]; };
layout(binding=6) buffer velbuf1 { vec4 vel1[]; };
layout(binding=7) buffer velbuf2 { vec4 vel2[]; };

// work group size
layout(local_size_x = 1024) in;

// time step
const float dt = 0.1;

// compute shader
void main()
{
    // global thread id
    uint gid = gl_GlobalInvocationID.x;

    pos1[gid].xyz += vel1[gid].xyz;
}