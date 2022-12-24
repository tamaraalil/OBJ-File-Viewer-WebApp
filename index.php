<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Assignment 3</title>
    <link rel="stylesheet" href="./webgl.css" type="text/css">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/gl-matrix/2.8.1/gl-matrix-min.js"
      integrity="sha512-zhHQR0/H5SEBL3Wn6yYSaTTZej12z0hVZKOv3TwCUXT1z5qeqGcXJLLrbERYRScEDDpYIJhPC1fk31gqR783iQ=="
      crossorigin="anonymous" defer>
    </script>
    <script location.reload(); ></script>
    <script src="webgl-demo.js" defer ></script>
    <script src="loaddata.js" ></script>
  </head>

  <body>
    <!-- Create Databases -->
    <?php
      
    ?>

    <!-- Select File Buttons -->
    <?php
    echo <<<_END
    <HTML><BODY>
    <H3> <p style="text-align:center;">Object Viewer</p> </H3>
    <B>Upload Files to Server:</B>
    <FORM method="post" action="" enctype="multipart/form-data">
    Select .obj File: <input type="file" name="uploadfilename[]" id="uploadfilename">
    Select .ppm File: <input type="file" name="uploadfilename[]" id="uploadfilename">
    <input type="submit" value="Upload" name="submit">
    <BR>
    </FORM>
    _END;
    $name;
    if ($_FILES) {
      if(isset($_POST['submit'])){
        $allowed = array('ppm', 'obj');

        // Count total files
        $countfiles = count($_FILES['uploadfilename']['name']);
  
        for($i = 0; $i < $countfiles; $i++){ // Looping all files
          $filename = $_FILES['uploadfilename']['name'][$i];
          $err = 0;

          // Check if file is .ppm or .obj
          $ext = pathinfo($filename, PATHINFO_EXTENSION);
          if (!in_array($ext, $allowed)) {
              echo 'Error - must import only .ppm or .obj files.';
              $err = 1;
          }

          // Upload file
          if ($err == 0) {
            move_uploaded_file($_FILES['uploadfilename']['tmp_name'][$i],'/srv/talilovi/'.$filename);
            echo ('Uploaded ' .  $filename);
          }
          $name = $filename;
        }
      } 
    }
    echo ('</BODY></HTML>');
    ?>

    <!-- Transfer Button -->
    <?php
      echo <<<_END
      <HTML><BODY>
      <br>
      <B>Transfer Files to Database:</B>
      <FORM method="post" action="" enctype="multipart/form-data">
      Transfer .obj File: <input type="file" name="transferfilename[]" id="transferfilename">
      Transfer .ppm File: <input type="file" name="transferfilename[]" id="transferfilename">
      <input type="submit" value="Upload" name="transfer">
      </FORM>
      _END;

      $objects = array();
      // create connection to the database
      $username = "talilovi";
      $password = "1090984";
      $database = "talilovi";
      $server = "dursley.socs.uoguelph.ca";
      $conn = new mysqli($server, $username, $password, $database);

      $filesUploaded = 0;
      if ($_FILES) {
        if(isset($_POST['transfer'])){
          $allowed = array('ppm', 'obj');
          $objext = array('obj');
          $ppmext = array('ppm');
          $count = 0;
  
          // Count total files
          $countfiles = count($_FILES['transferfilename']['name']);
    
          for($i = 0; $i < $countfiles; $i++){ // Looping all files
            $filename = $_FILES['transferfilename']['name'][$i];
            $err = 0;

            // Add object name to array of objects in database
            $obj = substr($filename, 0, -4); // Remove ".obj" or ".ppm" from file chosen
            $objects = array();
            if (in_array($obj, $objects) === FALSE)  {
              $objects[] = $obj;
            }
  
            // Check if file is .ppm or .obj
            $ext = pathinfo($filename, PATHINFO_EXTENSION);
            if (!in_array($ext, $allowed)) {
                echo 'Error - must import only .ppm or .obj files.';
                $err = 1;
            }

            /*if (in_array($ext, $objext)) {
              $filesUploaded++;
            }
            if (in_array($ext, $ppmext)) {
              $filesUploaded++;
            }*/

            // Upload file
            if ($err == 0) {
                $dir = getcwd();
                move_uploaded_file($_FILES['transferfilename']['tmp_name'][$i],$dir.'/'.$filename);
                echo (' Uploaded ' .  $filename);
                $count++;
            }
            $name = $filename;
            //echo $name;
            echo "<br>";
            
            // Add data to database
            if ($count === 1) {
                // Display object on website
                $dir = getcwd();
                putenv("PYTHONPATH=".$dir."/"); // Set PYTHONPATH
                $obj = substr($name, 0, -4); // Remove ".obj" or ".ppm" from file chosen
                $cmd = "./a1 " . $obj; // Append name to C command
                exec($cmd, $output, $status); // Run C program
                // Resend files to client
                echo <<<_END
                    <script location.reload(); ></script>
                    _END;
                if ($status) {
                    // Error message if exec fails
                    //echo "exec failed status $status";
                } else {
                    // Resend files to client
                    echo <<<_END
                    <script location.reload(); ></script>
                    _END;
                    // Insert values to table in database
                    $id = rand();
                    if ($file = fopen("arrays.txt", "r")) {
                      while(!feof($file)) {
                          $line = fgets($file);
                          // Insert values to table in database
                          $sql = "insert into file values ('$id', '$obj', '$line')";
                          if ($conn->query($sql) === TRUE) {
                            //echo "New record created successfully $i <BR>";
                          } else {
                            echo "Error: " . $sql . "<br>" . $conn->error;
                          }
                          $id++;
                      }
                      $sql = "insert into objects values ('$id', '$obj')";
                      if ($conn->query($sql) === TRUE) {
                        //echo "New record created successfully $i <BR>";
                      } else {
                        echo "Error: " . $sql . "<br>" . $conn->error;
                      }
                      fclose($file);
                    }   
                }
            }
            $count = 0;
          }
        } 
      }

      // Connect to database
      $username = "talilovi";
      $password = "1090984";
      $database = "talilovi";
      $server = "dursley.socs.uoguelph.ca";
      $conn = new mysqli($server, $username, $password, $database);
      
      // Pull names of objects in database
      $flag = 0;
      $sql = $conn->query("select distinct name from objects");
      if($sql) {
        while($arr = $sql->fetch_assoc()) {
            $resultArr[] = $arr['name']; //storing values into an array
            $flag = 1;
        }
      }
      
      // Display # buttons
      if ($flag === 1) {
          foreach ($resultArr as $name) {
            echo <<<_END
            <HTML><BODY>
            
            <FORM method = "post">
            Display object: <input type="submit" value="Display $name" name="$name">
            </FORM>
            _END;
      
            if(array_key_exists($name, $_POST)) {
              display($name);
            }
          }
          $flag = 0;
          //$filesUploaded = 0;
      }

      function display($name) {
        // Connect to database
        $username = "talilovi";
        $password = "1090984";
        $database = "talilovi";
        $server = "dursley.socs.uoguelph.ca";
        $conn = new mysqli($server, $username, $password, $database);
        
        // Pull data for selected object from database
        echo "Displaying " . $name;
        $sql = "select * from file where object = \"$name\"";
        $result = mysqli_query($conn, $sql);

        // Output data to loaddata.js
        if (mysqli_num_rows($result) > 0) {
          $myfile = fopen("loaddata.js", "w") or die("Unable to open loaddata.js file.");
          while($row = mysqli_fetch_assoc($result)) {
            fwrite($myfile, $row["content"]);
          }
          fclose($myfile);
          //header("Refresh:0");
        } else {
          echo "0 results";
        }
      }
      echo ('</BODY></HTML>');
    ?>


    <!-- Drop Object Button -->
    <?php
      echo <<<_END
      <HTML><BODY>
      <br>
      <B>Delete Object from Database:</B>
      <FORM name="form" method="post" action="">
      Type name of object to delete: <input type="text" name="t1" id="t1" value="">
            <input type="submit" name="submit">
      </FORM>
      _END;
      //Type object name to delete: <input type="post" onclick="" value="Input obj name" name="drop">
      if(isset($_POST['submit'])){
          $obj = $_POST['t1']; //accessing value from the text field

          // Connecting to database
          $username = "talilovi";
          $password = "1090984";
          $database = "talilovi";
          $server = "dursley.socs.uoguelph.ca";
          $conn = new mysqli($server, $username, $password, $database);
          
          // Delete rows with obj name specified by user from tables
          $sql = "delete from file where object = \"$obj\"";
          if ($conn->query($sql) === TRUE) {
            //echo "Records for object $obj deleted successfully. <BR>";
          } else {
            echo "Error: " . $sql . "<br>" . $conn->error;
          }

          $sql = "delete from objects where name = \"$obj\"";
          if ($conn->query($sql) === TRUE) {
            header("Refresh:0");
            //echo "Records for object $obj deleted successfully. <BR>";
          } else {
            echo "Error: " . $sql . "<br>" . $conn->error;
          }
      }
      
      
    ?>

    <!-- Drop All Tables -->
    <?php
      // Create connection to the database
      $username = "talilovi";
      $password = "1090984";
      $database = "talilovi";
      $server = "dursley.socs.uoguelph.ca";
      $conn = new mysqli($server, $username, $password, $database);
      
      echo <<<_END
      <HTML><BODY>
      <br>
      <FORM method="post" action="" enctype="multipart/form-data">
      <B>Delete All Objects from Database:</B> <input type="submit" value="Delete" name="delete">
      </FORM>
      _END;

      if(array_key_exists('delete', $_POST)) {
          // Drop file table
          $sql =  "drop table file";
          if ($conn->query($sql) === TRUE) {
            //echo "Table file dropped successfully <BR>";
          } else {
            echo "Error dropping table: " . $conn->error;
          }

          // Drop file objects
          $sql =  "drop table objects";
          if ($conn->query($sql) === TRUE) {
            //echo "Table objects dropped successfully <BR>";
          } else {
            echo "Error dropping table: " . $conn->error;
          }

          header("Refresh:0");
          //echo "All tables have been dropped.<br>";
          $objects = array();
      }

      // Check connection
      if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
      }
      //echo "connected successfully <BR>";

      // Create file table
      $sql =  "create table file (id int not null auto_increment,
                object char(25),
                content char(255),
                primary key(id) )";

      if ($conn->query($sql) === TRUE) {
          //echo "Table table created successfully <BR>";
          $tables[] = "file";
      } else {
          //echo "Error creating table: " . $conn->error;
      }

      // Create objects table
      $sql =  "create table objects (id int not null auto_increment,
                name char(25),
                primary key(id) )";

      if ($conn->query($sql) === TRUE) {
        //echo "Table table created successfully <BR>";
        $tables[] = "objects";
      } else {
        //echo "Error creating table: " . $conn->error;
      }
      
    ?>

    <!-- Exit Button -->
    <div style = "text-align:right;">
        <form method = "POST" action="exit.html">
        <input type="submit" value = "Exit">
        </form>
    </div>
    <br><br>

    <!-- WebGL Canvas -->
    <div style = "text-align:center;">
        <canvas id="glcanvas" width="640" height="480"></canvas>
    </div>
  </body>
</html>
