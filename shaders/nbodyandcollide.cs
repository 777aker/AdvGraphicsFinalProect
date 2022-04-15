// compute shader for velocities and collisions
#version 440 compatibility

// array positions
layout(binding=4) buffer posbuf1 { vec4 pos1[]; };
layout(binding=5) buffer posbuf2 { vec4 pos2[]; };
layout(binding=6) buffer velbuf1 { vec4 vel1[]; };
layout(binding=7) buffer velbuf2 { vec4 vel2[]; };
layout(binding=8) buffer colbuf { vec4 col[]; };

// work group size
layout(local_size_x = 512) in;

// time step
const float dt = 0.1;

// array size
uniform int n;

// compute shader
void main()
{
    // global thread id
    uint gid = gl_GlobalInvocationID.x;
    
    float dt = 0.002;
    vec3 F = vec3(0,0,0);
    float density = 0;
    vel2[gid].xyz = vec3(0,0,0);
    vec3 vel = vel1[gid].xyz;
    for(int i = 0; i < n; i++)
    {
        vec3 D = pos1[i].xyz - pos1[gid].xyz;
        float d = sqrt(dot(D,D)) + 1;
        if(d < 1.2) {
            density += 1;
            vel *= .999;
            vel2[gid].xyz += vel1[i].xyz * .5;
        }
        F += 1/(d*d*d)*D;
    }
    float val = 1/(density * density * density + 1);
    vel2[gid].xyz *= val * .0001;
    vel2[gid].xyz = vel + dt*F + vel2[gid].xyz;
    pos2[gid].xyz = pos1[gid].xyz + dt*vel1[gid].xyz;
    col[gid].r = density*.1;
    col[gid].b = 1-density*.1;
}