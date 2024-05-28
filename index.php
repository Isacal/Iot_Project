<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "temperature_db";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $temperature = $_POST['temperature'];
    $fanState = $_POST['fanState']; // Assuming 'fanState' is sent from ESP8266

    $sql = "INSERT INTO temperature_data (temperature, fan_state) VALUES ('$temperature', '$fanState')";

    if ($conn->query($sql) === TRUE) {
        echo "------------New record created successfully------------";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
}


$conn->close();
?>
