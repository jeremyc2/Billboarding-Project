#define SDL_MAIN_HANDLED
#include "/usr/local/include/SDL2/SDL.h"
#include "gl.h"

// compile command
// g++ gl.cpp -lopengl -lglew -lSDL2
// TODO move bunny back
int main(int aargc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("OpenGL Program", 200, 200,
        SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    SDL_GLContext cont = SDL_GL_CreateContext(win);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    bool success = true;
    string vertexShader;
    string secondVertexShader;
    string fragmentShader;

    int vertexHandle;
    int secondVertexHandle;
    int fragHandle;
    int programHandle;
    int secondProgramHandle;

    success &= parseFile((char*) "vertexBillboard.vs", vertexShader); //vertexBillboard //phongVertex2
    success &= parseFile((char*) "phongFrag2.fs", fragmentShader);
    success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexHandle);
    success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragHandle);
    success &= compileProgram(vertexHandle, fragHandle, programHandle);

    success &= parseFile((char*) "phongVertex2.vs", secondVertexShader); //vertexPhong //phongVertex2
    success &= compileShader(secondVertexShader.c_str(), GL_VERTEX_SHADER, secondVertexHandle);
    success &= compileProgram(secondVertexHandle, fragHandle, secondProgramHandle);

    /********************************
    * Object Loader Bunny
    * ******************************/
    vector<material> materials;
    vector<vertexData> vertexBuffer;
    vector<material> materials1;
    vector<vertexData> vertexBuffer1;

    bool hasUV;
    bool hasNormal;
    bool hasUV1;
    bool hasNormal1;

    success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);
    success &= getObjData("floor.obj", materials1, vertexBuffer1, hasUV1, hasNormal1);

    // Build out a single array of floats
    int stride1 = 3 + 2 * hasUV1 + 3 * hasNormal1;
    //int vertexBufferNumBytes1 = stride1 * vertexBuffer1.size() * sizeof(float);
    //float *vertexBufferData1 = (float *)(malloc(vertexBufferNumBytes));

    // Build out a single array of floats
    int stride = 3 + 2 * hasUV + 3 * hasNormal;
    int vertexBufferNumBytes = (stride + stride1) * (vertexBuffer.size() + vertexBuffer1.size()) 
                               * sizeof(float);
    float *vertexBufferData = (float *)(malloc(vertexBufferNumBytes));

    int i = 0;
    // Join data into interleaved buffer;
    for (int vb = 0; vb < vertexBuffer.size(); vb++) {
        vertexBufferData[i++] = vertexBuffer[vb].vert[0];
        vertexBufferData[i++] = vertexBuffer[vb].vert[1];
        vertexBufferData[i++] = vertexBuffer[vb].vert[2];

        if (hasUV) {
            vertexBufferData[i++] = vertexBuffer[vb].uv[0];
            vertexBufferData[i++] = vertexBuffer[vb].uv[1];
        }

        if (hasNormal) {
            vertexBufferData[i++] = vertexBuffer[vb].normal[0];
            vertexBufferData[i++] = vertexBuffer[vb].normal[1];
            vertexBufferData[i++] = vertexBuffer[vb].normal[2];
        }

    }

    //i = 0;
    // Join data into interleaved buffer;
    for (int vb = 0; vb < vertexBuffer1.size(); vb++) {
        vertexBufferData[i++] = vertexBuffer1[vb].vert[0];
        vertexBufferData[i++] = vertexBuffer1[vb].vert[1];
        vertexBufferData[i++] = vertexBuffer1[vb].vert[2];

        if (hasUV1) {
            vertexBufferData[i++] = vertexBuffer1[vb].uv[0];
            vertexBufferData[i++] = vertexBuffer1[vb].uv[1];
        }

        if (hasNormal1) {
            vertexBufferData[i++] = vertexBuffer1[vb].normal[0];
            vertexBufferData[i++] = vertexBuffer1[vb].normal[1];
            vertexBufferData[i++] = vertexBuffer1[vb].normal[2];
        }
    }

    // TODO add in the other texture
    vector<int> textureIDs;
    for (int mat = 0; mat < materials.size(); mat++) {
        std::cout << mat << endl;
        int tmp;
        material m = materials[mat];
        success &= loadTexture(m.map_Kd, tmp);
        textureIDs.push_back(tmp);
    }
    for (int mat = 0; mat < materials1.size(); mat++) {
                std::cout << mat << endl;
        int tmp1;
        material m1 = materials1[mat];
        success &= loadTexture(m1.map_Kd, tmp1);
        textureIDs.push_back(tmp1);
    }

    validate(success, (char*) "Setup OpenGL Program");

    /********************************
    * Vertex Buffer Object Bunny
    * ******************************/
   int VBO;
   glGenBuffers(1, (GLuint *) &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

    /********************************
    * Vertex Buffer Object Floor
    * ******************************/
//    int VBO1;
//    glGenBuffers(1, (GLuint *) &VBO1);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

//    glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes1, vertexBufferData1, GL_STATIC_DRAW);

    /********************************
    * Attribute Handles
    * ******************************/
   int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
   int aUVHandle = glGetAttribLocation(programHandle, "a_UV");
   int aNormalHandle = glGetAttribLocation(programHandle, "a_Normal");

   /********************************
   * Uniform Handles
   * ******************************/
   int uMatrixHandle = glGetUniformLocation(programHandle, "u_Matrix");
   int uModelHandle = glGetUniformLocation(programHandle, "u_Model");
   int uViewHandle = glGetUniformLocation(programHandle, "u_View");
   int uProjHandle = glGetUniformLocation(programHandle, "u_Projection");
   int uTextureHandle = glGetUniformLocation(programHandle, "u_Texture");
   int uAmbientStrengthHandle = glGetUniformLocation(programHandle, "ambientStrength");
   int uDiffuseStrengthHandle = glGetUniformLocation(programHandle, "diffuseStrength");
   int uSpecularStrengthHandle = glGetUniformLocation(programHandle, "specularStrength");
   int uLightPosHandle = glGetUniformLocation(programHandle, "lightPos");
   int uViewPosHandle = glGetUniformLocation(programHandle, "viewPos");
   int uLightColorHandle = glGetUniformLocation(programHandle, "lightColor");
   //int uThresholdHandle = glGetUniformLocation(programHandle, "u_Threshold");

   /**********************************
    * 
    *  Second program
    * ********************************/

    /********************************
    * Attribute Handles
    * ******************************/
   int aPositionHandle1 = glGetAttribLocation(secondProgramHandle, "a_Position");
   int aUVHandle1 = glGetAttribLocation(secondProgramHandle, "a_UV");
   int aNormalHandle1 = glGetAttribLocation(secondProgramHandle, "a_Normal");

   /********************************
   * Uniform Handles
   * ******************************/
   int uMatrixHandle1 = glGetUniformLocation(secondProgramHandle, "u_Matrix");
   int uModelHandle1 = glGetUniformLocation(secondProgramHandle, "u_Model");
   int uViewHandle1 = glGetUniformLocation(secondProgramHandle, "u_View");
   int uProjHandle1 = glGetUniformLocation(secondProgramHandle, "u_Projection");
   int uTextureHandle1 = glGetUniformLocation(secondProgramHandle, "u_Texture");
   int uAmbientStrengthHandle1 = glGetUniformLocation(secondProgramHandle, "ambientStrength");
   int uDiffuseStrengthHandle1 = glGetUniformLocation(secondProgramHandle, "diffuseStrength");
   int uSpecularStrengthHandle1 = glGetUniformLocation(secondProgramHandle, "specularStrength");
   int uLightPosHandle1 = glGetUniformLocation(secondProgramHandle, "lightPos");
   int uViewPosHandle1 = glGetUniformLocation(secondProgramHandle, "viewPos");
   int uLightColorHandle1 = glGetUniformLocation(secondProgramHandle, "lightColor");
   //int uThresholdHandle1 = glGetUniformLocation(secondProgramHandle, "u_Threshold");

   // MVP Matrix
   mat4 mvp;
   mat4 model;
   mat4 view;
   mat4 proj;
   
   // Setup Camera Data
   myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

   int numDraw1 = vertexBuffer.size();
   int numDraw2 =vertexBuffer1.size();

   //int numDraw1 = vertexBuffer1.size();

   bool running = true;
   int frame = 0;

   while(running) {
       processUserInputs(running);

       {
           // Clear buffers
           glClearColor(0, 0, 0, 1.0);
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

           if(currentCycle == BILLBOARD)
            glUseProgram(programHandle);
           else
            glUseProgram(secondProgramHandle);

            frame++;

           /*************************
            * Setup Attributes
            *************************/
           glBindBuffer(GL_ARRAY_BUFFER, VBO);
           //glBindBuffer(GL_ARRAY_BUFFER, VBO1);

           glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
           glEnableVertexAttribArray(aPositionHandle);

           glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(0 + 3*sizeof(float)));
           glEnableVertexAttribArray(aUVHandle);

           glVertexAttribPointer(aNormalHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(0 + 5*sizeof(float)));
           glEnableVertexAttribArray(aNormalHandle);

           /*************************
            * Setup Uniforms
            *************************/
           // Update texture
           glActiveTexture(GL_TEXTURE0 + 0);
           glBindTexture(GL_TEXTURE_2D, textureIDs[0]);



           //glUniform1i(uTextureHandle, 0);
           //glUniform1f(uThresholdHandle, threshold);

           if(currentCycle == BILLBOARD)
            {
                glUseProgram(programHandle);
                glUniform3f(uAmbientStrengthHandle, 0.2,0.2,0.2);
                glUniform3f(uDiffuseStrengthHandle, 0.64,0.64,0.64);
                glUniform4f(uSpecularStrengthHandle, 0.9,0.9,0.9,0.9);
                glUniform3f(uLightPosHandle, 1.0,1.2,4.0);
                glUniform3f(uViewPosHandle, 1.0,1.0,1.0);
                glUniform4f(uLightColorHandle, 1.0,1.0,1.0,1.0);

                setupMVP(mvp,model,view,proj);
                glUniformMatrix4fv(uMatrixHandle, 1, false, &mvp[0][0]);
                glUniformMatrix4fv(uModelHandle, 1, false, &model[0][0]);
                glUniformMatrix4fv(uViewHandle, 1, false, &view[0][0]);
                glUniformMatrix4fv(uProjHandle, 1, false, &proj[0][0]);
            }
           else
           {
                glUseProgram(secondProgramHandle);
                glUniform3f(uAmbientStrengthHandle1, 0.2,0.2,0.2);
                glUniform3f(uDiffuseStrengthHandle1, 0.64,0.64,0.64);
                glUniform4f(uSpecularStrengthHandle1, 0.9,0.9,0.9,0.9);
                glUniform3f(uLightPosHandle1, 1.0,1.2,4.0);
                glUniform3f(uViewPosHandle1, 1.0,1.0,1.0);
                glUniform4f(uLightColorHandle1, 1.0,1.0,1.0,1.0);

                setupMVP(mvp,model,view,proj);
                glUniformMatrix4fv(uMatrixHandle1, 1, false, &mvp[0][0]);
                glUniformMatrix4fv(uModelHandle1, 1, false, &model[0][0]);
                glUniformMatrix4fv(uViewHandle1, 1, false, &view[0][0]);
                glUniformMatrix4fv(uProjHandle1, 1, false, &proj[0][0]);
               
           }

           glDrawArrays(GL_TRIANGLES, 0, numDraw1);

           // Update texture
           glActiveTexture(GL_TEXTURE0 + 0);
           glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
           glUseProgram(secondProgramHandle);
           glUniform3f(uAmbientStrengthHandle1, 0.2,0.2,0.2);
           glUniform3f(uDiffuseStrengthHandle1, 0.64,0.64,0.64);
           glUniform4f(uSpecularStrengthHandle1, 0.9,0.9,0.9,0.9);
           glUniform3f(uLightPosHandle1, 1.0,1.2,4.0);
           glUniform3f(uViewPosHandle1, 1.0,1.0,1.0);
           glUniform4f(uLightColorHandle1, 1.0,1.0,1.0,1.0);

           setupMVP1(mvp,model,view,proj);
           glUniformMatrix4fv(uMatrixHandle1, 1, false, &mvp[0][0]);
           glUniformMatrix4fv(uModelHandle1, 1, false, &model[0][0]);
           glUniformMatrix4fv(uViewHandle1, 1, false, &view[0][0]);
           glUniformMatrix4fv(uProjHandle1, 1, false, &proj[0][0]);


           glDrawArrays(GL_TRIANGLES, numDraw1, numDraw2);
       }

       SDL_GL_SwapWindow(win);
   }

   free(vertexBufferData);
   //free(vertexBufferData1);
   
   return 0;
}