<?php
  /*
    This file assumes there is already a MySQL server running on the local
    machine, and that the password is sql_pass.secret
  */
  require 'sql_pass.secret';

  $conn = mysqli_connect($sql_host, $sql_username, $sql_pass);
  if (!$conn) {
    echo "Could not connect to DB";
  } else {
    $create_db_sql = "CREATE DATABASE $sql_db;";
    if (!mysqli_query($conn, $create_db_sql)) {
      echo "Error creating databse: " . mysqli_error($conn);
    } else {
      $create_table_sql = "CREATE TABLE users_keys (
        User_ID VARCHAR(8) UNIQUE,
        Spotify_Code VARCHAR(1024)
      );";
      mysqli_query($conn, "USE $sql_db;");
      if (mysqli_query($conn, $create_table_sql)) {
        echo "Database and table created successfully.";
      } else {
        echo "Error: " . mysqli_error($conn);
      }
    }
  }
?>
