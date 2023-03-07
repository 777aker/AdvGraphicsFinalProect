// compute shader for velocities and collisions
#version 440 compatibility

// positions of the objects
layout(binding = 4) buffer posbuf1 { vec4 pos1 []; };
layout(binding = 5) buffer posbuf2 { vec4 pos2 []; };
// vectors of the objects
layout(binding = 6) buffer velbuf1 { vec4 vel1 []; };
layout(binding = 7) buffer velbuf2 { vec4 vel2 []; };
// colors of the objects
layout(binding = 8) buffer colbuf { vec4 col []; };
// I need to pos and vel buffers because I need each array to
// look at the same information for collisions to work, so they basically
// all look at array 1, say ok we all have this information lets each
// do our math independently, then we will change array 2 which is the next frame

// work group size
layout(local_size_x = 1024) in;
// change in time per frame
const float dt = .01;
// how much velocity is lost in a collision
const float collisionloss = .8;
// how much red we gain from a collision
const float rgain = .1;

// array size
uniform int n;
// helper function for doing the gravity and collisions
void gravandcollide(uint effect, int other)
{
    // first off, we need our vector that points in the direction of our
    // collision and since this is a sphere colliding with a sphere
    // also behaves as our surface normal
    vec3 c2 = pos1[other].xyz - pos1[effect].xyz;
    // actual distance from each other for calculations
    float D = length(c2 + vel1[other].xyz * dt - vel1[effect].xyz * dt);
    if(D > 2)
    {
        //col[effect].r -= 1 * rloss;
        if(length(c2) < 2)
        {
            // if your length is less than 2 just do nothing and let them
            // phase through each other bc otherwise all the math gets real
            // funky, this only matters on initialization bc the random positions
            // means sometimes spheres start inside of each other randomly
            return;
        } else
        {
            // do gravity, which is the least of my problems and really easy
            vel2[effect].xyz += 1 / (D*D*D) * c2 * dt;
            return;
        }
    }
    // so now we also need the other guys normal to add to ourself
    vec3 c1 = normalize(pos1[effect].xyz - pos1[other].xyz);
    // also normalize ourself, waited till after if statement
    // bc most of the time we can skip this expensive nonsense which is nice
    c2 = normalize(c2);
    
    // wait a sec, you mean it took me 20 hours to figure out this
    // I think i got caught up on the 3d, this sucks
    // take our velocity add to it the velocity of the other guy projected
    // in the direction of the collision and subtract from our velocity
    // what we give to the other guy. subtract our velocity in direction
    // of collision, add other guys velocity in direction of collision
    vel2[effect].xyz += c1 * dot(c1, vel1[other].xyz) * collisionloss;
    vel2[effect].xyz -= c2 * dot(c2, vel1[effect].xyz) * collisionloss;
    //vel2[effect].xyz -= c1 * vel1[effect].xyz * .8 - c1 * length(vel1[effect].xyz) * .2;
        
    //vel2[effect].xyz += c2 * vel1[other].xyz * .8 + c1 * length(vel1[other].xyz) * .2;
    // move the objects every so slightly away from each other so funky
    // nonsense doesn't happen and they are no longer colliding
    pos2[effect].xyz += c1 * (2 - D) * 1.2;
    // there was a collision so increase our r value a little
    col[effect].r += 1 * rgain;

    return;
}

// compute shader
void main()
{
    // global thread id
    uint gid = gl_GlobalInvocationID.x;
    // set our next frame to our current frame
    // that way there isn't weird jumping around each frame
    vel2[gid].xyz = vel1[gid].xyz;
    pos2[gid].xyz = pos1[gid].xyz;
    // reset the r value
    //col[gid].r = .2;
    //bool collided = false;
    // go through every particle and do the collide and gravity helper function
    for(int i = 0; i < n; i++)
    {
        if(i != gid)
        {
            gravandcollide(gid, i);
        }
    }
    // update position with our new velocity
    pos2[gid].xyz += vel2[gid].xyz * dt;
    // clamp our color bc it can go insane in either direction if you don't
    col[gid].r = clamp(col[gid].r, .1, 1.0);
    // subtract a nice one every frame so if it hasn't collided in a little bit
    // it just goes back to 0
    col[gid].r -= .1;
}

/* this is some bad math that makes no sense and doesn't work
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
/* this is some old gravity and collision code
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