// compute shader for velocities and collisions
#version 440 compatibility

// array positions
layout(binding = 4) buffer posbuf1 { vec4 pos1 []; };
layout(binding = 5) buffer posbuf2 { vec4 pos2 []; };
layout(binding = 6) buffer velbuf1 { vec4 vel1 []; };
layout(binding = 7) buffer velbuf2 { vec4 vel2 []; };
layout(binding = 8) buffer colbuf { vec4 col []; };

// work group size
layout(local_size_x = 512) in;

const float dt = .01;
const float gforce = .01;

// array size
uniform int n;

vec3 project(vec3 u, vec3 v)
{
    return v * (dot(u,v) / dot(v, v));
}

void gravandcollide(uint effect, int other)
{
    vec3 c2 = pos1[other].xyz - pos1[effect].xyz;
    float D = length(c2 + vel1[other].xyz * dt - vel1[effect].xyz * dt);
    if(D > 2)
    {
        if(length(c2) < 2)
        {
            // can put repel here
            return;
        } else
        {
            vel2[effect].xyz += 1 / (D*D*D) * c2 * gforce;
            return;
        }
    }

    vec3 c1 = normalize(pos1[effect].xyz - pos1[other].xyz);
    c2 = normalize(c2);
    
    // wait a sec, you mean it took me 20 hours to figure out this
    // I think i got caught up on the 3d
    vel2[effect].xyz += c1 * dot(c1, vel1[other].xyz) * .9;
    vel2[effect].xyz -= c2 * dot(c2, vel1[effect].xyz) * .9;
    //vel2[effect].xyz -= c1 * vel1[effect].xyz * .8 - c1 * length(vel1[effect].xyz) * .2;
        
    //vel2[effect].xyz += c2 * vel1[other].xyz * .8 + c1 * length(vel1[other].xyz) * .2;

    pos2[effect].xyz += c1 * (2 - D) * 1.2;

    return;
}

// compute shader
void main()
{
    // global thread id
    uint gid = gl_GlobalInvocationID.x;

    vel2[gid].xyz = vel1[gid].xyz;
    pos2[gid].xyz = pos1[gid].xyz;
    
    //bool collided = false;

    for(int i = 0; i < n; i++)
    {
        if(i != gid)
        {
            gravandcollide(gid, i);
        }
    }

    pos2[gid].xyz += vel2[gid].xyz * dt;
    
}

/*
    vec3 D = pos1[other].xyz - pos1[effect].xyz;
    vec3 DV = vel1[other].xyz - vel1[effect].xyz;
    vec3 DVD = abs(D + DV);
    if(DVD.x > 2 || DVD.y > 2 || DVD.z > 2)
        return false;
    if(length(DVD) > 2 || length(D) < 1)
        return false;

    float xyz = length(D) * length(vel1[other].xyz);
    if (xyz == 0)
    {
        vel2[effect].xyz = vel1[other].xyz;
        return true;
    }
    
    xyz = acos(
        //fract(
        dot(D, vel1[other].xyz) /
        xyz
        //)
        );
    
    float vcs = length(vel1[other].xyz)*cos(xyz);
    
    float xz = length(D.xz) * length(vel1[other].xz);
    if (xz == 0)
    {
        xz = 1;
    } else
    {
        xz = acos(
        //fract(
        dot(D.xz, vel1[other].xz) /
        xz
        //)
        );
    }

    float xy = length(D.xy) * length(vel1[other].xy);
    if (xy == 0)
    {
        xy = 1;
    } else
    {
        xy = acos(
        //fract(
        dot(D.xy, vel1[other].xy) /
        xy
        //)
        );   
    }
    vec3 vc = vec3(0, 0, 0);
    vc.x = vcs * sin(xz) * cos(xy);
    vc.y = vcs * sin(xz) * sin(xy);
    vc.z = vcs * cos(xz);
    vel2[effect].xyz = vel1[effect].xyz - vc;
    pos2[effect].xyz += vel2[effect].xyz * dt;

    return true;
    */
/*
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
    */