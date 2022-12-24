	// original code taken from MDN Web Docs
	// https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial/Lighting_in_WebGL

    var cubeRotation = 0.0;

    main();
    
    //
    // Start here
    //
    function main() {
      const canvas = document.querySelector('#glcanvas');
      const gl = canvas.getContext('webgl');
    
      // If we don't have a GL context, give up now
    
      if (!gl) {
        alert('Unable to initialize WebGL. Your browser or machine may not support it.');
        return;
      }
    
      // Vertex shader program
    
      const vsSource = `
        attribute vec4 aVertexPosition;
        attribute vec3 aVertexNormal;
        attribute vec2 aTextureCoord;
        uniform mat4 uNormalMatrix;
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;
        varying highp vec2 vTextureCoord;
        varying highp vec3 vLighting;
        void main(void) {
          gl_Position = uProjectionMatrix * uModelViewMatrix * aVertexPosition;
          vTextureCoord = aTextureCoord;
          // Apply lighting effect
          highp vec3 ambientLight = vec3(0.3, 0.3, 0.3);
          highp vec3 directionalLightColor = vec3(1, 1, 1);
          highp vec3 directionalVector = normalize(vec3(0.85, 0.8, 0.75));
          highp vec4 transformedNormal = uNormalMatrix * vec4(aVertexNormal, 1.0);
          highp float directional = max(dot(transformedNormal.xyz, directionalVector), 0.0);
          vLighting = ambientLight + (directionalLightColor * directional);
        }
      `;
    
      // Fragment shader program
    
      const fsSource = `
        varying highp vec2 vTextureCoord;
        varying highp vec3 vLighting;
        uniform sampler2D uSampler;
        void main(void) {
          highp vec4 texelColor = texture2D(uSampler, vTextureCoord);
          gl_FragColor = vec4(texelColor.rgb * vLighting, texelColor.a);
        }
      `;
    
      // Initialize a shader program; this is where all the lighting
      // for the vertices and so forth is established.
      const shaderProgram = initShaderProgram(gl, vsSource, fsSource);
    
      // Collect all the info needed to use the shader program.
      // Look up which attributes our shader program is using
      // for aVertexPosition, aVertexNormal, aTextureCoord,
      // and look up uniform locations.
      const programInfo = {
        program: shaderProgram,
        attribLocations: {
          vertexPosition: gl.getAttribLocation(shaderProgram, 'aVertexPosition'),
          vertexNormal: gl.getAttribLocation(shaderProgram, 'aVertexNormal'),
          textureCoord: gl.getAttribLocation(shaderProgram, 'aTextureCoord'),
        },
        uniformLocations: {
          projectionMatrix: gl.getUniformLocation(shaderProgram, 'uProjectionMatrix'),
          modelViewMatrix: gl.getUniformLocation(shaderProgram, 'uModelViewMatrix'),
          normalMatrix: gl.getUniformLocation(shaderProgram, 'uNormalMatrix'),
          uSampler: gl.getUniformLocation(shaderProgram, 'uSampler'),
        }
      };
    
      // Here's where we call the routine that builds all the
      // objects we'll be drawing.
      const buffers = initBuffers(gl);
    
      const texture = loadTexture(gl, 'cubetexture.png');
    
      var then = 0;
    
      // Draw the scene repeatedly
      function render(now) {
        now *= 0.001;  // convert to seconds
        const deltaTime = now - then;
        then = now;
    
        drawScene(gl, programInfo, buffers, texture, deltaTime);
    
        requestAnimationFrame(render);
      }
      requestAnimationFrame(render);
    }
    
    //
    // initBuffers
    //
    // Initialize the buffers we'll need. For this demo, we just
    // have one object -- a simple three-dimensional cube.
    //
    function initBuffers(gl) {
    
      // Create a buffer for the cube's vertex positions.
    
      const positionBuffer = gl.createBuffer();
    
      // Select the positionBuffer as the one to apply buffer
      // operations to from here out.
    
      gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    
      // Now create an array of positions for the cube.
    
      // load vertex data from loaddata.js
      const positions = loadvertices();
    
      // Now pass the list of positions into WebGL to build the
      // shape. We do this by creating a Float32Array from the
      // JavaScript array, then use it to fill the current buffer.
    
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
    
      // Set up the normals for the vertices, so that we can compute lighting.
    
      const normalBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
    
      // load normal data from loaddata.js
      const vertexNormals = loadnormals();
    
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexNormals),
                    gl.STATIC_DRAW);
    
      // Now set up the texture coordinates for the faces.
    
      const textureCoordBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);
    
      // load texture coordinates data from loaddata.js
      const textureCoordinates = loadtextcoords();
    
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),
                    gl.STATIC_DRAW);
    
      // Build the element array buffer; this specifies the indices
      // into the vertex arrays for each face's vertices.
    
      const indexBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    
      // This array defines each face as two triangles, using the
      // indices into the vertex array to specify each triangle's
      // position.
    
      // load indices from loaddata.js
      const indices = loadvertexindices();
    
      // Now send the element array to GL
    
      gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,
          new Uint16Array(indices), gl.STATIC_DRAW);
    
      return {
        position: positionBuffer,
        normal: normalBuffer,
        textureCoord: textureCoordBuffer,
        indices: indexBuffer,
      };
    }
    
    //
    // Initialize a texture and load an image.
    // When the image finished loading copy it into the texture.
    //
    function loadTexture(gl, url) {
      const texture = gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D, texture);
    
      // Because images have to be download over the internet
      // they might take a moment until they are ready.
      // Until then put a single pixel in the texture so we can
      // use it immediately. When the image has finished downloading
      // we'll update the texture with the contents of the image.
      const level = 0;
      const internalFormat = gl.RGBA;
    
      // load height and width functions from loaddata.js
      const width = loadwidth();
      const height = loadheight();
    
      const border = 0;
      const srcFormat = gl.RGBA;
      const srcType = gl.UNSIGNED_BYTE;
    
      // load texture from loaddata.js
      const pixel = loadtexture();
  
      gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
                    width, height, border, srcFormat, srcType,
                    pixel);
    
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    
      return texture;
    }
    
    function isPowerOf2(value) {
      return (value & (value - 1)) == 0;
    }
    
    //
    // Draw the scene.
    //
    function drawScene(gl, programInfo, buffers, texture, deltaTime) {
      gl.clearColor(0.0, 0.0, 0.0, 1.0);  // Clear to black, fully opaque
      gl.clearDepth(1.0);                 // Clear everything
      gl.enable(gl.DEPTH_TEST);           // Enable depth testing
      gl.depthFunc(gl.LEQUAL);            // Near things obscure far things
    
      // Clear the canvas before we start drawing on it.
    
      gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
      // Create a perspective matrix, a special matrix that is
      // used to simulate the distortion of perspective in a camera.
      // Our field of view is 45 degrees, with a width/height
      // ratio that matches the display size of the canvas
      // and we only want to see objects between 0.1 units
      // and 100 units away from the camera.
    
      const fieldOfView = 45 * Math.PI / 180;   // in radians
      const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
      const zNear = 0.1;
      const zFar = 100.0;
      const projectionMatrix = mat4.create();
    
      // note: glmatrix.js always has the first argument
      // as the destination to receive the result.
      mat4.perspective(projectionMatrix,
                       fieldOfView,
                       aspect,
                       zNear,
                       zFar);
    
      // Set the drawing position to the "identity" point, which is
      // the center of the scene.
      const modelViewMatrix = mat4.create();
    
      // read distance to translate object away from viewpoing
      // from configuration file
       distance = getdistance();
     
      // Now move the drawing position a bit to where we want to
      // start drawing the square.
    
      mat4.translate(modelViewMatrix,     // destination matrix
                     modelViewMatrix,     // matrix to translate
                     [-0.0, 0.0, distance]);  // amount to translate
      mat4.rotate(modelViewMatrix,  // destination matrix
                  modelViewMatrix,  // matrix to rotate
                  cubeRotation,     // amount to rotate in radians
                  [0, 0, 1]);       // axis to rotate around (Z)
      mat4.rotate(modelViewMatrix,  // destination matrix
                  modelViewMatrix,  // matrix to rotate
                  cubeRotation * .7,// amount to rotate in radians
                  [0, 1, 0]);       // axis to rotate around (X)
    
      const normalMatrix = mat4.create();
      mat4.invert(normalMatrix, modelViewMatrix);
      mat4.transpose(normalMatrix, normalMatrix);
    
      // Tell WebGL how to pull out the positions from the position
      // buffer into the vertexPosition attribute
      {
        const numComponents = 3;
        const type = gl.FLOAT;
        const normalize = false;
        const stride = 0;
        const offset = 0;
        gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
        gl.vertexAttribPointer(
            programInfo.attribLocations.vertexPosition,
            numComponents,
            type,
            normalize,
            stride,
            offset);
        gl.enableVertexAttribArray(
            programInfo.attribLocations.vertexPosition);
      }
    
      // Tell WebGL how to pull out the texture coordinates from
      // the texture coordinate buffer into the textureCoord attribute.
      {
        const numComponents = 2;
        const type = gl.FLOAT;
        const normalize = false;
        const stride = 0;
        const offset = 0;
        gl.bindBuffer(gl.ARRAY_BUFFER, buffers.textureCoord);
        gl.vertexAttribPointer(
            programInfo.attribLocations.textureCoord,
            numComponents,
            type,
            normalize,
            stride,
            offset);
        gl.enableVertexAttribArray(
            programInfo.attribLocations.textureCoord);
      }
    
      // Tell WebGL how to pull out the normals from
      // the normal buffer into the vertexNormal attribute.
      {
        const numComponents = 3;
        const type = gl.FLOAT;
        const normalize = false;
        const stride = 0;
        const offset = 0;
        gl.bindBuffer(gl.ARRAY_BUFFER, buffers.normal);
        gl.vertexAttribPointer(
            programInfo.attribLocations.vertexNormal,
            numComponents,
            type,
            normalize,
            stride,
            offset);
        gl.enableVertexAttribArray(
            programInfo.attribLocations.vertexNormal);
      }
    
      // Tell WebGL which indices to use to index the vertices
      gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.indices);
    
      // Tell WebGL to use our program when drawing
    
      gl.useProgram(programInfo.program);
    
      // Set the shader uniforms
    
      gl.uniformMatrix4fv(
          programInfo.uniformLocations.projectionMatrix,
          false,
          projectionMatrix);
      gl.uniformMatrix4fv(
          programInfo.uniformLocations.modelViewMatrix,
          false,
          modelViewMatrix);
      gl.uniformMatrix4fv(
          programInfo.uniformLocations.normalMatrix,
          false,
          normalMatrix);
    
      // Specify the texture to map onto the faces.
    
      // Tell WebGL we want to affect texture unit 0
      gl.activeTexture(gl.TEXTURE0);
    
      // Bind the texture to texture unit 0
      gl.bindTexture(gl.TEXTURE_2D, texture);
    
      // Tell the shader we bound the texture to texture unit 0
      gl.uniform1i(programInfo.uniformLocations.uSampler, 0);
    
      {
        const vertexCount = getVertexCount();
        const type = gl.UNSIGNED_SHORT;
        const offset = 0;
        gl.drawElements(gl.TRIANGLES, vertexCount, type, offset);
      }
    
      // Update the rotation for the next draw
    
      cubeRotation += deltaTime;
    }
    
    //
    // Initialize a shader program, so WebGL knows how to draw our data
    //
    function initShaderProgram(gl, vsSource, fsSource) {
      const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
      const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);
    
      // Create the shader program
    
      const shaderProgram = gl.createProgram();
      gl.attachShader(shaderProgram, vertexShader);
      gl.attachShader(shaderProgram, fragmentShader);
      gl.linkProgram(shaderProgram);
    
      // If creating the shader program failed, alert
    
      if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
        return null;
      }
    
      return shaderProgram;
    }
    
    //
    // creates a shader of the given type, uploads the source and
    // compiles it.
    //
    function loadShader(gl, type, source) {
      const shader = gl.createShader(type);
    
      // Send the source to the shader object
    
      gl.shaderSource(shader, source);
    
      // Compile the shader program
    
      gl.compileShader(shader);
    
      // See if it compiled successfully
    
      if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
        gl.deleteShader(shader);
        return null;
      }
    
      return shader;
    }
    