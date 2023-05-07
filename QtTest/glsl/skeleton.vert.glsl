#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

out vec3 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.

uniform mat4 u_BindMats[100];
uniform mat4 u_TransMats[100];
uniform mat2x4 u_DualQuats[100];

//wrong way: in int[2] vs_JointIds;
//don't know how to pass array to the shader
//in int vs_JointIds[2];
//in float vs_Weights[2];

in ivec2 vs_JointIds;
in vec2 vs_Weights;

vec4 quatMultiply(vec4 q1,vec4 q2){
    float w1 = q1[0];
    float x1 = q1[1];
    float y1 = q1[2];
    float z1 = q1[3];
    float w2 = q2[0];
    float x2 = q2[1];
    float y2 = q2[2];
    float z2 = q2[3];
    float w = w1*w2 - (x1*x2 + y1*y2 + z1*z2);
    float x = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    float y = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    float z = w1*z2 + x1*y2 - y1*x2 + z1*w2;
    return vec4(w,x,y,z);
}
mat2x4 dQuatMultiply(mat2x4 dq1,mat2x4 dq2){
   vec4 q1r = dq1[0];
   vec4 q1c = dq1[1];
   vec4 q2r = dq2[0];
   vec4 q2c = dq2[1];

   return mat2x4(quatMultiply(q1r,q2r),quatMultiply(q1r,q2c)+quatMultiply(q1c,q2r));
}

void main()
{
    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    int id1 = vs_JointIds[0];
    int id2 = vs_JointIds[1];
    bool isBound = !(id1==-1&&id2==-1);//because there will be default value
    mat4 f1 = mat4(0.f);
    mat4 f2 = mat4(0.f);
    mat4 transformMat = mat4(1.f);
    if(isBound){
        if(id1!=-1){
          f1 = u_TransMats[id1]*u_BindMats[id1]*vs_Weights[0];
        }
        if(id2!=-1){
          f2 = u_TransMats[id2]*u_BindMats[id2]*vs_Weights[1];
        }
        transformMat = f1+f2;
    }else{
        f1 = mat4(1.f);
        transformMat = f1+f2;
    }
    //dual quat
    if(isBound){
        vec4 qr = vec4(0,0,0,0);
        vec4 qc = vec4(0,0,0,0);
        if(id1!=-1){
            vec4 qr1 = u_DualQuats[id1][0];
            vec4 qc1 = u_DualQuats[id1][1];
            qr += qr1*vs_Weights[0];
            qc += qc1*vs_Weights[0];
        }
        if(id2!=-1){
            vec4 qr2 = u_DualQuats[id2][0];
            vec4 qc2 = u_DualQuats[id2][1];
            qr += qr2*vs_Weights[1];
            qc += qc2*vs_Weights[1];
        }
        //normalize
        float qrLen = sqrt(qr.x*qr.x + qr.y*qr.y + qr.z*qr.z + qr.w*qr.w);
        qr = qr/qrLen;
        qc = qc/qrLen;
        //endnormalize
        float w = qr[0];
        float x = qr[1];
        float y = qr[2];
        float z = qr[3];
        float we = qc[0];
        float xe = qc[1];
        float ye = qc[2];
        float ze = qc[3];
        float t0 = 2*(-we*x+xe*w-ye*z+ze*y);
        float t1 = 2*(-we*y+xe*z+ye*w-ze*x);
        float t2 = 2*(-we*z-xe*y+ye*x+ze*w);
        transformMat[0] = vec4(1-2*y*y-2*z*z,2*x*y+2*w*z,2*x*z-2*w*y,0);
        transformMat[1] = vec4(2*x*y-2*w*z,1-2*x*x-2*z*z,2*y*z+2*w*x,0);
        transformMat[2] = vec4(2*x*z+2*w*y,2*y*z-2*w*x,1-2*x*x-2*y*y,0);
        transformMat[3] = vec4(t0,t1,t2,1);
    }else{
        f1 = mat4(1.f);
        transformMat = f1+f2;
    }

    //end dual quat

    mat3 invTranspose = mat3(u_ModelInvTr);
    mat3 jointRot = mat3(transformMat);
    fs_Nor = vec4(jointRot * invTranspose * vec3(vs_Nor), 0);
                                                            // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

     vec4 modelposition = u_Model * transformMat * vs_Pos;   // Temporarily store the transformed vertex positions for use below
     fs_Pos = modelposition.xyz;
     gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                              // used to render the final positions of the geometry's vertices

}
