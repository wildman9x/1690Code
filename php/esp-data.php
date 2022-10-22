<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP8266 Data</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@700&display=swap" rel="stylesheet">
<!-- bootstrap -->
<!-- CSS only -->
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-Zenh87qX5JnK2Jl0vWa8Ck2rdkQ2Bzep5IDxbcnCeuOxjzrPF/et3URy9Bv1WTRi" crossorigin="anonymous">
<!-- JavaScript Bundle with Popper -->
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-OERcA2EqjJCMA+/3y+gxIOqMEjwtxJY7qPCqsdltbNJuaOe923+mo//f6V8Qbsw3" crossorigin="anonymous"></script>
<!-- set the font of the whole body to space grotesk -->
<style>
body {
font-family: 'Space Grotesk', sans-serif;
/* off white background */
background-color: #f5f5f5;
}
.container {
margin-top: 5em;
margin-bottom: 5em;

width: 60%;
}
/* set border for each table cell */
td {
border: 0.3em solid black;
}

table {
  border: 0.3em solid black;
/* drop shadow right and bottom */
  box-shadow: 0.3em 0.3em 0.3em solid black;
}

</style>

<script>
  // create function deleteAll() to delete all data from the database
  // use sql command DELETE * FROM SensorData
</script>
<!-- refresh the site every 4 second using php -->
<meta http-equiv="refresh" content="4">

</head>  
<body>
<?php


/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

$servername = "localhost";

// REPLACE with your Database name
$dbname = "id19744016_health_monitor";
// REPLACE with Database user
$username = "id19744016_root";
// REPLACE with Database user password
$password = "PxTe1}me)7B))0KA";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, sensor, location, pulse, spo2, studentStatus, reading_time FROM SensorData ORDER BY id DESC";

// create a title for the table
// create a button to delete all the data in the table using php
echo "<div class='container' >
<h1>Students' health status</h1>
</div>";


// put the table in a container
echo '<div class="container">';
echo '<table class = "table" cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>Sensor</td> 
        <td>Location</td> 
        <td>Heart rate</td> 
        <td>SPO2 level</td>
        <td>Student Status</td>
        <td>Timestamp</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["sensor"];
        $row_location = $row["location"];
        $row_pulse = $row["pulse"];
        $row_spo2 = $row["spo2"]; 
        $row_studentStatus = $row["studentStatus"];
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      // if student status is Good, highlight the row in green
      // if student status is Bad, highlight the row in red
      // if student status is Warning, highlight the row in yellow
      if ($row_studentStatus == "Good") { 
        echo '<tr bgcolor="#BCE29E">';
      } else if ($row_studentStatus == "Bad") {
        echo '<tr bgcolor="#FF8787">';
      } else if ($row_studentStatus == "Warning") {
        echo '<tr bgcolor="#FFD384">';
      } else {
        echo '<tr>';
      }
        echo '<td>' . $row_id . '</td>
              <td>' . $row_sensor . '</td>
              <td>' . $row_location . '</td>
              <td>' . $row_pulse . '</td>
              <td>' . $row_spo2 . '</td>
              <td>' . $row_studentStatus . '</td>
              <td>' . $row_reading_time . '</td>
          </tr>';
        // echo '<tr> 
        //         <td>' . $row_id . '</td> 
        //         <td>' . $row_sensor . '</td> 
        //         <td>' . $row_location . '</td> 
        //         <td>' . $row_pulse . '</td> 
        //         <td>' . $row_spo2 . '</td>
        //         <td>' . $row_studentStatus . '</td>
                
        //         <td>' . $row_reading_time . '</td> 
        //       </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</div>
</body>
</html>